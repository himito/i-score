#include "GroupPanelView.hpp"

#include "Widgets/GroupListWidget.hpp"
#include "Widgets/GroupTableWidget.hpp"
#include "DistributedScenario/Group.hpp"
#include "DistributedScenario/Commands/CreateGroup.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>

GroupPanelView::GroupPanelView(iscore::View* v):
    m_widget{new QWidget}
{
    setObjectName(tr("Groups"));
    auto lay = new QVBoxLayout;
    m_widget->setLayout(lay);
}

QWidget*GroupPanelView::getWidget()
{
    return m_widget;
}

Qt::DockWidgetArea GroupPanelView::defaultDock() const
{
    return Qt::LeftDockWidgetArea;
}

void GroupPanelView::setView(const GroupManager* mgr,
                             const Session* session)
{
    // Make the containing widget
    delete m_subWidget;
    m_subWidget = new QWidget;

    auto lay = new QVBoxLayout;
    m_subWidget->setLayout(lay);

    m_widget->layout()->addWidget(m_subWidget);

    // The sub-widgets (group data presentation)
    m_subWidget->layout()->addWidget(new GroupListWidget{mgr, m_subWidget});

    // Add group button
    auto button = new QPushButton{tr("Add group")};
    ObjectPath mgrpath{iscore::IDocument::path(mgr)};
    connect(button, &QPushButton::pressed, this, [=] ( )
    {
        bool ok;
        QString text = QInputDialog::getText(m_widget, tr("New group"),
                                             tr("Group name:"), QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty())
        {
            auto cmd = new CreateGroup{ObjectPath{mgrpath}, text};

            CommandDispatcher<> dispatcher{
                iscore::IDocument::documentFromObject(mgr)->commandStack(),
                        nullptr};
            dispatcher.submitCommand(cmd);
        }
    });
    m_subWidget->layout()->addWidget(button);

    // Group table
    m_subWidget->layout()->addWidget(new GroupTableWidget{mgr, session, m_widget});
}

void GroupPanelView::setEmptyView()
{
    delete m_subWidget;
}
