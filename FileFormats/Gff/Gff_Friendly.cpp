#include "FileFormats/Gff/Gff_Friendly.hpp"

#include <cstring>
#include <memory>
#include <unordered_map>

namespace FileFormats::Gff::Friendly {

Raw::GffField::Type GetTypeFromType(const Type_BYTE&)
{
    return Raw::GffField::Type::BYTE;
}

Raw::GffField::Type GetTypeFromType(const Type_CHAR&)
{
    return Raw::GffField::Type::CHAR;
}

Raw::GffField::Type GetTypeFromType(const Type_WORD&)
{
    return Raw::GffField::Type::WORD;
}

Raw::GffField::Type GetTypeFromType(const Type_SHORT&)
{
    return Raw::GffField::Type::SHORT;
}

Raw::GffField::Type GetTypeFromType(const Type_DWORD&)
{
    return Raw::GffField::Type::DWORD;
}

Raw::GffField::Type GetTypeFromType(const Type_INT&)
{
    return Raw::GffField::Type::INT;
}

Raw::GffField::Type GetTypeFromType(const Type_DWORD64&)
{
    return Raw::GffField::Type::DWORD64;
}

Raw::GffField::Type GetTypeFromType(const Type_INT64&)
{
    return Raw::GffField::Type::INT64;
}

Raw::GffField::Type GetTypeFromType(const Type_FLOAT&)
{
    return Raw::GffField::Type::FLOAT;
}

Raw::GffField::Type GetTypeFromType(const Type_DOUBLE&)
{
    return Raw::GffField::Type::DOUBLE;
}

Raw::GffField::Type GetTypeFromType(const Type_CExoString&)
{
    return Raw::GffField::Type::CExoString;
}

Raw::GffField::Type GetTypeFromType(const Type_CResRef&)
{
    return Raw::GffField::Type::ResRef;
}

Raw::GffField::Type GetTypeFromType(const Type_CExoLocString&)
{
    return Raw::GffField::Type::CExoLocString;
}

Raw::GffField::Type GetTypeFromType(const Type_VOID&)
{
    return Raw::GffField::Type::VOID;
}

Raw::GffField::Type GetTypeFromType(const Type_Struct&)
{
    return Raw::GffField::Type::Struct;
}

Raw::GffField::Type GetTypeFromType(const Type_List&)
{
    return Raw::GffField::Type::List;
}

GffStruct::GffStruct(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff)
{
    ConstructInternal(rawStruct, rawGff);
}

GffStruct::GffStruct(Raw::GffField const& rawField, Raw::Gff const& rawGff)
{
    ConstructInternal(rawGff.ConstructStruct(rawField), rawGff);
}

GffStruct::FieldMap const& GffStruct::GetFields() const
{
    return m_Fields;
}

bool GffStruct::DeleteField(std::string const& fieldName)
{
    auto iter = m_Fields.find(fieldName);

    if (iter != std::end(m_Fields))
    {
        m_Fields.erase(iter);
        return true;
    }

    return false;
}

std::uint32_t GffStruct::GetUserDefinedId() const
{
    return m_UserDefinedId;
}

void GffStruct::SetUserDefinedId(std::uint32_t id)
{
    m_UserDefinedId = id;
}

void GffStruct::ConstructInternal(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff)
{
    m_UserDefinedId = rawStruct.m_Type;

    std::vector<Raw::GffField> fields;

    // Sometimes NWN (toolset?) produces ill-formed structures - whether a non-root structure with data offset as 0xFFFFFFFF
    // or an empty struct. This check guards against these cases.
    if (rawStruct.m_FieldCount && rawStruct.m_DataOrDataOffset != 0xFFFFFFFF)
    {
        if (rawStruct.m_FieldCount == 1)
        {
            fields.emplace_back(rawGff.m_Fields[rawStruct.m_DataOrDataOffset]);
        }
        else
        {
            std::uint32_t offsetIntoFieldIndexArray = rawStruct.m_DataOrDataOffset / sizeof(Raw::GffFieldIndex);
            ASSERT(offsetIntoFieldIndexArray < rawGff.m_FieldIndices.size());

            for (std::size_t i = 0; i < rawStruct.m_FieldCount; ++i)
            {
                std::uint32_t offsetIntoFieldArray = rawGff.m_FieldIndices[offsetIntoFieldIndexArray + i];
                ASSERT(offsetIntoFieldArray < rawGff.m_Fields.size());
                fields.emplace_back(rawGff.m_Fields[offsetIntoFieldArray]);
            }
        }
    }

    ConstructInternal(fields, rawGff);
}

void GffStruct::ConstructInternal(std::vector<Raw::GffField> const& rawFields, Raw::Gff const& rawGff)
{
    for (Raw::GffField const& rawField : rawFields)
    {
        char const* rawLabel = rawGff.m_Labels[rawField.m_LabelIndex].m_Label;
        std::string label = std::string(rawLabel, rawLabel + strnlen(rawLabel, 16));
        ASSERT(m_Fields.find(label) == std::end(m_Fields));

        switch (rawField.m_Type)
        {
            case Raw::GffField::Type::BYTE:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructBYTE(rawField)); break;
            case Raw::GffField::Type::CHAR:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCHAR(rawField)); break;
            case Raw::GffField::Type::WORD:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructWORD(rawField)); break;
            case Raw::GffField::Type::SHORT:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructSHORT(rawField)); break;
            case Raw::GffField::Type::DWORD:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDWORD(rawField)); break;
            case Raw::GffField::Type::INT:           m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructINT(rawField)); break;
            case Raw::GffField::Type::DWORD64:       m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDWORD64(rawField)); break;
            case Raw::GffField::Type::INT64:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructINT64(rawField)); break;
            case Raw::GffField::Type::FLOAT:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructFLOAT(rawField)); break;
            case Raw::GffField::Type::DOUBLE:        m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDOUBLE(rawField)); break;
            case Raw::GffField::Type::CExoString:    m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCExoString(rawField)); break;
            case Raw::GffField::Type::ResRef:        m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructResRef(rawField)); break;
            case Raw::GffField::Type::CExoLocString: m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCExoLocString(rawField)); break;
            case Raw::GffField::Type::VOID:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructVOID(rawField)); break;
            case Raw::GffField::Type::Struct:        m_Fields[label] = std::make_pair(rawField.m_Type, GffStruct(rawField, rawGff)); break;
            case Raw::GffField::Type::List:          m_Fields[label] = std::make_pair(rawField.m_Type, GffList(rawField, rawGff)); break;
            default: ASSERT_FAIL_MSG("Unrecognised GFF field type: %d", rawField.m_Type); break;
        }
    }
}

GffList::GffList(Raw::GffField const& rawField, Raw::Gff const& rawGff)
{
    ASSERT(rawField.m_Type == Raw::GffField::Type::List);

    Raw::GffField::Type_List list = rawGff.ConstructList(rawField);

    for (std::uint32_t offsetIntoStructArray : list.m_Elements)
    {
        ASSERT(offsetIntoStructArray < rawGff.m_Structs.size());
        m_Structs.emplace_back(rawGff.m_Structs[offsetIntoStructArray], rawGff);
    }
}

std::vector<GffStruct>& GffList::GetStructs()
{
    return m_Structs;
}

std::vector<GffStruct> const& GffList::GetStructs() const
{
    return m_Structs;
}

Gff::Gff() : m_TopLevelStruct()
{ }

Gff::Gff(Raw::Gff const& rawGff) : m_TopLevelStruct(rawGff.m_Structs[0], rawGff)
{ }

GffStruct& Gff::GetTopLevelStruct()
{
    return m_TopLevelStruct;
}

GffStruct const& Gff::GetTopLevelStruct() const
{
    return m_TopLevelStruct;
}

struct GffCreator
{
public:
    std::unique_ptr<Raw::Gff> Create(const GffStruct& topLevelStruct);

private:
    template <typename T>
    Raw::GffField CreateField(const GffStruct& parent,
        const GffStruct::FieldMap::value_type& kvp);

    std::uint32_t CreateLabel(const std::string& str);

    // Each of these returns an index into the relevant array (either struct or field).
    Raw::GffField InsertIntoRawGff(const Type_BYTE& type);
    Raw::GffField InsertIntoRawGff(const Type_CHAR& type);
    Raw::GffField InsertIntoRawGff(const Type_WORD& type);
    Raw::GffField InsertIntoRawGff(const Type_SHORT& type);
    Raw::GffField InsertIntoRawGff(const Type_DWORD& type);
    Raw::GffField InsertIntoRawGff(const Type_INT& type);
    Raw::GffField InsertIntoRawGff(const Type_DWORD64& type);
    Raw::GffField InsertIntoRawGff(const Type_INT64& type);
    Raw::GffField InsertIntoRawGff(const Type_FLOAT& type);
    Raw::GffField InsertIntoRawGff(const Type_DOUBLE& type);
    Raw::GffField InsertIntoRawGff(const Type_CExoString& type);
    Raw::GffField InsertIntoRawGff(const Type_CResRef& type);
    Raw::GffField InsertIntoRawGff(const Type_CExoLocString& type);
    Raw::GffField InsertIntoRawGff(const Type_VOID& type);
    Raw::GffField InsertIntoRawGff(const Type_Struct& type);
    Raw::GffField InsertIntoRawGff(const Type_List& type);

private:
    std::unique_ptr<Raw::Gff> m_RawGff;
    std::unordered_map<std::string, std::uint32_t> m_LabelCache;
};

bool Gff::WriteToFile(char const* path) const
{
    return GffCreator().Create(m_TopLevelStruct)->WriteToFile(path);
}

std::unique_ptr<Raw::Gff> GffCreator::Create(const GffStruct& topLevelStruct)
{
    m_RawGff = std::make_unique<Raw::Gff>();
    InsertIntoRawGff(topLevelStruct);

    Raw::GffHeader* header = &m_RawGff->m_Header;
    std::memcpy(header->m_FileType, "UTC ", 4);
    std::memcpy(header->m_FileVersion, "V3.2", 4);

    header->m_StructOffset = sizeof(Raw::GffHeader);
    header->m_StructCount = static_cast<std::uint32_t>(m_RawGff->m_Structs.size());

    header->m_FieldOffset = header->m_StructOffset + (header->m_StructCount * sizeof(Raw::GffStruct));
    header->m_FieldCount = static_cast<std::uint32_t>(m_RawGff->m_Fields.size());

    header->m_LabelOffset = header->m_FieldOffset + (header->m_FieldCount * sizeof(Raw::GffField));
    header->m_LabelCount = static_cast<std::uint32_t>(m_RawGff->m_Labels.size());

    header->m_FieldDataOffset = header->m_LabelOffset + (header->m_LabelCount * sizeof(Raw::GffLabel));
    header->m_FieldDataCount = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());

    header->m_FieldIndicesOffset = header->m_FieldDataOffset + header->m_FieldDataCount;
    header->m_FieldIndicesCount = static_cast<std::uint32_t>(m_RawGff->m_FieldIndices.size()) * 4;

    header->m_ListIndicesOffset = header->m_FieldIndicesOffset + header->m_FieldIndicesCount;
    header->m_ListIndicesCount = static_cast<std::uint32_t>(m_RawGff->m_ListIndices.size());

    return std::move(m_RawGff);
}

template <typename T>
Raw::GffField GffCreator::CreateField(const GffStruct& parent,
    const GffStruct::FieldMap::value_type& kvp)
{
    T data;
    bool read = parent.ReadField(kvp, &data);
    ASSERT(read);

    std::uint32_t label = CreateLabel(kvp.first);
    Raw::GffField field = InsertIntoRawGff(data);
    field.m_LabelIndex = label;
    field.m_Type = GetTypeFromType(data);
    return field;
}

std::uint32_t GffCreator::CreateLabel(const std::string& str)
{
    auto iter = m_LabelCache.find(str);
    if (iter == std::end(m_LabelCache))
    {
        iter = m_LabelCache.insert(std::make_pair(str, static_cast<std::uint32_t>(m_RawGff->m_Labels.size()))).first;
        Raw::GffLabel label = {};
        std::memcpy(&label.m_Label, str.c_str(), strnlen(str.c_str(), 16));
        m_RawGff->m_Labels.emplace_back(std::move(label));
    }

    return iter->second;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_BYTE& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_CHAR& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_WORD& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_SHORT& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_DWORD& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_INT& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_DWORD64& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(type));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_INT64& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(type));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_FLOAT& type)
{
    Raw::GffField field = {};
    std::memcpy(&field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_DOUBLE& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(type));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &type, sizeof(type));
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_CExoString& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());

    std::uint32_t stringSize = static_cast<std::uint32_t>(type.m_String.size());
    const char* string = type.m_String.c_str();
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(stringSize) + stringSize);

    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &stringSize, sizeof(stringSize));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset + sizeof(stringSize), string, stringSize);

    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_CResRef& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(type.m_Size) + type.m_Size);

    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &type.m_Size, sizeof(type.m_Size));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset + sizeof(type.m_Size), &type.m_String, type.m_Size);

    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_CExoLocString& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());

    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(type.m_TotalSize) + type.m_TotalSize);

    std::byte* ptr = m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset;
    std::byte* ptrEnd = ptr + sizeof(type.m_TotalSize) + type.m_TotalSize;

    std::memcpy(ptr, &type.m_TotalSize, sizeof(type.m_TotalSize));
    ptr += sizeof(type.m_TotalSize);

    std::memcpy(ptr, &type.m_StringRef, sizeof(type.m_StringRef));
    ptr += sizeof(type.m_StringRef);

    std::uint32_t stringCount = static_cast<std::uint32_t>(type.m_SubStrings.size());
    std::memcpy(ptr, &stringCount, sizeof(stringCount));
    ptr += sizeof(stringCount);

    for (std::uint32_t i = 0; i < stringCount; ++i)
    {
        const Raw::GffField::Type_CExoLocString::SubString& substring = type.m_SubStrings[i];

        std::memcpy(ptr, &substring.m_StringID, sizeof(substring.m_StringID));
        ptr += sizeof(substring.m_StringID);

        std::uint32_t substringLength = static_cast<std::uint32_t>(substring.m_String.size());
        std::memcpy(ptr, &substringLength, sizeof(substringLength));
        ptr += sizeof(substringLength);

        std::memcpy(ptr, substring.m_String.c_str(), substringLength);
        ptr += substringLength;
    }

    ASSERT(ptr == ptrEnd);

    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_VOID& type)
{
    Raw::GffField field = {};
    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_FieldData.size());

    std::uint32_t size = static_cast<std::uint32_t>(type.m_Data.size());
    m_RawGff->m_FieldData.resize(field.m_DataOrDataOffset + sizeof(size) + size);

    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset, &size, sizeof(size));
    std::memcpy(m_RawGff->m_FieldData.data() + field.m_DataOrDataOffset + sizeof(size), type.m_Data.data(), size);

    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_Struct& type)
{
    std::size_t baseFieldIndex = m_RawGff->m_Fields.size();
    std::size_t newFields = type.GetFields().size();
    m_RawGff->m_Fields.resize(baseFieldIndex + newFields);

    std::uint32_t structIndex = static_cast<std::uint32_t>(m_RawGff->m_Structs.size());
    m_RawGff->m_Structs.resize(structIndex + 1);

    Raw::GffStruct rawStruct;
    rawStruct.m_Type = type.GetUserDefinedId();
    rawStruct.m_FieldCount = static_cast<std::uint32_t>(newFields);

    if (rawStruct.m_FieldCount > 1)
    {
        std::size_t baseFieldIndicesIndex = m_RawGff->m_FieldIndices.size();
        rawStruct.m_DataOrDataOffset = static_cast<std::uint32_t>(baseFieldIndicesIndex * sizeof(m_RawGff->m_FieldIndices[0]));
        m_RawGff->m_FieldIndices.resize(baseFieldIndicesIndex + rawStruct.m_FieldCount);

        for (std::size_t i = baseFieldIndex, index = baseFieldIndicesIndex;
            index < baseFieldIndicesIndex + rawStruct.m_FieldCount;
            ++i, ++index)
        {
            m_RawGff->m_FieldIndices[index] = static_cast<std::uint32_t>(i);
        }
    }
    else
    {
        // Point directly to the head of the field array.
        // In the case of no fields, this will wrap around to be 0xFFFFFFFF, which is expected.
        rawStruct.m_DataOrDataOffset = static_cast<std::uint32_t>(baseFieldIndex);
    }

    m_RawGff->m_Structs[structIndex] = std::move(rawStruct);

    std::size_t fieldProcessing = 0;
    for (auto& kvp : type.GetFields())
    {
        Raw::GffField::Type fieldType = kvp.second.first;
        Raw::GffField field;

        switch (kvp.second.first)
        {
            case Raw::GffField::Type::BYTE:          field = CreateField<Type_BYTE>(type, kvp); break;
            case Raw::GffField::Type::CHAR:          field = CreateField<Type_CHAR>(type, kvp); break;
            case Raw::GffField::Type::WORD:          field = CreateField<Type_WORD>(type, kvp); break;
            case Raw::GffField::Type::SHORT:         field = CreateField<Type_SHORT>(type, kvp); break;
            case Raw::GffField::Type::DWORD:         field = CreateField<Type_DWORD>(type, kvp); break;
            case Raw::GffField::Type::INT:           field = CreateField<Type_INT>(type, kvp); break;
            case Raw::GffField::Type::DWORD64:       field = CreateField<Type_DWORD64>(type, kvp); break;
            case Raw::GffField::Type::INT64:         field = CreateField<Type_INT64>(type, kvp); break;
            case Raw::GffField::Type::FLOAT:         field = CreateField<Type_FLOAT>(type, kvp); break;
            case Raw::GffField::Type::DOUBLE:        field = CreateField<Type_DOUBLE>(type, kvp); break;
            case Raw::GffField::Type::CExoString:    field = CreateField<Type_CExoString>(type, kvp); break;
            case Raw::GffField::Type::ResRef:        field = CreateField<Type_CResRef>(type, kvp); break;
            case Raw::GffField::Type::CExoLocString: field = CreateField<Type_CExoLocString>(type, kvp); break;
            case Raw::GffField::Type::VOID:          field = CreateField<Type_VOID>(type, kvp); break;
            case Raw::GffField::Type::Struct:        field = CreateField<Type_Struct>(type, kvp); break;
            case Raw::GffField::Type::List:          field = CreateField<Type_List>(type, kvp); break;
            default: ASSERT_FAIL_MSG("Unrecognised GFF field type: %d", fieldType); break;
        }

        m_RawGff->m_Fields[baseFieldIndex + fieldProcessing++] = field;
    }

    Raw::GffField field = {};
    field.m_DataOrDataOffset = structIndex;
    return field;
}

Raw::GffField GffCreator::InsertIntoRawGff(const Type_List& type)
{
    Raw::GffField field = {};

    std::vector<std::uint32_t> structsInserted;

    for (const GffStruct& friendlyStruct : type.GetStructs())
    {
        std::uint32_t structIndex = static_cast<std::uint32_t>(m_RawGff->m_Structs.size());
        structsInserted.emplace_back(structIndex);
        InsertIntoRawGff(friendlyStruct);
    }

    field.m_DataOrDataOffset = static_cast<std::uint32_t>(m_RawGff->m_ListIndices.size());

    std::uint32_t structsInsertedCount = static_cast<std::uint32_t>(structsInserted.size());
    std::uint32_t sizeByteCount = sizeof(structsInsertedCount);
    std::uint32_t indicesByteCount = structsInsertedCount * sizeof(structsInserted[0]);
    m_RawGff->m_ListIndices.resize(field.m_DataOrDataOffset + sizeByteCount + indicesByteCount);

    std::byte* ptr = m_RawGff->m_ListIndices.data() + field.m_DataOrDataOffset;

    std::memcpy(ptr, &structsInsertedCount, sizeByteCount);
    ptr += sizeByteCount;

    std::memcpy(ptr, structsInserted.data(), indicesByteCount);

    return field;
}



}
