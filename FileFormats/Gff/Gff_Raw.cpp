#include "FileFormats/Gff/Gff_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Gff::Raw {

bool Gff::ReadFromBytes(std::byte const* bytes, Gff* out)
{
    ASSERT(bytes);
    ASSERT(out);

    std::memcpy(&out->m_Header, bytes, sizeof(out->m_Header));

    if (std::memcmp(out->m_Header.m_FileVersion, "V3.2", 4) != 0)
    {
        return false;
    }

    out->ReadStructs(bytes);
    out->ReadFields(bytes);
    out->ReadLabels(bytes);
    out->ReadFieldData(bytes);
    out->ReadFieldIndices(bytes);
    out->ReadLists(bytes);

    return true;
}

namespace {

template <typename T>
T ReadSimpleType(GffField const& field, GffField::Type type)
{
    ASSERT(field.m_Type == type);
    T value;
    std::memcpy(&value, &field.m_DataOrDataOffset, sizeof(value));
    return value;
}

}

GffField::Type_BYTE Gff::ConstructBYTE(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_BYTE>(field, GffField::Type::BYTE);
}

GffField::Type_CHAR Gff::ConstructCHAR(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_CHAR>(field, GffField::Type::CHAR);
}

GffField::Type_WORD Gff::ConstructWORD(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_WORD>(field, GffField::Type::WORD);
}

GffField::Type_SHORT Gff::ConstructSHORT(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_SHORT>(field, GffField::Type::SHORT);
}

GffField::Type_DWORD Gff::ConstructDWORD(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_DWORD>(field, GffField::Type::DWORD);
}

GffField::Type_INT Gff::ConstructINT(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_INT>(field, GffField::Type::INT);
}

GffField::Type_DWORD64 Gff::ConstructDWORD64(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::DWORD64);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_DWORD64 value;
    std::memcpy(&value, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(value));
    return value;
}

GffField::Type_INT64 Gff::ConstructINT64(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::INT64);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_INT64 value;
    std::memcpy(&value, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(value));
    return value;
}

GffField::Type_FLOAT Gff::ConstructFLOAT(GffField const& field) const
{
    return ReadSimpleType<GffField::Type_FLOAT>(field, GffField::Type::FLOAT);
}

GffField::Type_DOUBLE Gff::ConstructDOUBLE(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::DOUBLE);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_DOUBLE value;
    std::memcpy(&value, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(value));
    return value;
}

GffField::Type_CExoString Gff::ConstructCExoString(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::CExoString);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_CExoString string;

    std::uint32_t length;
    std::memcpy(&length, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(length));

    string.m_String = std::string(reinterpret_cast<char const*>(m_FieldData.data() + offsetIntoFieldDataArray + sizeof(length)), length);

    return string;
}

GffField::Type_CResRef Gff::ConstructResRef(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::ResRef);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_CResRef resref;

    std::memcpy(&resref.m_Size, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(resref.m_Size));
    std::memcpy(&resref.m_String, m_FieldData.data() + offsetIntoFieldDataArray + sizeof(resref.m_Size), sizeof(resref.m_String));

    return resref;
}

GffField::Type_CExoLocString Gff::ConstructCExoLocString(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::CExoLocString);
    return {}; // TODO
}

GffField::Type_VOID Gff::ConstructVOID(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::VOID);
    return {}; // TODO
}

GffField::Type_Struct Gff::ConstructStruct(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::Struct);
    return m_Structs[field.m_DataOrDataOffset];
}

GffField::Type_List Gff::ConstructList(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::List);

    std::uint32_t offsetIntoListIndicesArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoListIndicesArray < m_ListIndices.size());

    GffField::Type_List list;

    std::uint32_t length;
    std::memcpy(&length, m_ListIndices.data() + offsetIntoListIndicesArray, sizeof(length));

    list.m_Elements.resize(length);
    std::memcpy(list.m_Elements.data(), m_ListIndices.data() + offsetIntoListIndicesArray + sizeof(length), length * sizeof(std::uint32_t));

    return list;
}

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    for (std::uint32_t i = 0; i < count; ++i)
    {
        T entry;

        std::size_t len = sizeof(T);
        std::memcpy(&entry, bytesWithInitialOffset, len);
        out.emplace_back(entry);

        bytesWithInitialOffset += len;
    }
}

}

void Gff::ReadStructs(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_StructOffset;
    std::uint32_t count = m_Header.m_StructCount;
    ReadGenericOffsetable(data + offset, count, m_Structs);
}

void Gff::ReadFields(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_FieldOffset;
    std::uint32_t count = m_Header.m_FieldCount;
    ReadGenericOffsetable(data + offset, count, m_Fields);
}

void Gff::ReadLabels(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_LabelOffset;
    std::uint32_t count = m_Header.m_LabelCount;
    ReadGenericOffsetable(data + offset, count, m_Labels);
}

void Gff::ReadFieldData(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_FieldDataOffset;
    std::uint32_t count = m_Header.m_FieldDataCount;
    ReadGenericOffsetable(data + offset, count, m_FieldData);
}

void Gff::ReadFieldIndices(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_FieldIndicesOffset;
    std::uint32_t count = m_Header.m_FieldIndicesCount / sizeof(offset);
    ReadGenericOffsetable(data + offset, count, m_FieldIndices);
}

void Gff::ReadLists(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_ListIndicesOffset;
    std::uint32_t count = m_Header.m_ListIndicesCount;
    ReadGenericOffsetable(data + offset, count, m_ListIndices);
}

}
