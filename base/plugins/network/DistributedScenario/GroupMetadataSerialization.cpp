#include "GroupMetadata.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom(const GroupMetadata& elt)
{
    readFrom(elt.group());
    insertDelimiter();
}

template<>
void Visitor<Reader<JSON>>::readFrom(const GroupMetadata& elt)
{
    m_obj["Id"] = toJsonObject(elt.group());
}


template<>
void Visitor<Writer<DataStream>>::writeTo(GroupMetadata& elt)
{
    id_type<Group> id;
    m_stream >> id;
    elt.setGroup(id);

    checkDelimiter();
}

template<>
void Visitor<Writer<JSON>>::writeTo(GroupMetadata& elt)
{
    elt.setGroup(fromJsonObject<id_type<Group>>(m_obj["Id"].toObject()));
}
