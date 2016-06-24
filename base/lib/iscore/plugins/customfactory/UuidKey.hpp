#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/functional/hash.hpp>
#include <iscore/tools/Todo.hpp>
#include <iscore_lib_base_export.h>

#include <iscore/serialization/DataStreamVisitor.hpp>
class JSONObject;

namespace iscore
{
using uuid_t = boost::uuids::uuid;
}

template<typename Tag>
class UuidKey : iscore::uuid_t
{
        using this_type = UuidKey<Tag>;

        friend struct std::hash<this_type>;
        friend struct boost::hash<this_type>;
        friend struct boost::hash<const this_type>;
        friend bool operator==(const this_type& lhs, const this_type& rhs) {
            return static_cast<const iscore::uuid_t&>(lhs) == static_cast<const iscore::uuid_t&>(rhs);
        }
        friend bool operator!=(const this_type& lhs, const this_type& rhs) {
            return static_cast<const iscore::uuid_t&>(lhs) != static_cast<const iscore::uuid_t&>(rhs);
        }
        friend bool operator<(const this_type& lhs, const this_type& rhs) {
            return static_cast<const iscore::uuid_t&>(lhs) < static_cast<const iscore::uuid_t&>(rhs);
        }

    public:
        UuidKey() noexcept = default;
        UuidKey(const UuidKey& other) noexcept = default;
        UuidKey(UuidKey&& other) noexcept = default;
        UuidKey& operator=(const UuidKey& other) noexcept = default;
        UuidKey& operator=(UuidKey&& other) noexcept = default;

        UuidKey(iscore::uuid_t other) noexcept :
            iscore::uuid_t(other)
        {

        }

        UuidKey(const char* txt) noexcept :
            iscore::uuid_t(boost::uuids::string_generator{}(txt))
        {

        }

        template<int N>
        UuidKey(const char txt[N]) noexcept :
            iscore::uuid_t(boost::uuids::string_generator{}(txt, txt + N))
        {

        }

        const iscore::uuid_t& impl() const { return *this; }
        iscore::uuid_t& impl() { return *this; }
};

namespace std
{
template<typename T>
struct hash<UuidKey<T>>
{
        std::size_t operator()(const UuidKey<T>& kagi) const noexcept
        { return boost::hash<boost::uuids::uuid>()(static_cast<const iscore::uuid_t&>(kagi)); }
};
}

namespace boost
{
template<typename T>
struct hash<UuidKey<T>>
{
        std::size_t operator()(const UuidKey<T>& kagi) const noexcept
        { return boost::hash<boost::uuids::uuid>()(static_cast<const iscore::uuid_t&>(kagi)); }
};

template<typename T>
struct hash<const UuidKey<T>>
{
        std::size_t operator()(const UuidKey<T>& kagi) const noexcept
        { return boost::hash<boost::uuids::uuid>()(static_cast<const iscore::uuid_t&>(kagi)); }
};
}

template<typename U>
struct TSerializer<DataStream, void, UuidKey<U>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const UuidKey<U>& uid)
        {
            s.stream().stream.writeRawData((const char*) uid.impl().data, sizeof(uid.impl().data));
        }

        static void writeTo(
                DataStream::Deserializer& s,
                UuidKey<U>& uid)
        {
            s.stream().stream.readRawData((char*) uid.impl().data, sizeof(uid.impl().data));
        }
};


#include <iscore/serialization/JSONValueVisitor.hpp>
template<typename U>
struct TSerializer<JSONValue, UuidKey<U>>
{
        static void readFrom(
                JSONValue::Serializer& s,
                const UuidKey<U>& uid)
        {
            s.readFrom(uid.impl());
        }

        static void writeTo(
                JSONValue::Deserializer& s,
                UuidKey<U>& uid)
        {
            s.writeTo(uid.impl());
        }
};
