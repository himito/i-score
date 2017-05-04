#pragma once
#include <QObject>
#include <QStringList>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <vector>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/application/GUIApplicationPlugin.hpp>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>

/**
 * \namespace Engine
 * \brief Link of i-score with the OSSIA API execution engine.
 *
 * This namespace provides the tools that are used when going from
 * the i-score model, with purely serializable data structures, to
 * the OSSIA API model.
 *
 * OSSIA implementations of the protocolsare also provided.
 *
 * There are three main parts :
 *
 * * \ref Engine::LocalTree handles the conversion from i-score's data
 * structures to
 * the Local protocol.
 * * \ref Engine::Execution handles the conversion from i-score's data
 * structures to
 * the OSSIA classes responsible for the execution (ossia::time_process,
 * ossia::clock, etc.)
 * * \ref Engine::Network wraps the various OSSIA protocols (Minuit, OSC, etc.)
 *   behind Device::DeviceInterface, and provides edition widgets for these
 * protocols.
 * * Classes used to handle the various node listening strategies are provided.
 *
 * Two files, \ref iscore2OSSIA.hpp and \ref OSSIA2iscore.hpp contain tools
 * to convert the various data structures of each environment into each other.
 */

/**
 * \namespace Engine::LocalTree
 * \brief Local tree provides a way to extend the tree given through the \ref
 * Engine::Network::LocalDevice.
 *
 * It is a device tree used to access to i-score's internal data structures
 * from the outside,
 * or use it in automations, etc.
 *
 * For instance, it allows to do conditions based on the advancement of each
 * Scenario::ConstraintModel.
 *
 * It is possible to extend the tree for Process::ProcessModel%s, or other
 * polymorphic types.
 */

/**
 * \namespace Engine::Execution
 * \brief Components used for the execution of a score.
 *
 * This hierarchy of types is used to create the OSSIA structures for the
 * execution behind
 * iscore::Component%s.
 * <br>
 * Currently, all the execution structures are recreated from scratch when
 * doing "play".
 * <br>
 * The classes inheriting from Engine::Execution::ProcessComponent
 * are first created, then the Engine::Execution::ConstraintComponent will ask
 * them for their ossia::time_process which it gives to the matching
 * ossia::time_constraint.
 *
 * \section LiveModification Live modification during execution.
 * The execution engine allows live modification of scores.
 * Since the execution happens in a different thread than edition, we have to be
 * extremely careful however.
 * <br>
 * Instead of locking all the data structures of the OSSIA API with mutex,
 * which may slow down the execution, we instead have a lock-free queue of
 * edition commands.
 * <br>
 * Modifications are submitted from the component hierarchy :
 * <br>
 * * Engine::Execution::ScenarioComponent
 * * Engine::Execution::ConstraintComponent
 * * etc...
 * <br><br>
 * To the OSSIA structures :
 * <br>
 * * ossia::time_process
 * * ossia::time_constraint
 * * etc...
 * <br>
 * <br>
 *
 * A modification follows this pattern :
 * \code
 * [ User modification in the GUI ]
 *          |
 *          v
 * [ Commands applied ]
 *          |
 *          v
 * [ Models modified and
 *   modification signals emitted ]
 *          |
 *          v
 * [ Execution components
 *   catches the signal ]
 *          |
 *          v
 * [ Command inserted into
 *   Engine::Execution::Context::executionQueue ]
 *          |
 *          v
 * [ The execution algorithm applies the
 *   commands at the end of the current tick ]
 * \endcode
 *
 * For modification of values, for instance the duration of a
 * Constraint, this is easily visible.
 * See for instance Engine::Execution::ConstraintComponentBase 's constructor.
 * <br>
 * For creation and removal of objects, this should be handled automatically by
 * the various ComponentHierarchy classes which take care of creating and removing the objects
 * in the correct order. The Component classes just have to provide functions that will
 * do the actual instantiation, and pre- & post- removal steps.
 * <br><br>
 * The actual "root" execution algorithm is given in Engine::Execution::DefaulClockManager::makeDefaultCallback
 *
 * \subsection ExecutionThreadSafety Execution Thread Safety
 * One must take care when modifying the Execution classes, since thins
 * happen on two different threads.
 *
 * The biggest problem is that the i-score structures could be created and deleted
 * in a single tick. For instance when doing a complete undo - redo of the whole undo stack.
 * <br>
 * This means that anything send to the command queue must absolutely never access
 * any of the iscore structures (for instance Scenario::ConstraintModel, etc) directly : they have to be copied.
 * Else, there *will* be crashes, someday.
 * <br>
 * In the flow graph shown before, everything up to and including "Command inserted into the execution queue"
 * happens in the GUI thread, hence one can rely on everything "being here" at this point.
 * However, in the actual commands, the only things safe to use are :
 * * Copies of data : simple values, ints, etc, are safe.
 * * Shared pointers : unlike most other places in i-score, the Execution components
 * are not owned by their parents, but through shared pointers. This means that shall the component
 * be removed, if the pointer was copied in the ExecutionCommand, there is no risk of crash.
 * But one must take care of copying the actual `shared_ptr` and not just the `this` pointer for instance.
 * Multiple classes inherit from `std::enable_shared_from_this` to allow a `shared_from_this()` function
 * that gives back a `shared_ptr` to the this instance.
 *
 */

/**
 * \namespace Engine::Network
 * \brief OSSIA protocols wrapped into i-score
 *
 * This namespace provides the implementations in i-score for
 * various protocols: OSC, MIDI, Minuit, HTTP, WebSocket...
 */

class iscore_plugin_engine final
    : public QObject,
      public iscore::ApplicationPlugin_QtInterface,
      public iscore::FactoryList_QtInterface,
      public iscore::FactoryInterface_QtInterface,
      public iscore::Plugin_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ApplicationPlugin_QtInterface_iid)
  Q_INTERFACES(
      iscore::ApplicationPlugin_QtInterface
          iscore::FactoryList_QtInterface iscore::FactoryInterface_QtInterface
              iscore::Plugin_QtInterface)

  ISCORE_PLUGIN_METADATA(1, "d4758f8d-64ac-41b4-8aaf-1cbd6f3feb91")
public:
  iscore_plugin_engine();
  virtual ~iscore_plugin_engine();

private:
  iscore::GUIApplicationPlugin*
  make_guiApplicationPlugin(const iscore::GUIApplicationContext& app) override;

  std::vector<std::unique_ptr<iscore::InterfaceListBase>>
  factoryFamilies() override;

  // Contains the OSC, MIDI, Minuit factories
  std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
      const iscore::ApplicationContext&,
      const iscore::InterfaceKey& factoryName) const override;

  std::vector<iscore::PluginKey> required() const override;
};
