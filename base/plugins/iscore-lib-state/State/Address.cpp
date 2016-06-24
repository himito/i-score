#include <algorithm>

#include <State/Address.hpp>
#include <iscore/tools/Todo.hpp>
namespace State
{
    bool Address::validateString(const QString &str)
    {
        auto firstcolon = str.indexOf(":");
        auto firstslash = str.indexOf("/");

        bool valid = str == QString(str.toUtf8())
                      && (firstcolon > 0)
                      && (firstslash == (firstcolon + 1)
                      && !str.contains("//"));

        QStringList path = str.split("/");
        valid &= !path.empty();

        path.first().remove(":");

        Foreach(path, [&] (const auto& fragment) {
            valid &= validateFragment(fragment);
        });

        return valid;
    }

    bool Address::validateFragment(const QString& s)
    {
        // TODO refactor with ExpressionParser.cpp (auto base = +qi::char_("a-zA-Z0-9_~().-");)
        return std::all_of(s.cbegin(), s.cend(), [] (auto uc) {
            auto c = uc.toLatin1();
            return (c >= 'a' && c <= 'z')
                    || (c >= 'A' && c <= 'Z')
                    || (c >= '0' && c <= '9')
                    || (c == '.')
                    || (c == '~')
                    || (c == '_')
                    || (c == '(')
                    || (c == ')')
                    || (c == '-')
                    ;
        });
    }


    Address Address::fromString(const QString &str)
    {
        if (!validateString( str))
            return {"", {""} };

        QStringList path = str.split("/");
        ISCORE_ASSERT(path.size() > 0);

        auto device = path.first().remove(":");
        path.removeFirst(); // Remove the device.
        if(path.first().isEmpty()) // case "device:/"
        {
            return {device, {}};
        }

        return {device, path};
    }

    Address Address::rootAddress()
    {
        return Address();
    }

    QString Address::toString() const
    {
        QString ad =  device + ":/" + path.join("/");
        if (!validateString(ad))
            ad.clear();

        return ad;
    }

    QString AddressAccessor::toString() const
    {
        auto str = address.toString();
        for(auto acc : accessors)
        {
            str += "[" + QString::number(acc) + "]";
        }
        return str;
    }

}
