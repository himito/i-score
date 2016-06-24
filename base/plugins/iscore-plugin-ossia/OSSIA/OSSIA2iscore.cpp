#include <QChar>
#include <QDebug>
#include <QString>
#include <algorithm>
#include <memory>
#include <vector>

#include "Editor/Domain.h"
#include "Editor/Value.h"
#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include <OSSIA/OSSIA2iscore.hpp>
#include <iscore/tools/TreeNode.hpp>

namespace Ossia
{
namespace convert
{


Device::IOType ToIOType(OSSIA::AccessMode t)
{
    switch(t)
    {
        case OSSIA::AccessMode::GET:
            return Device::IOType::In;
        case OSSIA::AccessMode::SET:
            return Device::IOType::Out;
        case OSSIA::AccessMode::BI:
            return Device::IOType::InOut;
        default:
            ISCORE_ABORT;
            return Device::IOType::Invalid;
    }
}


Device::ClipMode ToClipMode(OSSIA::BoundingMode b)
{
    switch(b)
    {
        case OSSIA::BoundingMode::CLIP:
            return Device::ClipMode::Clip;
            break;
        case OSSIA::BoundingMode::FOLD:
            return Device::ClipMode::Fold;
            break;
        case OSSIA::BoundingMode::FREE:
            return Device::ClipMode::Free;
            break;
        case OSSIA::BoundingMode::WRAP:
            return Device::ClipMode::Wrap;
            break;
        default:
            ISCORE_ABORT;
            return static_cast<Device::ClipMode>(-1);
    }
}

State::Value ToValue(const OSSIA::Value *val)
{
    if(!val)
        return {};

    switch(val->getType())
    {
        case OSSIA::Value::Type::IMPULSE:
            return State::Value::fromValue(State::impulse_t{});
        case OSSIA::Value::Type::BOOL:
        {
            auto val_sub = static_cast<const OSSIA::Bool*>(val);
            return val_sub ? State::Value::fromValue(val_sub->value) : State::Value::fromValue(bool{});
        }
        case OSSIA::Value::Type::INT:
        {
            auto val_sub = static_cast<const OSSIA::Int*>(val);
            return val_sub ? State::Value::fromValue(val_sub->value) : State::Value::fromValue(int{});
        }
        case OSSIA::Value::Type::FLOAT:
        {
            auto val_sub = static_cast<const OSSIA::Float*>(val);
            return val_sub ? State::Value::fromValue(val_sub->value) : State::Value::fromValue(float{});
        }
        case OSSIA::Value::Type::CHAR:
        {
            auto val_sub = static_cast<const OSSIA::Char*>(val);
            return val_sub ? State::Value::fromValue(val_sub->value) : State::Value::fromValue(char{});
        }
        case OSSIA::Value::Type::STRING:
        {
            auto val_sub = static_cast<const OSSIA::String*>(val);
            return val_sub ? State::Value::fromValue(QString::fromStdString(val_sub->value)) : State::Value::fromValue(QString{});
        }
        case OSSIA::Value::Type::TUPLE:
        {
            auto ossia_tuple = static_cast<const OSSIA::Tuple*>(val);

            State::tuple_t tuple;
            if(!ossia_tuple)
                return State::Value::fromValue(tuple);

            tuple.reserve(ossia_tuple->value.size());
            for (const auto & e : ossia_tuple->value)
            {
                tuple.push_back(ToValue(e).val); // TODO REVIEW THIS
            }

            return State::Value::fromValue(tuple);
        }
        case OSSIA::Value::Type::GENERIC:
        {
            ISCORE_TODO;
            return {};
            /*
            auto generic = dynamic_cast<const OSSIA::Generic*>(val);
            v = QByteArray{generic->start, generic->size};
            break;
            */
        }
        case OSSIA::Value::Type::DESTINATION:
        case OSSIA::Value::Type::BEHAVIOR:
        default:
            return {};
    }
}

State::Address ToAddress(const OSSIA::Node& node)
{
    State::Address addr;
    const OSSIA::Node* cur = &node;

    while(!dynamic_cast<const OSSIA::Device*>(cur))
    {
        addr.path.push_front(QString::fromStdString(cur->getName()));
        cur = cur->getParent().get();
        ISCORE_ASSERT(cur);
    }

    ISCORE_ASSERT(dynamic_cast<const OSSIA::Device*>(cur));
    addr.device = QString::fromStdString(cur->getName());
    return addr;
}

Device::AddressSettings ToAddressSettings(const OSSIA::Node &node)
{
    Device::AddressSettings s;
    s.name = QString::fromStdString(node.getName());

    const auto& addr = node.getAddress();

    if(addr)
    {
        addr->pullValue();
        try {
            if(auto val = addr->cloneValue())
            {
                s.value = ToValue(val);
                delete val;
            }
            else
            {
                s.value = ToValue(addr->getValueType());
            }
        }
        catch(...)
        {
            s.value = ToValue(addr->getValueType());

        }

        /* Debug code
        else
        {
            QStringList total;
            auto nptr = &node;
            while(nptr)
            {
                total += QString::fromStdString(nptr->getName());
                if(nptr->getParent())
                {
                    nptr = nptr->getParent().get();
                }
                else
                    break;
            }
            std::reverse(total.begin(), total.end());
            qDebug() << total.join("/");
        }
        */

        s.ioType = ToIOType(addr->getAccessMode());
        s.clipMode = ToClipMode(addr->getBoundingMode());
        s.repetitionFilter = addr->getRepetitionFilter();

        if(auto& domain = addr->getDomain())
            s.domain = ToDomain(*domain);
    }
    return s;
}


Device::Node ToDeviceExplorer(const OSSIA::Node &ossia_node)
{
    Device::Node iscore_node{ToAddressSettings(ossia_node), nullptr};
    iscore_node.reserve(ossia_node.children().size());

    // 2. Recurse on the children
    for(const auto& ossia_child : ossia_node.children())
    {
        auto child_n = ToDeviceExplorer(*ossia_child.get());
        child_n.setParent(&iscore_node);
        iscore_node.push_back(std::move(child_n));
    }

    return iscore_node;
}


Device::Domain ToDomain(OSSIA::Domain &domain)
{
    Device::Domain d;
    d.min = ToValue(domain.getMin());
    d.max = ToValue(domain.getMax());

    for(const auto& val : domain.getValues())
    {
        d.values.append(ToValue(val));
    }

    return d;
}

State::Value ToValue(OSSIA::Value::Type t)
{
    switch(t)
    {
        case OSSIA::Value::Type::FLOAT:
            return State::Value::fromValue(float{});
        case OSSIA::Value::Type::IMPULSE:
            return State::Value::fromValue(State::impulse_t{});
        case OSSIA::Value::Type::INT:
            return State::Value::fromValue(int{});
        case OSSIA::Value::Type::BOOL:
            return State::Value::fromValue(bool{});
        case OSSIA::Value::Type::CHAR:
            return State::Value::fromValue(QChar{});
        case OSSIA::Value::Type::STRING:
            return State::Value::fromValue(QString{});
        case OSSIA::Value::Type::TUPLE:
            return State::Value::fromValue(State::tuple_t{});
        case OSSIA::Value::Type::GENERIC:
        case OSSIA::Value::Type::DESTINATION:
        case OSSIA::Value::Type::BEHAVIOR:
        default:
            return State::Value{};
    }

}

}
}
