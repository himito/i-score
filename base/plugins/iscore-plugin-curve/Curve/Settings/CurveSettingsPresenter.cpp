#include "CurveSettingsPresenter.hpp"
#include "CurveSettingsModel.hpp"
#include "CurveSettingsView.hpp"
#include <QApplication>
#include <QStyle>
#include <iscore/command/Dispatchers/ICommandDispatcher.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/command/SettingsCommand.hpp>

namespace Curve
{
namespace Settings
{
Presenter::Presenter(Model& m, View& v, QObject* parent)
    : iscore::SettingsDelegatePresenter{m, v, parent}
{
  {
    // view -> model
    con(v, &View::simplificationRatioChanged, this,
        [&](auto simplificationRatio) {
          if (simplificationRatio != m.getSimplificationRatio())
          {
            m_disp.submitCommand<SetModelSimplificationRatio>(
                this->model(this), simplificationRatio);
          }
        });

    // model -> view
    con(m, &Model::SimplificationRatioChanged, &v,
        &View::setSimplificationRatio);

    // initial value
    v.setSimplificationRatio(m.getSimplificationRatio());
  }

  {
    // view -> model
    con(v, &View::simplifyChanged, this, [&](auto simplify) {
      if (simplify != m.getSimplify())
      {
        m_disp.submitCommand<SetModelSimplify>(this->model(this), simplify);
      }
    });

    // model -> view
    con(m, &Model::SimplifyChanged, &v, &View::setSimplify);

    // initial value
    v.setSimplify(m.getSimplify());
  }

  {
    // view -> model
    con(v, &View::modeChanged, this, [&](auto val) {
      if (val != m.getCurveMode())
      {
        m_disp.submitCommand<SetModelCurveMode>(this->model(this), val);
      }
    });

    // model -> view
    con(m, &Model::CurveModeChanged, &v, &View::setMode);

    // initial value
    v.setMode(m.getCurveMode());
  }

  {
    // view -> model
    con(v, &View::playWhileRecordingChanged, this, [&](auto val) {
      if (val != m.getPlayWhileRecording())
      {
        m_disp.submitCommand<SetModelPlayWhileRecording>(
            this->model(this), val);
      }
    });

    // model -> view
    con(m, &Model::PlayWhileRecordingChanged, &v,
        &View::setPlayWhileRecording);

    // initial value
    v.setPlayWhileRecording(m.getPlayWhileRecording());
  }
}

QString Presenter::settingsName()
{
  return tr("Recording");
}

QIcon Presenter::settingsIcon()
{
  return QApplication::style()->standardIcon(QStyle::SP_DriveCDIcon);
}
}
}
