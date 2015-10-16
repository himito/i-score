#include "ValueConversion.hpp"
#include <QJsonArray>
template<>
QVariant iscore::convert::value(const iscore::Value& val)
{
    ISCORE_TODO;
}

template<>
QJsonValue iscore::convert::value(const iscore::Value& val)
{
    static const constexpr struct {
        public:
            using return_type = QJsonValue;
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int i) const { return i; }
            return_type operator()(float f) const { return f; }
            return_type operator()(bool b) const { return b; }
            return_type operator()(const QString& s) const { return s; }

            return_type operator()(const QChar& c) const
            {
                // TODO check this
                return QString(c);
            }

            return_type operator()(const tuple_t& t) const
            {
                QJsonArray arr;

                for(const auto& elt : t)
                {
                    arr.append(eggs::variants::apply(*this, elt.impl()));
                }

                return arr;
            }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}


QString iscore::convert::textualType(const iscore::Value& val)
{
    // TODO apply_type ?
    static const constexpr struct {
        public:
            using return_type = QString;
            return_type operator()(const impulse_t&) const { return "Impulse"; }
            return_type operator()(int) const { return "Int"; }
            return_type operator()(float) const { return "Float"; }
            return_type operator()(bool) const { return "Bool"; }
            return_type operator()(const QString&) const { return "String"; }
            return_type operator()(const QChar&) const { return "Char"; }
            return_type operator()(const tuple_t&) const { return "Tuple"; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

#include <array>

static std::size_t which(const QString& val)
{
    static const std::array<QString, 7> types{{
        QString{"Impulse"}, QString{"Int"}, QString{"Float"}, QString{"Bool"}, QString{"String"}, QString{"Char"}, QString{"Tuple"}
    }};

    auto it = std::find(types.begin(), types.end(), val);
    if(it != types.end())
        return std::distance(types.begin(), it);
    return iscore::ValueImpl::variant_t::npos;
}


static iscore::Value toValue(const QJsonValue& val, std::size_t which)
{
    ISCORE_TODO;
}

iscore::Value iscore::convert::toValue(const QJsonValue& val, const QString& type)
{
    return toValue(val, which(type));
}



QString iscore::convert::prettyType(const iscore::Value& val)
{
    // TODO apply_type ?

    // TODO find a way to keep this in sync with
    // AddressSettingsFactory::AddressSettingsFactory()
    static const constexpr struct {
        public:
            using return_type = QString;
            return_type operator()(const impulse_t&) const { return QObject::tr("Impulse"); }
            return_type operator()(int) const { return QObject::tr("Int"); }
            return_type operator()(float) const { return QObject::tr("Float"); }
            return_type operator()(bool) const { return QObject::tr("Bool"); }
            return_type operator()(const QString&) const { return QObject::tr("String"); }
            return_type operator()(const QChar&) const { return QObject::tr("Char"); }
            return_type operator()(const tuple_t&) const { return QObject::tr("Tuple"); }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}


QStringList iscore::convert::prettyTypes()
{
    return {
        QString{"Impulse"}, QString{"Int"}, QString{"Float"}, QString{"Bool"}, QString{"String"}, QString{"Char"}, QString{"Tuple"}
    };
}

template<>
int iscore::convert::value(const iscore::Value& val)
{
    static const constexpr struct {
        public:
            using return_type = int;
            return_type operator()(const impulse_t&) const { return 0; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const { return v.toInt(); }
            return_type operator()(const QChar& v) const { return QString(v).toInt(); }
            return_type operator()(const tuple_t& v) const { return 0; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
float iscore::convert::value(const iscore::Value& val)
{
    static const constexpr struct {
        public:
            using return_type = float;
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const { return v.toInt(); }
            return_type operator()(const QChar& v) const { return QString(v).toInt(); }
            return_type operator()(const tuple_t& v) const { return {}; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}
template<>
double iscore::convert::value(const iscore::Value& val)
{
    return (double) iscore::convert::value<float>(val);
}

template<>
bool iscore::convert::value(const iscore::Value& val)
{
    static const constexpr struct {
        public:
            using return_type = bool;
            return_type operator()(const impulse_t&) const { return {}; }
            return_type operator()(int v) const { return v; }
            return_type operator()(float v) const { return v; }
            return_type operator()(bool v) const { return v; }
            return_type operator()(const QString& v) const { return v == "true" || v == "True"; } // TODO boueeeff
            return_type operator()(const QChar& v) const { return v.toLatin1(); }
            return_type operator()(const tuple_t& v) const { return false; }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}

template<>
QString iscore::convert::value(const iscore::Value& val)
{
    static const constexpr struct {
            QString operator()(const iscore::impulse_t&) const { return {}; }
            QString operator()(int i) const { return QString::number(i); }
            QString operator()(float f) const { return QString::number(f); }
            QString operator()(bool b) const {
                static const QString tr = "true";
                static const QString f = "false";
                return b ? tr : f;
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

            QString operator()(const iscore::tuple_t& t) const
            {
                QString s{"["};

                for(const auto& elt : t)
                {
                    s += eggs::variants::apply(*this, elt.impl());
                    s += ", ";
                }

                s+= "]";
                return s;
            }
    } visitor{};

    return eggs::variants::apply(visitor, val.val.impl());
}