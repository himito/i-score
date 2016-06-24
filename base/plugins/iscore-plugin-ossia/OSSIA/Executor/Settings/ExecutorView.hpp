#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateView.hpp>
#include <OSSIA/Executor/Settings/ExecutorModel.hpp>
class QSpinBox;
class QComboBox;
namespace RecreateOnPlay
{
namespace Settings
{

class View :
        public iscore::SettingsDelegateView
{
        Q_OBJECT
    public:
        View();

        void setRate(int);
        void setClock(ClockManagerFactory::ConcreteFactoryKey k);

        void populateClocks(const std::map<QString, ClockManagerFactory::ConcreteFactoryKey>&);

    signals:
        void rateChanged(int);
        void clockChanged(ClockManagerFactory::ConcreteFactoryKey);

    private:
        QWidget* getWidget() override;
        QWidget* m_widg{};

        QSpinBox* m_sb{};
        QComboBox* m_cb{};
};

}
}
