#pragma once
#include <iscore/serialization/VisitorInterface.hpp>
#include <QByteArray>
#include <QDataStream>
#include <sys/types.h>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <iscore/tools/NamedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/std/Optional.hpp>
namespace iscore
{
struct ApplicationContext;
}

template<typename T>
using enable_if_QDataStreamSerializable =
    typename std::enable_if_t<
        std::is_arithmetic<T>::value ||
        std::is_same<T, QStringList>::value ||
        std::is_same<T, QVector3D>::value ||
        std::is_same<T, QPointF>::value ||
        std::is_same<T, QPoint>::value
>;

template <class, class Enable = void>
struct is_QDataStreamSerializable : std::false_type {};

template <class T>
struct is_QDataStreamSerializable<T, enable_if_QDataStreamSerializable<typename std::decay<T>::type> > : std::true_type {};



class QIODevice;
class QStringList;
template <typename model> class IdentifiedObject;

struct DataStreamInput
{
        QDataStream& stream;

        template<typename T>
        friend DataStreamInput& operator<<(DataStreamInput& s, T&& obj)
        {
            s.stream << obj;
            return s;
        }
};

struct DataStreamOutput
{
        QDataStream& stream;

        template<typename T>
        friend DataStreamOutput& operator>>(DataStreamOutput& s, T&& obj)
        {
            s.stream >> obj;
            return s;
        }
};


/**
 * This file contains facilities
 * to serialize an object using QDataStream.
 *
 * Generally, it is used with QByteArrays, but it works with any QIODevice.
 */
class DataStream;

class DataStream
{
    public:
        using Serializer = Visitor<Reader<DataStream>>;
        using Deserializer = Visitor<Writer<DataStream>>;
        static constexpr SerializationIdentifier type()
        {
            return 2;
        }
};

template<>
class ISCORE_LIB_BASE_EXPORT Visitor<Reader<DataStream>> : public AbstractVisitor
{
    public:
        using is_visitor_tag = std::integral_constant<bool, true>;

        VisitorVariant toVariant() { return {*this, DataStream::type()}; }

        Visitor();
        Visitor(const Visitor&) = delete;
        Visitor& operator=(const Visitor&) = delete;

        Visitor(QByteArray* array);
        Visitor(QIODevice* dev);

        template<typename T>
        static auto marshall(const T& t)
        {
            QByteArray arr;
            Visitor<Reader<DataStream>> reader{&arr};
            reader.readFrom(t);
            return arr;
        }

        template<template<class...> class T,
                 typename... Args>
        void readFrom(
                const T<Args...>& obj,
                typename std::enable_if_t<
                    is_template<T<Args...>>::value &&
                    !is_abstract_base<T<Args...>>::value> * = nullptr)
        {
            TSerializer<DataStream, void, T<Args...>>::readFrom(*this, obj);
        }

        template<typename T,
                 std::enable_if_t<
                     is_abstract_base<T>::value
                     >* = nullptr>
        void readFrom(const T& obj)
        {
            AbstractSerializer<DataStream, T>::readFrom(*this, obj);
        }

        template<typename T>
        void readFrom_impl(const T&);

        template<typename T,
                 std::enable_if_t<
                     !std::is_enum<T>::value &&
                     !is_template<T>::value &&
                     !is_abstract_base<T>::value
                     >* = nullptr>
        void readFrom(const T&);

        template<typename T,
                 std::enable_if_t<
                     std::is_enum<T>::value &&
                     !is_template<T>::value>* = nullptr>
        void readFrom(const T& elt)
        {
            m_stream << (int32_t) elt;
        }

        /**
         * @brief insertDelimiter
         *
         * Adds a delimiter that is to be checked by the reader.
         */
        void insertDelimiter()
        {
            m_stream << int32_t (0xDEADBEEF);
        }

        auto& stream() { return m_stream; }
    private:
        QDataStream m_stream_impl;

    public:
        const iscore::ApplicationContext& context;
        DataStreamInput m_stream{m_stream_impl};
};

template<>
class ISCORE_LIB_BASE_EXPORT Visitor<Writer<DataStream>> : public AbstractVisitor
{
    public:
        using is_visitor_tag = std::integral_constant<bool, true>;

        VisitorVariant toVariant() { return {*this, DataStream::type()}; }

        Visitor();
        Visitor(const Visitor&) = delete;
        Visitor& operator=(const Visitor&) = delete;

        Visitor(const QByteArray& array);
        Visitor(QIODevice* dev);


        template<typename T>
        static auto unmarshall(const QByteArray& arr)
        {
            T data;
            Visitor<Writer<DataStream>> wrt{arr};
            wrt.writeTo(data);
            return data;
        }

        template<
                template<class...> class T,
                typename... Args>
        void writeTo(
                T<Args...>& obj,
                typename std::enable_if<is_template<T<Args...>>::value, void>::type * = nullptr)
        {
            TSerializer<DataStream, void, T<Args...>>::writeTo(*this, obj);
        }

        template<typename T,
                 std::enable_if_t<!std::is_enum<T>::value && !is_template<T>::value>* = nullptr >
        void writeTo(T&);

        template<typename T,
                 std::enable_if_t<std::is_enum<T>::value && !is_template<T>::value>* = nullptr>
        void writeTo(T& elt)
        {
            int32_t e;
            m_stream >> e;
            elt = static_cast<T>(e);
        }

        /**
         * @brief checkDelimiter
         *
         * Checks if a delimiter is present at the current
         * stream position, and fails if it isn't.
         */
        void checkDelimiter()
        {
            int val{};
            m_stream >> val;

            if(val != int32_t (0xDEADBEEF))
            {
                ISCORE_BREAKPOINT;
                throw std::runtime_error("Corrupt save file.");
            }
        }

        auto& stream() { return m_stream; }
    private:
        QDataStream m_stream_impl;

    public:
        const iscore::ApplicationContext& context;
        DataStreamOutput m_stream{m_stream_impl};
};


// TODO instead why not add a iscore_serializable tag to our classes ?
template<typename T,
         std::enable_if_t<
             ! std::is_arithmetic<T>::value
          && ! std::is_same<T, QStringList>::value>* = nullptr>
QDataStream& operator<< (QDataStream& stream, const T& obj)
{
    Visitor<Reader<DataStream>> reader{stream.device()};
    reader.readFrom(obj);
    return stream;
}

template<typename T,
         std::enable_if_t<
             ! std::is_arithmetic<T>::value
          && ! std::is_same<T, QStringList>::value>* = nullptr>
QDataStream& operator>> (QDataStream& stream, T& obj)
{
    Visitor<Writer<DataStream>> writer{stream.device()};
    writer.writeTo(obj);

    return stream;
}

template<typename U>
struct TSerializer<DataStream, void, Id<U>>
{
    static void readFrom(
            DataStream::Serializer& s,
            const Id<U>& obj)
    {
        s.stream() << bool (obj.val());

        if(obj.val())
        {
            s.stream() << *obj.val();
        }
    }

    static void writeTo(
            DataStream::Deserializer& s,
            Id<U>& obj)
    {
        bool init {};
        int32_t val {};
        s.stream() >> init;

        if(init)
        {
            s.stream() >> val;
            obj.setVal(val);
        }
        else
        {
            obj.unset();
        }
    }
};


template<typename T>
struct TSerializer<DataStream, void, IdentifiedObject<T>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const IdentifiedObject<T>& obj)
        {
            s.readFrom(static_cast<const NamedObject&>(obj));
            s.readFrom(obj.id());
        }

        static void writeTo(
                DataStream::Deserializer& s,
                IdentifiedObject<T>& obj)
        {
            Id<T> id;
            s.writeTo(id);
            obj.setId(std::move(id));
        }

};

template<typename T>
struct TSerializer<DataStream, void, optional<T>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const optional<T>& obj)
        {
            s.stream() << static_cast<bool>(obj);

            if(obj)
            {
                s.stream() << *obj;
            }
        }

        static void writeTo(
                DataStream::Deserializer& s,
                optional<T>& obj)
        {
            bool b {};
            s.stream() >> b;

            if(b)
            {
                T val;
                s.stream() >> val;
                obj = val;
            }
            else
            {
                obj = iscore::none;
            }
        }
};

template<typename T>
struct TSerializer<DataStream, void, QList<T>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const QList<T>& obj)
        {
            s.stream() << obj;
        }

        static void writeTo(
                DataStream::Deserializer& s,
                QList<T>& obj)
        {
            s.stream() >> obj;
        }
};

template<typename... Args>
struct TSerializer<
        DataStream,
        std::enable_if_t<!is_QDataStreamSerializable<typename std::vector<Args...>::value_type>::value>,
        std::vector<Args...>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const std::vector<Args...>& vec)
        {
            s.stream() << (int32_t)vec.size();
            for(const auto& elt : vec)
                s.readFrom(elt);

            s.insertDelimiter();
        }

        static void writeTo(
                DataStream::Deserializer& s,
                std::vector<Args...>& vec)
        {
            int32_t n = 0;
            s.stream() >> n;

            vec.clear();
            vec.resize(n);
            for(int i = 0; i < n; i++)
            {
                s.writeTo(vec[i]);
            }

            s.checkDelimiter();
        }
};

template<typename... Args>
struct TSerializer<
        DataStream,
        std::enable_if_t<is_QDataStreamSerializable<typename std::vector<Args...>::value_type>::value>,
        std::vector<Args...>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const std::vector<Args...>& vec)
        {
            s.stream() << (int32_t)vec.size();
            for(const auto& elt : vec)
                s.stream() << elt;

            s.insertDelimiter();
        }

        static void writeTo(
                DataStream::Deserializer& s,
                std::vector<Args...>& vec)
        {
            int32_t n = 0;
            s.stream() >> n;

            vec.clear();
            vec.resize(n);
            for(int i = 0; i < n; i++)
            {
                s.stream() >> vec[i];
            }

            s.checkDelimiter();
        }
};


Q_DECLARE_METATYPE(Visitor<Reader<DataStream>>*)
Q_DECLARE_METATYPE(Visitor<Writer<DataStream>>*)
