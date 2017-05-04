#include <Engine/Executor/ContextMenu/PlayFromConstraintInScenario.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <Process/Process.hpp>
#include <Engine/Executor/ConstraintComponent.hpp>
#include <Engine/Executor/ProcessComponent.hpp>
#include <Engine/Executor/ScenarioComponent.hpp>
#include <Engine/Executor/BaseScenarioComponent.hpp>

#include <Scenario/Process/Algorithms/Accessors.hpp>

namespace Engine
{
namespace Execution
{

struct dfs_visitor_state
{
  tsl::hopscotch_set<Scenario::ConstraintModel*> constraints;
  tsl::hopscotch_set<Scenario::TimeNodeModel*> nodes;
};

struct dfs_visitor : public boost::default_dfs_visitor
{
  // because these geniuses of boost decided to pass the visitor by value...
  std::shared_ptr<dfs_visitor_state> state{std::make_shared<dfs_visitor_state>()};

  void discover_vertex(Scenario::Graph::vertex_descriptor i, const Scenario::Graph& g)
  {
    state->nodes.insert(g[i]);
  }
  void examine_edge(Scenario::Graph::edge_descriptor i, const Scenario::Graph& g)
  {
    state->constraints.insert(g[i]);
  }

};

tsl::hopscotch_set<Scenario::ConstraintModel*>
PlayFromConstraintScenarioPruner::constraintsToKeep() const
{
  Scenario::TimenodeGraph g{scenar};

  // First find the vertex matching the time node after our constraint
  auto vertex = g.vertices().at(&Scenario::endTimeNode(constraint, scenar));

  // Do a depth-first search from where we're starting
  dfs_visitor vis;
  std::vector<boost::default_color_type> color_map(boost::num_vertices(g.graph()));

  boost::depth_first_visit(g.graph(), vertex, vis,
                           boost::make_iterator_property_map(
                             color_map.begin(),
                             boost::get(boost::vertex_index, g.graph()),
                             color_map[0]));

  // Add the first constraint
  vis.state->constraints.insert(&constraint);
  return vis.state->constraints;
}

bool PlayFromConstraintScenarioPruner::toRemove(const tsl::hopscotch_set<Scenario::ConstraintModel*>& toKeep, Scenario::ConstraintModel& cst) const
{
  auto c_addr = &cst;
  return (toKeep.find(c_addr) == toKeep.end()) && (c_addr != &constraint);
}

void PlayFromConstraintScenarioPruner::operator()(const Context& exec_ctx)
{
  // We prune all the superfluous components of the scenario, ie the one that aren't either
  // the started constraint, or the ones following it.

  // First build a vector with all the constraints that we want to keep.
  auto toKeep = constraintsToKeep();

  // Get the constraints in the scenario execution
  auto process_ptr = dynamic_cast<const Process::ProcessModel*>(&scenar);
  auto& source_procs = exec_ctx.scenario.baseConstraint().processes();
  auto scenar_proc_it = source_procs.find(process_ptr->id());

  ISCORE_ASSERT(scenar_proc_it != source_procs.end());

  auto scenar_comp = dynamic_cast<ScenarioComponent*>((*scenar_proc_it).second.get());
  auto scenar_constraints = scenar_comp->constraints();
  ConstraintComponent* other_cst{};
  for(auto elt : scenar_constraints)
  {
    auto& is = elt.second->iscoreConstraint();
    if(toRemove(toKeep, is))
    {
       scenar_comp->remove(is);
    }
    else if(&is == &constraint)
    {
      other_cst = elt.second.get();
    }
  }

  ISCORE_ASSERT(other_cst);

  // Get the time_constraint element of the constraint we're starting from,
  // unless it is already linked to the beginning.
  auto& start_e = *scenar_comp->OSSIAProcess().get_start_time_node()->get_time_events()[0];
  auto& new_end_e = other_cst->OSSIAConstraint()->get_start_event();
  if(&start_e != &new_end_e)
  {
    auto end_date = new_end_e.get_time_node().get_date();
    auto new_cst = ossia::time_constraint::create(
          ossia::time_constraint::exec_callback{},
          *scenar_comp->OSSIAProcess().get_start_time_node()->get_time_events()[0],
        new_end_e, end_date, end_date, end_date);

    scenar_comp->OSSIAProcess().add_time_constraint(new_cst);
  }

  // Then we add a constraint from the beginning of the scenario to this one,
  // and we do an offset.

  // TODO how to remove also the states ? for instance if there is a state on the first timenode ?

}

}
}
