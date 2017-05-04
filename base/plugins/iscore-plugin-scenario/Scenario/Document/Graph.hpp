#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <iscore/tools/std/HashMap.hpp>
#include <iscore_plugin_scenario_export.h>

namespace Scenario
{
class TimeNodeModel;
class ConstraintModel;
class ScenarioInterface;

using GraphVertex = Scenario::TimeNodeModel*;
using GraphEdge = Scenario::ConstraintModel*;

using Graph = boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::directedS,
  GraphVertex,
  GraphEdge>;

/**
 * @brief A directed graph of all the TimeNode%s in a ScenarioInterface.
 *
 * The vertices are the TimeNode%s, the edges are the ConstraintModel%s.
 * The graph is built upon construction.
 *
 */
struct ISCORE_PLUGIN_SCENARIO_EXPORT TimenodeGraph
{
  TimenodeGraph(const Scenario::ScenarioInterface& scenar);

  const Graph& graph() const
  { return m_graph; }
  const auto& edges() const
  { return m_edges; }
  const auto& vertices() const
  { return m_vertices; }

  //! Writes graphviz output on stdout
  void writeGraphviz();

private:
  Graph m_graph;

  iscore::hash_map<
      const Scenario::TimeNodeModel*,
      Graph::vertex_descriptor> m_vertices;
  iscore::hash_map<
      const Scenario::ConstraintModel*,
      Graph::edge_descriptor> m_edges;
};

}
