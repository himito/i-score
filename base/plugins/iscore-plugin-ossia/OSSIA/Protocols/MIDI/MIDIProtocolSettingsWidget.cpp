#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#include <QRadioButton>
#include <QString>
#include <QLineEdit>
#include <QVariant>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <Device/Protocol/ProtocolSettingsWidget.hpp>
#include "MIDIProtocolSettingsWidget.hpp"
#include "MIDISpecificSettings.hpp"
class QWidget;

namespace Ossia
{
MIDIProtocolSettingsWidget::MIDIProtocolSettingsWidget(QWidget* parent)
    : ProtocolSettingsWidget(parent)
{
    buildGUI();
}

void
MIDIProtocolSettingsWidget::buildGUI()
{
    m_name = new QLineEdit;
    m_inButton = new QCheckBox(tr("Input"), this);
    m_inButton->setAutoExclusive(true);
    m_outButton = new QCheckBox(tr("Output"), this);
    m_outButton->setAutoExclusive(true);
    m_deviceCBox = new QComboBox(this);

    auto gb = new QWidget;
    gb->setContentsMargins(0, 0, 0, 0);
    auto gb_lay = new QHBoxLayout;
    gb_lay->setContentsMargins(0, 0, 0, 0);
    gb_lay->addWidget(m_inButton);
    gb_lay->addWidget(m_outButton);
    gb->setLayout(gb_lay);

    auto lay = new QFormLayout;
    lay->addRow(tr("Name"), m_name);
    lay->addRow(tr("Type"), gb);
    lay->addRow(tr("Device"), m_deviceCBox);

    setLayout(lay);

    connect(m_inButton, &QAbstractButton::toggled,
            this, [this] (bool b) {
        if(b)
        {
            updateDevices(OSSIA::MidiInfo::Type::RemoteInput);
        }
    });
    connect(m_outButton, &QAbstractButton::toggled,
            this, [this] (bool b) {
        if(b)
        {
            updateDevices(OSSIA::MidiInfo::Type::RemoteOutput);
        }
    });


    m_inButton->setChecked(true);  //TODO: QSettings
    updateInputDevices();
}

Device::DeviceSettings MIDIProtocolSettingsWidget::getSettings() const
{
    ISCORE_ASSERT(m_deviceCBox);
    ISCORE_ASSERT(m_inButton);

    // TODO *** Initialize with ProtocolFactory.defaultSettings().
    Device::DeviceSettings s;
    MIDISpecificSettings midi;
    s.name = m_name->text();

    midi.io = m_inButton->isChecked()
              ? MIDISpecificSettings::IO::In
              : MIDISpecificSettings::IO::Out;
    midi.endpoint = m_deviceCBox->currentText();
    midi.port = m_deviceCBox->currentData().toInt();

    s.deviceSpecificSettings = QVariant::fromValue(midi);

    return s;
}

void
MIDIProtocolSettingsWidget::setSettings(const Device::DeviceSettings &settings)
{
    /*
    ISCORE_ASSERT(settings.size() == 2);

    if(settings.at(1) == "In")
    {
        m_inButton->setChecked(true);
    }
    else
    {
        m_outButton->setChecked(true);
    }
*/
    int index = m_deviceCBox->findText(settings.name);

    if(index >= 0 && index < m_deviceCBox->count())
    {
        m_deviceCBox->setCurrentIndex(index);
    }

    if (settings.deviceSpecificSettings.canConvert<MIDISpecificSettings>())
    {
        MIDISpecificSettings midi = settings.deviceSpecificSettings.value<MIDISpecificSettings>();
        if(midi.io == MIDISpecificSettings::IO::In)
        {
            m_inButton->setChecked(true);
        }
        else
        {
            m_outButton->setChecked(true);
        }
    }
}

void MIDIProtocolSettingsWidget::updateDevices(OSSIA::MidiInfo::Type t)
{
    auto prot = OSSIA::MIDI::create();
    auto vec = prot->scan();

    m_deviceCBox->clear();
    for(auto& elt : vec)
    {
        if(elt.type == t)
        {
            m_deviceCBox->addItem(QString::fromStdString(elt.device), QVariant::fromValue(elt.port));
        }
    }
    m_deviceCBox->setCurrentIndex(0);
    qDebug() << m_deviceCBox->count();
}

void
MIDIProtocolSettingsWidget::updateInputDevices()
{
    updateDevices(OSSIA::MidiInfo::Type::RemoteInput);
}

void
MIDIProtocolSettingsWidget::updateOutputDevices()
{
    updateDevices(OSSIA::MidiInfo::Type::RemoteOutput);
}
}
