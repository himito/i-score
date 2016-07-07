#include <eggs/variant/variant.hpp>
#include <QJsonArray>
#include <QList>
#include <QLocale>
#include <QMetaType>
#include <QObject>

#include <QStringList>
#include <algorithm>
#include <array>
#include <iterator>

#include <State/Value.hpp>
#include "ValueConversion.hpp"
#include "Expression.hpp"

namespace State
{
namespace convert
{

static const std::array<const QString, 8> ValueTypesArray{{
        QStringLiteral("Impulse"),
                QStringLiteral("Int"),
                QStringLiteral("Float"),
                QStringLiteral("Bool"),
                QStringLiteral("String"),
                QStringLiteral("Char"),
                QStringLiteral("Tuple"),
                QStringLiteral("None")
                                                          }};

static const std::array<const QString, 8> ValuePrettyTypesArray{{
        QObject::tr("Impulse"),
                QObject::tr("Int"),
                QObject::tr("Float"),
                QObject::tr("Bool"),
                QObject::tr("String"),
                QObject::tr("Char"),
                QObject::tr("Tuple"),
                QObject::tr("None")
                                                                }};

static const std::array<std::pair<QString, ValueType>, 7>
    ValuePrettyTypesPairArray{{
        std::make_pair(QObject::tr("Impulse"), ValueType::Impulse),
        std::make_pair(QObject::tr("Int"), ValueType::Int),
        std::make_pair(QObject::tr("Float"), ValueType::Float),
        std::make_pair(QObject::tr("Bool"), ValueType::Bool),
        std::make_pair(QObject::tr("String"), ValueType::String),
        std::make_pair(QObject::tr("Char"), ValueType::Char),
        std::make_pair(QObject::tr("Tuple"), ValueType::Tuple)}};

template<>
QVariant value(const State::Value& val)
{
    struct vis {
        public:
            using return_type = QVariant;
            return_type operator()(const no_value_t&) const { return QVariant::fromValue(State::no_value_t{}); }
            return_type operator()(const impulse_t&) const { return QVariant::fromValue(State::impulse_t{}); }
            return_type operator()(int i) const { return QVariant::fromValue(i); }
            return_type operator()(float f) const { return QVariant::fromValue(f); }
            return_type operator()(bool b) const { return QVariant::fromValue(b); }
            return_type operator()(const QString& s) const { return QVariant::fromValue(s); }
            return_type operator()(const QChar& c) const { return QVariant::fromValue(c); }

            return_type operator()(const tuple_t& t) const
            {
                QVariantList arr;
                arr.reserve(t.size());

                for(const auto& elt : t)
                {
                    arr.push_back(eggs::variants::apply(*this, elt.impl()));
                }

                return arr;
            }
    };

    return eggs::variants::apply(vis{}, val.val.impl());
}

template<>
QJsonValue value(const State::Value& val)
{
    struct vis {
        public:
            using return_type = QJsonValue;
            return_type operator()(const no_value_t&) const { return {}; }
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int i) const { return i; }
            return_type operator()(float f) const { return f; }
            return_type operator()(bool b) const { return b; }
            return_type operator()(const QString& s) const { return s; }

            return_type operator()(const QChar& c) const
            {
                // Note : it is saved as a string but the actual type should be saved also
                // so that the QChar can be recovered.
                return QString(c);
            }

            return_type operator()(const tuple_t& t) const
            {
                QJsonArray arr;

                for(const auto& elt : t)
                {
                    QJsonObject obj;
                    obj[iscore::StringConstant().Type] = textualType(elt);
                    obj[iscore::StringConstant().Value] = eggs::variants::apply(*this, elt.impl());
                    arr.append(obj);
                }

                return arr;
            }
    };

    return eggs::variants::apply(vis{}, val.val.impl());
}


QString textualType(const State::Value& val)
{
    const auto& impl = val.val.impl();
    ISCORE_ASSERT(impl.which() < ValueTypesArray.size());
    return ValueTypesArray.at(impl.which());
}

static ValueType which(const QString& val)
{
    auto it = find(ValueTypesArray, val);
    ISCORE_ASSERT(it != ValueTypesArray.end()); // What happens if there is a corrupt save file ?
    return static_cast<State::ValueType>(std::distance(ValueTypesArray.begin(), it));
}

static State::ValueImpl fromQJsonValueImpl(
        const QJsonValue& val)
{
    switch(val.type())
    {
        case QJsonValue::Type::Null:
            return no_value_t{};
        case QJsonValue::Type::Bool:
            return val.toBool();
        case QJsonValue::Type::Double:
            return val.toDouble();
        case QJsonValue::Type::String:
            return val.toString();
        case QJsonValue::Type::Array:
        {
            const auto& arr = val.toArray();
            State::tuple_t tuple;
            tuple.reserve(arr.size());

            for(const auto& v : arr)
            {
                tuple.push_back(fromQJsonValueImpl(v));
            }

            return tuple;
        }
        case QJsonValue::Type::Object:
            return no_value_t{};
        case QJsonValue::Type::Undefined:
            return no_value_t{};
        default:
            return no_value_t{};
    }
}

State::Value fromQJsonValue(const QJsonValue& val)
{
    return State::Value::fromValue(fromQJsonValueImpl(val));
}


static State::ValueImpl fromQJsonValueImpl(const QJsonValue& val, State::ValueType type)
{
    if(val.isNull())
        return State::ValueImpl{State::no_value_t{}};

    switch(type)
    {
        case ValueType::NoValue:
            return State::ValueImpl{State::no_value_t{}};
        case ValueType::Impulse:
            return State::ValueImpl{State::impulse_t{}};
        case ValueType::Int:
            return State::ValueImpl{val.toInt()};
        case ValueType::Float:
            return State::ValueImpl{val.toDouble()};
        case ValueType::Bool:
            return State::ValueImpl{val.toBool()};
        case ValueType::String:
            return State::ValueImpl{val.toString()};
        case ValueType::Char:
        {
            auto str = val.toString();
            if(!str.isEmpty())
                return State::ValueImpl{str[0]};
            return State::ValueImpl{QChar{}};
        }
        case ValueType::Tuple:
        {
            auto arr = val.toArray();
            State::tuple_t tuple;
            tuple.reserve(arr.size());

            Foreach(arr, [&] (const auto& elt)
            {
                auto obj = elt.toObject();
                auto type_it = obj.find(iscore::StringConstant().Type);
                auto val_it = obj.find(iscore::StringConstant().Value);
                if(val_it != obj.end() && type_it != obj.end())
                {
                    tuple.push_back(fromQJsonValueImpl(*val_it, which((*type_it).toString())));
                }
            });

            return State::ValueImpl{tuple};
        }
        default:
            ISCORE_ABORT;
            throw;
    }
}

static State::Value fromQJsonValue(const QJsonValue& val, ValueType which)
{
    return State::Value{fromQJsonValueImpl(val, which)};
}

State::Value fromQJsonValue(const QJsonValue& val, const QString& type)
{
    return fromQJsonValue(val, which(type));
}



QString prettyType(const State::Value& val)
{
    const auto& impl = val.val.impl();
    ISCORE_ASSERT(impl.which() < ValuePrettyTypesArray.size());
    return ValuePrettyTypesArray.at(impl.which());
}


const QStringList& ValuePrettyTypesList()
{
    static bool init = false;
    static QStringList lst;
    if(!init)
    {
        for(const auto& str : ValuePrettyTypesArray)
            lst.append(str);
        init = true;
    }
    return lst;
}

template<>
int value(const State::Value& val)
{
    struct {
        public:
            using return_type = int;
            return_type operator()(const no_value_t&) const { return 0; }
            return_type operator()(const impulse_t&) const { return 0; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const { return QLocale::c().toInt(v); }
            return_type operator()(const QChar& v) const { return QLocale::c().toInt(QString(v)); }
            return_type operator()(const tuple_t& v) const { return 0; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
float value(const State::Value& val)
{
    struct {
        public:
            using return_type = float;
            return_type operator()(const no_value_t&) const { return {}; }
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const { return QLocale::c().toFloat(v); }
            return_type operator()(const QChar& v) const { return QLocale::c().toFloat(QString(v)); }
            return_type operator()(const tuple_t& v) const { return {}; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}
template<>
double value(const State::Value& val)
{
    return (double) value<float>(val);
}

template<>
bool value(const State::Value& val)
{
    struct {
        public:
            using return_type = bool;
            return_type operator()(const no_value_t&) const { return {}; }
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const {
                return v == iscore::StringConstant().lowercase_true ||
                       v == iscore::StringConstant().True ||
                       v == iscore::StringConstant().lowercase_yes||
                       v == iscore::StringConstant().Yes; }
            return_type operator()(const QChar& v) const { return v == 't' || v == 'T' || v == 'y' || v == 'Y'; }
            return_type operator()(const tuple_t& v) const { return false; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
QChar value(const State::Value& val)
{
    struct {
        public:
            using return_type = QChar;
            return_type operator()(const no_value_t&) const { return '-'; }
            return_type operator()(const impulse_t&) const { return '-'; }
            return_type operator()(int) const { return '-'; }
            return_type operator()(float) const { return '-'; }
            return_type operator()(bool v) const { return v ? 'T' : 'F'; }
            return_type operator()(const QString& s) const { return !s.isEmpty() ? s[0] : '-'; } // TODO boueeeff
                return_type operator()(const QChar& v) const { return  v; }
            return_type operator()(const tuple_t&) const { return '-'; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
QString value(const State::Value& val)
{
    struct {
            using return_type = QString;
            return_type operator()(const State::no_value_t&) const { return {}; }
            return_type operator()(const State::impulse_t&) const { return {}; }
            return_type operator()(int i) const { return QLocale::c().toString(i); }
            return_type operator()(float f) const { return QLocale::c().toString(f); }
            return_type operator()(bool b) const {
                return b
                        ? iscore::StringConstant().lowercase_true
                        : iscore::StringConstant().lowercase_false;
            }
            return_type operator()(const QString& s) const { return s; }
            return_type operator()(const QChar& c) const { return c; }
            return_type operator()(const State::tuple_t& t) const { return ""; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
tuple_t value(const State::Value& val)
{
    struct vis {
            using return_type = tuple_t;
            return_type operator()(const State::no_value_t&) const { return {impulse_t{}}; }
            return_type operator()(const State::impulse_t&) const { return {impulse_t{}}; }
            return_type operator()(int i) const { return {i}; }
            return_type operator()(float f) const { return {f}; }
            return_type operator()(bool b) const {
                return {b};
            }
            return_type operator()(const QString& s) const {
                auto v = parseValue(s);

                if(v && v->val.is<tuple_t>())
                    return v->val.get<State::tuple_t>();

                return {s};
            }
            return_type operator()(const QChar& c) const { return {c}; }
            return_type operator()(const State::tuple_t& t) const { return t; }
    };

    return eggs::variants::apply(vis{}, val.val.impl());
}


QString toPrettyString(const State::Value& val)
{
    struct vis {
            QString operator()(const State::no_value_t&) const { return {}; }
            QString operator()(const State::impulse_t&) const { return {}; }
            QString operator()(int i) const {
                const auto& loc = QLocale::c();
                auto str = loc.toString(i);
                str.remove(loc.groupSeparator());
                return str;
            }
            QString operator()(float f) const {
                const auto& loc = QLocale::c();
                auto str = loc.toString(f);
                str.remove(loc.groupSeparator());
                return str;
            }
            QString operator()(bool b) const {
                return b
                        ? iscore::StringConstant().lowercase_true
                        : iscore::StringConstant().lowercase_false;
            }
            QString operator()(const QString& s) const
            {
                // TODO escape ?
                return QString("\"%1\"").arg(s);
            }

            QString operator()(const QChar& c) const
            {
                return QString("'%1'").arg(c);
            }

            QString operator()(const State::tuple_t& t) const
            {
                QString s{"["};

                auto n = t.size();
                if(n >= 1)
                {
                    s += eggs::variants::apply(*this, t[0].impl());
                }

                for(std::size_t i = 1; i < n; i++)
                {
                    s += ", ";
                    s += eggs::variants::apply(*this, t[i].impl());
                }

                s+= "]";
                return s;
            }
    };

    return eggs::variants::apply(vis{}, val.val.impl());
}


bool convert(const State::Value& orig, State::Value& toConvert)
{
    switch(orig.val.which())
    {
        case ValueType::NoValue:
            toConvert.val = no_value_t{};
            break;
        case ValueType::Impulse:
            toConvert.val = impulse_t{};
            break;
        case ValueType::Int:
            toConvert.val = value<int>(toConvert);
            break;
        case ValueType::Float:
            toConvert.val = value<float>(toConvert);
            break;
        case ValueType::Bool:
            toConvert.val = value<bool>(toConvert);
            break;
        case ValueType::String:
            toConvert.val = value<QString>(toConvert);
            break;
        case ValueType::Char:
            toConvert.val = value<QChar>(toConvert);
            break;
        case ValueType::Tuple:
            toConvert.val = value<tuple_t>(toConvert);
            break;
        default:
            break;
    }

    return true;
}


static State::ValueImpl fromQVariantImpl(const QVariant& val)
{
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch(auto t = QMetaType::Type(val.type()))
    {
        case QMetaType::Int:
            return State::ValueImpl{val.toInt()};
        case QMetaType::UInt:
            return State::ValueImpl{(int)val.toUInt()};
        case QMetaType::Long:
            return State::ValueImpl{(int)val.value<int64_t>()};
        case QMetaType::LongLong:
            return State::ValueImpl{(int)val.toLongLong()};
        case QMetaType::ULong:
            return State::ValueImpl{(int)val.value<uint64_t>()};
        case QMetaType::ULongLong:
            return State::ValueImpl{(int)val.toULongLong()};
        case QMetaType::Short:
            return State::ValueImpl{(int)val.value<int16_t>()};
        case QMetaType::UShort:
            return State::ValueImpl{(int)val.value<uint16_t>()};
        case QMetaType::Float:
            return State::ValueImpl{val.toFloat()};
        case QMetaType::Double:
            return State::ValueImpl{(float)val.toDouble()};
        case QMetaType::Bool:
            return State::ValueImpl{val.toBool()};
        case QMetaType::QString:
            return State::ValueImpl{val.toString()};
        case QMetaType::Char:
            return State::ValueImpl{(QChar)val.value<char>()};
        case QMetaType::QChar:
            return State::ValueImpl{val.toChar()};
        case QMetaType::QVariantList:
        {
            auto list = val.value<QVariantList>();
            tuple_t tuple_val;
            tuple_val.reserve(list.size());

            Foreach(list, [&] (const auto& elt)
            {
                tuple_val.push_back(fromQVariantImpl(elt));
            });
            return tuple_val;
        }
        default:
        {
            if(t == qMetaTypeId<State::Value>())
            {
                return State::ValueImpl{impulse_t{}};
            }
            else
            {
                return State::ValueImpl{no_value_t{}};
            }
        }
    }

#pragma GCC diagnostic warning "-Wswitch"
#pragma GCC diagnostic warning "-Wswitch-enum"
}


State::Value fromQVariant(const QVariant& val)
{
    return State::Value{fromQVariantImpl(val)};
}

QString prettyType(ValueType t)
{
    return ValuePrettyTypesArray[static_cast<int>(t)];
}

const std::array<std::pair<QString, State::ValueType>, 7> & ValuePrettyTypesMap()
{
    return ValuePrettyTypesPairArray;
}

}
}
