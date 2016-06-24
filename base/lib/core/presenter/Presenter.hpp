#pragma once
#include <iscore/application/ApplicationComponents.hpp>
#include <iscore/application/GUIApplicationContext.hpp>
#include <iscore/actions/Action.hpp>

#include <core/presenter/DocumentManager.hpp>

#include <iscore/tools/NamedObject.hpp>
#include <iscore/widgets/OrderedToolbar.hpp>
#include <vector>
#include <QMenuBar>
#include <iscore_lib_base_export.h>
class QObject;


namespace iscore
{

class CoreApplicationPlugin;
class View;
class Settings;

/**
     * @brief The Presenter class
     *
     * Certainly needs refactoring.
     * For now, manages menus and plug-in objects.
     *
     * It is also able to instantiate a Command from serialized Undo/Redo data.
     * (this should go in the DocumentPresenter maybe ?)
     */
class ISCORE_LIB_BASE_EXPORT Presenter final : public NamedObject
{
        Q_OBJECT
        friend class iscore::CoreApplicationPlugin;
    public:
        Presenter(
                const iscore::ApplicationSettings& app,
                const iscore::Settings& set,
                iscore::View* view,
                QObject* parent);

        // Exit i-score
        bool exit();

        View* view() const;

        auto& menuManager() { return m_menus; }
        auto& toolbarManager() { return m_toolbars; }
        auto& actionManager() { return m_actions; }

        // Called after all the classes
        // have been loaded from plug-ins.
        void setupGUI();


        auto& documentManager()
        { return m_docManager; }
        const ApplicationComponents& applicationComponents()
        { return m_components_readonly; }
        const GUIApplicationContext& applicationContext()
        { return m_context; }

        auto& components()
        { return m_components; }
    private:
        void setupMenus();
        View* m_view {};
        const Settings& m_settings;

        DocumentManager m_docManager;
        ApplicationComponentsData m_components;
        ApplicationComponents m_components_readonly;

        QMenuBar* m_menubar{};
        GUIApplicationContext m_context;

        MenuManager m_menus;
        ToolbarManager m_toolbars;
        ActionManager m_actions;

};
}
