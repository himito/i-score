#include "ScenarioStyle.hpp"
#include <iscore/model/Skin.hpp>
// TODO namespace
ScenarioStyle::ScenarioStyle(const iscore::Skin& s) noexcept
    :

    ConstraintBase{&s.Base1}
    , ConstraintSelected{&s.Base2}
    , ConstraintPlayFill{&s.Base3}
    , ConstraintPlayDashFill{&s.Pulse1}
    , ConstraintLoop{&s.Warn1}
    , ConstraintWarning{&s.Warn2}
    , ConstraintInvalid{&s.Warn3}
    , ConstraintMuted{&s.Tender2}
    , ConstraintDefaultLabel{&s.Gray}
    , ConstraintDefaultBackground{&s.Transparent1}
    ,

    RackSideBorder{&s.Base1}
    ,

    ConstraintFullViewParentSelected{&s.Emphasis1}
    ,

    ConstraintHeaderText{&s.Light}
    , ConstraintHeaderBottomLine{&s.Transparent1}
    , ConstraintHeaderRackHidden{&s.Transparent1}
    , ConstraintHeaderSideBorder{&s.Base1}
    ,

    ProcessViewBorder{&s.Gray}
    ,

    SlotFocus{&s.Base2}
    , SlotOverlayBorder{&s.Dark}
    , SlotOverlay{&s.Transparent2}
    , SlotHandle{&s.Transparent3}
    ,

    TimenodeDefault{&s.HalfLight}
    , TimenodeSelected{&s.Base2}
    ,

    EventDefault{&s.Emphasis4}
    , EventWaiting{&s.HalfLight}
    , EventPending{&s.Warn1}
    , EventHappened{&s.Base3}
    , EventDisposed{&s.Warn3}
    , EventSelected{&s.Base2}
    ,

    ConditionDefault{&s.Smooth3}
    , ConditionWaiting{&s.Gray}
    , ConditionDisabled{&s.Base1}
    , ConditionFalse{&s.Smooth1}
    , ConditionTrue{&s.Smooth2}
    ,

    StateOutline{&s.Light}
    , StateSelected{&s.Base2}
    , StateDot{&s.Base1}
    ,

    Background{&s.Background1}
    , ProcessPanelBackground{&s.Transparent1}
    ,

    TimeRulerBackground{&s.Background1}
    , TimeRuler{&s.Base1}
    , LocalTimeRuler{&s.Gray}
{
  update(s);
  QObject::connect(&s, &iscore::Skin::changed, [&] { this->update(s); });
}

void ScenarioStyle::setConstraintWidth(double w)
{
  ConstraintSolidPen.setWidth(3 * w);
  ConstraintDashPen.setWidth(3 * w);
  ConstraintPlayPen.setWidth(3 * w);
  ConstraintPlayDashPen.setWidth(3 * w);
}

ScenarioStyle& ScenarioStyle::instance()
{
  static ScenarioStyle s(iscore::Skin::instance());
  return s;
}

ScenarioStyle::ScenarioStyle() noexcept
{
  update(iscore::Skin::instance());
}

void ScenarioStyle::update(const iscore::Skin& skin)
{
  ConstraintSolidPen = QPen{QBrush{Qt::black}, 3, Qt::SolidLine, Qt::SquareCap,
                            Qt::RoundJoin};
  ConstraintDashPen = [] {
    QPen pen{QBrush{Qt::black}, 3, Qt::CustomDashLine, Qt::SquareCap,
             Qt::RoundJoin};

    pen.setDashPattern({2., 4.});
    return pen;
  }();
  ConstraintRackPen
      = QPen{Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  ConstraintPlayPen
      = QPen{Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  ConstraintPlayDashPen = ConstraintDashPen;
  ConstraintHeaderSeparator
      = QPen{ConstraintHeaderSideBorder.getColor(), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

  ConstraintBrace
      = QPen{ConstraintBase.getColor(), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  ConstraintBraceSelected = ConstraintBrace;
  ConstraintBraceSelected.setBrush(ConstraintSelected.getColor());
  ConstraintBraceWarning = ConstraintBrace;
  ConstraintBraceWarning.setBrush(ConstraintWarning.getColor());
  ConstraintBraceInvalid = ConstraintBrace;
  ConstraintBraceInvalid.setBrush(ConstraintInvalid.getColor());

  //ConstraintSolidPen.setCosmetic(true);
  ConstraintDashPen.setCosmetic(true);
  ConstraintRackPen.setCosmetic(true);
  ConstraintPlayPen.setCosmetic(true);
  ConstraintPlayDashPen.setCosmetic(true);
  TimenodePen = QPen{Qt::black, 2, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin};

  TimenodeBrush = QBrush{Qt::black};
  StateTemporalPointBrush = QBrush{Qt::black};
  StateTemporalPointPen.setCosmetic(true);
  StateBrush = QBrush{Qt::black};
  EventPen = QPen{Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  EventBrush = QBrush{Qt::black};

  TimeRulerLargePen = QPen{TimeRuler.getColor(), 2, Qt::SolidLine};
  TimeRulerLargePen.setCosmetic(true);
  TimeRulerSmallPen = QPen{TimeRuler.getColor(), 1, Qt::SolidLine};
  TimeRulerSmallPen.setCosmetic(true);

  SlotHandlePen.setWidth(0);
  SlotHandlePen.setBrush(ProcessViewBorder.getColor());

  Bold10Pt = skin.SansFont;
  Bold10Pt.setPointSize(10);
  Bold10Pt.setBold(true);

  Medium7Pt = skin.SansFont;
  Medium7Pt.setPointSize(7);
  Medium7Pt.setStyleStrategy(QFont::NoAntialias);
  Medium7Pt.setHintingPreference(QFont::HintingPreference::PreferFullHinting);

  Medium8Pt = skin.SansFont;
  Medium8Pt.setPointSize(8);

  Medium12Pt = skin.SansFont;
  Medium12Pt.setPointSize(12);
  Medium12Pt.setStyleStrategy(QFont::NoAntialias);
}
