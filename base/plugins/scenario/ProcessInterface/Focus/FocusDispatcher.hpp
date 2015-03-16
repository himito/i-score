#pragma once
#include <QObject>
class ProcessViewModelInterface;
namespace iscore
{
    class Document;
}
class BaseElementModel;

// Sets the focus on a scenario document.
class FocusDispatcher
{
    public:
        FocusDispatcher(iscore::Document& doc);

        void focus(ProcessViewModelInterface* obj);

    private:
        BaseElementModel& m_baseElementModel;
};