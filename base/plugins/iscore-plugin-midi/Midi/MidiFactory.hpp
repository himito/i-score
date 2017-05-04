#pragma once
#include <Process/GenericProcessFactory.hpp>

#include <Midi/MidiPresenter.hpp>
#include <Midi/MidiProcess.hpp>
#include <Midi/MidiView.hpp>
#include <Process/LayerModelPanelProxy.hpp>

namespace Midi
{
using ProcessFactory = Process::GenericProcessModelFactory<Midi::ProcessModel>;
using LayerFactory = Process::
    GenericLayerFactory<Midi::ProcessModel, Midi::Presenter, Midi::View, Process::GraphicsViewLayerPanelProxy>;
}
