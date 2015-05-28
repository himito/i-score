#include "DurationSectionWidget.hpp"

#include "Inspector/Constraint/ConstraintInspectorWidget.hpp"

#include "Document/Constraint/ConstraintModel.hpp"

#include "Commands/Constraint/SetMinDuration.hpp"
#include "Commands/Constraint/SetMaxDuration.hpp"
#include "Commands/Scenario/ResizeConstraint.hpp"
#include "Commands/ResizeBaseConstraint.hpp"
#include "Commands/Constraint/SetRigidity.hpp"

#include <QCheckBox>
#include <QSpinBox>
#include <QToolButton>
#include <QLabel>
#include <QFormLayout>
#include <QTimeEdit>
using namespace iscore;
DurationSectionWidget::DurationSectionWidget(ConstraintInspectorWidget* parent) :
    InspectorSectionWidget {"Durations", parent},
    m_model {parent->model()},
    m_parent {parent},
    m_cmdDispatcher{new OngoingCommandDispatcher<MergeStrategy::Simple>{
                            IDocument::documentFromObject(m_model)->commandStack()}}
{
    QWidget* widg{new QWidget{this}};
    QFormLayout* lay = new QFormLayout{widg};
    lay->setContentsMargins(0, 0, 0 , 0);
    lay->setVerticalSpacing(0);
    widg->setLayout(lay);

    m_minSpin = new QTimeEdit{};
    m_maxSpin = new QTimeEdit{};
    m_valueSpin = new QTimeEdit{};

    // TODO these need to be updated when the default duration changes
    connect(m_model, &ConstraintModel::defaultDurationChanged,
            this,	 &DurationSectionWidget::on_modelDefaultDurationChanged);
    connect(m_model,    &ConstraintModel::minDurationChanged,
            this,       &DurationSectionWidget::on_modelMinDurationChanged);
    connect(m_model,    &ConstraintModel::maxDurationChanged,
            this,       &DurationSectionWidget::on_modelMaxDurationChanged);

    m_valueSpin->setDisplayFormat(QString("mm.ss.zzz"));
    m_minSpin->setDisplayFormat(QString("mm.ss.zzz"));
    m_maxSpin->setDisplayFormat(QString("mm.ss.zzz"));

    QToolButton* timeValidation = new QToolButton{};
    timeValidation->setText("OK");

    m_default = m_model->defaultDuration();
    m_min = m_default;
    m_max = m_default;
    m_max.addMSecs(1);

    m_minSpin->setMaximumTime(m_min.toQTime());
    m_maxSpin->setMinimumTime(m_max.toQTime());


    auto rigid_checkbox = new QCheckBox{"Rigid"};
    connect(rigid_checkbox, &QCheckBox::toggled,
            [ = ](bool val)
    {
        m_minSpin->setEnabled(!val);
        m_maxSpin->setEnabled(!val);
    });


    m_infinite = new QCheckBox{"Infinite"};
    connect(m_infinite, &QCheckBox::toggled,
            [=](bool val)
    {
        TimeValue newTime = val
                            ? TimeValue(PositiveInfinity{})
                            : TimeValue(std::chrono::milliseconds(m_maxSpin->time().msecsSinceStartOfDay()));
        if(m_model->maxDuration() == newTime)
            return;

        auto cmd = new Scenario::Command::SetMaxDuration(
                       iscore::IDocument::path(m_model),
                       newTime);

        m_cmdDispatcher->submitCommand(cmd);
        m_cmdDispatcher->commit();
    });


    m_minSpin->setTime(m_min.toQTime());
    m_maxSpin->setTime((m_max.toQTime()));

    if(m_model->minDuration() == m_model->maxDuration())
    {
        rigid_checkbox->setChecked(true);
    }

    m_valueSpin->setTime(m_default.toQTime());

    lay->addRow(rigid_checkbox, m_infinite);
    lay->addRow(tr("Min duration"), m_minSpin);
    lay->addRow(tr("Max duration"), m_maxSpin);
    lay->addRow(tr("Default"), m_valueSpin);
    lay->addRow(timeValidation);

    connect(timeValidation, &QToolButton::clicked,
            this,           &DurationSectionWidget::on_durationsChanged);

    connect(rigid_checkbox,	&QCheckBox::toggled,
            this,		&DurationSectionWidget::rigidCheckboxToggled);


    addContent(widg);

    m_valueSpin->setFocus();
}

using namespace Scenario::Command;

void DurationSectionWidget::minDurationSpinboxChanged(int val)
{
    auto cmd = new SetMinDuration(
                   iscore::IDocument::path(m_model),
                   std::chrono::milliseconds {val});

    emit m_cmdDispatcher->submitCommand(cmd);
}

void DurationSectionWidget::maxDurationSpinboxChanged(int val)
{
    auto cmd = new SetMaxDuration(
                   iscore::IDocument::path(m_model),
                   std::chrono::milliseconds {val});

    emit m_cmdDispatcher->submitCommand(cmd);
}

void DurationSectionWidget::defaultDurationSpinboxChanged(int val)
{
    iscore::SerializableCommand* cmd {};

    if(m_model->objectName() != "BaseConstraintModel")
    {
        cmd = new ResizeConstraint(
                    iscore::IDocument::path(m_model),
                    std::chrono::milliseconds {val},
                    ExpandMode::Scale); //TODO check scenario mode instead
    }
    else
    {
        cmd = new ResizeBaseConstraint(
                    iscore::IDocument::path(m_model),
                    std::chrono::milliseconds {val});
    }

    emit m_cmdDispatcher->submitCommand(cmd);
}

void DurationSectionWidget::rigidCheckboxToggled(bool b)
{
    auto cmd = new SetRigidity(
                   iscore::IDocument::path(m_model),
                   b);

    emit m_parent->commandDispatcher()->submitCommand(cmd);
}

void DurationSectionWidget::on_modelDefaultDurationChanged(TimeValue dur)
{
    if (dur.toQTime() == m_valueSpin->time())
        return;

    m_valueSpin->setTime(dur.toQTime());
}

void DurationSectionWidget::on_modelMinDurationChanged(TimeValue dur)
{
    if (dur.toQTime() == m_minSpin->time())
        return;

    m_minSpin->setTime(dur.toQTime());
}

void DurationSectionWidget::on_modelMaxDurationChanged(TimeValue dur)
{
    if(dur.isInfinite())
    {
        if(!m_infinite->isChecked())
        {
            m_infinite->setCheckState(Qt::Checked);
            m_maxSpin->setEnabled(false);
        }
    }
    else
    {
        if(m_infinite->isChecked())
        {
            m_infinite->setCheckState(Qt::Unchecked);
            m_maxSpin->setEnabled(true);
        }

        if (dur.toQTime() == m_maxSpin->time())
            return;

        m_maxSpin->setTime(dur.toQTime());
    }
}

void DurationSectionWidget::on_durationsChanged()
{
    if (m_default.toQTime() != m_valueSpin->time())
    {
        defaultDurationSpinboxChanged(m_valueSpin->time().msecsSinceStartOfDay());
        m_cmdDispatcher->commit();
    }

    if (m_min.toQTime() != m_minSpin->time())
    {
        minDurationSpinboxChanged(m_minSpin->time().msecsSinceStartOfDay());
        m_cmdDispatcher->commit();

    }if (m_max.toQTime() != m_maxSpin->time())
    {
        maxDurationSpinboxChanged(m_maxSpin->time().msecsSinceStartOfDay());
        m_cmdDispatcher->commit();
    }
}

DurationSectionWidget::~DurationSectionWidget()
{
    delete m_cmdDispatcher;
}