#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateView.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/widgets/SpinBoxes.hpp>

class QComboBox;
class QSpinBox;
namespace Scenario
{
namespace Settings
{

class View :
        public iscore::SettingsDelegateView
{
        Q_OBJECT
    public:
        View();

        void setSkin(const QString&);
        void setZoom(const int); // zoom percentage
        void setSlotHeight(const qreal);
        void setDefaultDuration(const TimeValue& t);

    signals:
        void skinChanged(const QString&);
        void zoomChanged(int);
        void slotHeightChanged(qreal);
        void defaultDurationChanged(const TimeValue& t);

    private:
        QWidget* getWidget() override;
        QWidget* m_widg{};

        QComboBox* m_skin{};
        QSpinBox* m_zoomSpinBox{};
        QSpinBox* m_slotHeightBox{};
        iscore::TimeSpinBox* m_defaultDur{};
};

}
}
