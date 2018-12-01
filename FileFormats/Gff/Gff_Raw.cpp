#include "FileFormats/Gff/Gff_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"

#include <cstring>

namespace FileFormats::Gff::Raw {

bool Gff::ReadFromBytes(std::byte const* bytes, Gff* out)
{
    ASSERT(bytes);
    ASSERT(out);
    return out->ConstructInternal(bytes);
}

bool Gff::ReadFromByteVector(std::vector<std::byte>&& bytes, Gff* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);
    return out->ConstructInternal(bytes.data());
}

bool Gff::ReadFromFile(char const* path, Gff* out)
{
    ASSERT(path);
    ASSERT(out);

    MemoryMappedFile memmap;
    bool loaded = MemoryMappedFile::MemoryMap(path, &memmap);

    if (!loaded)
    {
        return false;
    }

    return out->ConstructInternal(memmap.GetDataBlock().GetData());
}

bool Gff::WriteToFile(char const* path) const
{
    ASSERT(path);

    FILE* outFile = std::fopen(path, "wb");

    if (outFile)
    {
        std::fwrite(&m_Header, sizeof(m_Header), 1, outFile);
        std::fwrite(m_Structs.data(), sizeof(m_Structs[0]), m_Structs.size(), outFile);
        std::fwrite(m_Fields.data(), sizeof(m_Fields[0]), m_Fields.size(), outFile);
        std::fwrite(m_Labels.data(), sizeof(m_Labels[0]), m_Labels.size(), outFile);
        std::fwrite(m_FieldData.data(), sizeof(m_FieldData[0]), m_FieldData.size(), outFile);
        std::fwrite(m_FieldIndices.data(), sizeof(m_FieldIndices[0]), m_FieldIndices.size(), outFile);
        std::fwrite(m_ListIndices.data(), sizeof(m_ListIndices[0]), m_ListIndices.size(), outFile);
        std::fclose(outFile);
        return true;
    }

    return false;
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

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_CExoLocString locString;

    std::byte const* ptr = m_FieldData.data() + offsetIntoFieldDataArray;

    std::memcpy(&locString.m_TotalSize, ptr, sizeof(locString.m_TotalSize));
    ptr += sizeof(locString.m_TotalSize);

    std::memcpy(&locString.m_StringRef, ptr, sizeof(locString.m_StringRef));
    ptr += sizeof(locString.m_StringRef);

    std::uint32_t stringCount;
    std::memcpy(&stringCount, ptr, sizeof(stringCount));
    ptr += sizeof(stringCount);

    for (std::size_t i = 0; i < stringCount; ++i)
    {
        GffField::Type_CExoLocString::SubString substring;

        std::memcpy(&substring.m_StringID, ptr, sizeof(substring.m_StringID));
        ptr += sizeof(substring.m_StringID);

        std::uint32_t substringLength;
        std::memcpy(&substringLength, ptr, sizeof(substringLength));
        ptr += sizeof(substringLength);

        substring.m_String = std::string(reinterpret_cast<char const*>(ptr), substringLength);
        ptr += substringLength;

        locString.m_SubStrings.emplace_back(std::move(substring));
    }

    return locString;
}

GffField::Type_VOID Gff::ConstructVOID(GffField const& field) const
{
    ASSERT(field.m_Type == GffField::Type::VOID);

    std::uint32_t offsetIntoFieldDataArray = field.m_DataOrDataOffset;
    ASSERT(offsetIntoFieldDataArray < m_FieldData.size());

    GffField::Type_VOID binary;

    std::uint32_t size;
    std::memcpy(&size, m_FieldData.data() + offsetIntoFieldDataArray, sizeof(size));

    binary.m_Data.resize(size);
    std::memcpy(binary.m_Data.data(), m_FieldData.data() + offsetIntoFieldDataArray + sizeof(size), size);

    return binary;
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

bool Gff::ConstructInternal(std::byte const* bytes)
{
    std::memcpy(&m_Header, bytes, sizeof(m_Header));

    if (std::memcmp(m_Header.m_FileVersion, "V3.2", 4) != 0)
    {
        return false;
    }

    ReadStructs(bytes);
    ReadFields(bytes);
    ReadLabels(bytes);
    ReadFieldData(bytes);
    ReadFieldIndices(bytes);
    ReadLists(bytes);

    return true;
}

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    out.resize(count);
    std::memcpy(out.data(), bytesWithInitialOffset, count * sizeof(T));
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
