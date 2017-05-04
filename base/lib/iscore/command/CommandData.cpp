#include <iscore/command/CommandData.hpp>
#include <iscore/command/Command.hpp>

iscore::CommandData::CommandData(const iscore::Command& cmd)
    : parentKey{cmd.parentKey()}, commandKey{cmd.key()}, data{cmd.serialize()}
{
}
