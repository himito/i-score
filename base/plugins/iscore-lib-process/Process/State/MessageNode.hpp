#pragma once
#include <State/Message.hpp>

#include <iscore/tools/std/Optional.hpp>
#include <iscore/tools/TreeNode.hpp>
#include <QString>
#include <QStringList>
#include <QVector>
#include <algorithm>
#include <array>

#include <State/Address.hpp>
#include <State/Value.hpp>
#include <State/ValueConversion.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_lib_process_export.h>

namespace Process {
class ProcessModel;

struct ProcessStateData
{
        Id<Process::ProcessModel> process;
        State::OptionalValue value;
};

enum class PriorityPolicy {
    User, Previous, Following
};

struct StateNodeValues
{
        bool empty() const
        {
            return previousProcessValues.isEmpty() && followingProcessValues.isEmpty() && !userValue;
        }

        // TODO use lists or queues instead to manage the priorities
        QVector<ProcessStateData> previousProcessValues;
        QVector<ProcessStateData> followingProcessValues;
        State::OptionalValue userValue;


        std::array<PriorityPolicy, 3> priorities{{
            PriorityPolicy::Previous,
            PriorityPolicy::Following,
            PriorityPolicy::User
        }};

        static bool hasValue(const QVector<ProcessStateData>& vec)
        {
            return std::any_of(vec.cbegin(), vec.cend(),
                            [] (const auto& pv) {
                    return bool(pv.value);
                });
        }

        bool hasValue() const
        {
            return hasValue(previousProcessValues) || hasValue(followingProcessValues) || bool(userValue);
        }

        static auto value(const QVector<ProcessStateData>& vec)
        {
            return std::find_if(vec.cbegin(), vec.cend(),
                            [] (const auto& pv) {
                    return bool(pv.value);
                });
        }

        // TODO here we have to choose a policy
        // if we have both previous and following processes ?
        State::OptionalValue value() const
        {
            for(const auto& prio : priorities)
            {
                switch(prio)
                {
                    case PriorityPolicy::User:
                    {
                        if(userValue)
                            return *userValue;
                        break;
                    }

                    case PriorityPolicy::Previous:
                    {
                        // OPTIMIZEME  by computing them only once
                        auto it = value(previousProcessValues);
                        if(it != previousProcessValues.cend())
                            return *it->value;
                        break;
                    }

                    case PriorityPolicy::Following:
                    {
                        auto it = value(followingProcessValues);
                        if(it != followingProcessValues.cend())
                            return *it->value;
                        break;
                    }

                    default:
                        break;
                }
            }

            return {};
        }

        QString displayValue() const
        {
            auto val = value();
            if(val)
                return State::convert::value<QString>(*val);
            return {};
        }

};

struct StateNodeMessage
{
        State::Address addr; // device + path
        StateNodeValues values;
};

struct StateNodeData
{
        QString name;
        StateNodeValues values;

        const QString& displayName() const
        { return name; }

        bool hasValue() const
        { return values.hasValue(); }

        State::OptionalValue value() const
        { return values.value(); }

};

inline QDebug operator<<(QDebug d, const ProcessStateData& mess)
{
    d << "{" << mess.process << State::convert::toPrettyString(*mess.value) << "}";
    return d;
}

inline QDebug operator<<(QDebug d, const StateNodeData& mess)
{
    if(mess.values.userValue)
        d << mess.name << mess.values.previousProcessValues << State::convert::toPrettyString(*mess.values.userValue) << mess.values.followingProcessValues;
    else
        d << mess.name<< mess.values.previousProcessValues << "-- no user value --" << mess.values.followingProcessValues;
    return d;
}

using MessageNode = TreeNode<StateNodeData>;
using MessageNodePath = TreePath<MessageNode>;

ISCORE_LIB_PROCESS_EXPORT State::Address address(const MessageNode& treeNode);
ISCORE_LIB_PROCESS_EXPORT State::Message message(const MessageNode& node);
ISCORE_LIB_PROCESS_EXPORT State::Message userMessage(const MessageNode& node);
ISCORE_LIB_PROCESS_EXPORT QStringList toStringList(const State::Address& addr);

ISCORE_LIB_PROCESS_EXPORT State::MessageList flatten(const MessageNode&);
ISCORE_LIB_PROCESS_EXPORT State::MessageList getUserMessages(const MessageNode&);
}
