#include "FileFormats/Gff/Gff_Friendly.hpp"

namespace FileFormats::Gff::Friendly {

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

void GffStruct::ConstructInternal(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff)
{
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
        char labelBuff[17];
        std::memcpy(labelBuff, rawGff.m_Labels[rawField.m_LabelIndex].m_Label, 16);
        labelBuff[16] = '\0';

        std::string const& label = std::string(labelBuff);
        ASSERT(m_Fields.find(label) == std::end(m_Fields));

        switch (rawField.m_Type)
        {
        case Raw::GffField::BYTE:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructBYTE(rawField)); break;
        case Raw::GffField::CHAR:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCHAR(rawField)); break;
        case Raw::GffField::WORD:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructWORD(rawField)); break;
        case Raw::GffField::SHORT:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructSHORT(rawField)); break;
        case Raw::GffField::DWORD:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDWORD(rawField)); break;
        case Raw::GffField::INT:           m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructINT(rawField)); break;
        case Raw::GffField::DWORD64:       m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDWORD64(rawField)); break;
        case Raw::GffField::INT64:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructINT64(rawField)); break;
        case Raw::GffField::FLOAT:         m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructFLOAT(rawField)); break;
        case Raw::GffField::DOUBLE:        m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructDOUBLE(rawField)); break;
        case Raw::GffField::CExoString:    m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCExoString(rawField)); break;
        case Raw::GffField::ResRef:        m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructResRef(rawField)); break;
        case Raw::GffField::CExoLocString: m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructCExoLocString(rawField)); break;
        case Raw::GffField::VOID:          m_Fields[label] = std::make_pair(rawField.m_Type, rawGff.ConstructVOID(rawField)); break;
        case Raw::GffField::Struct:        m_Fields[label] = std::make_pair(rawField.m_Type, GffStruct(rawField, rawGff)); break;
        case Raw::GffField::List:          m_Fields[label] = std::make_pair(rawField.m_Type, GffList(rawField, rawGff)); break;
        default: ASSERT_FAIL_MSG("Unrecognised GFF field type: %d", rawField.m_Type); break;
        }
    }
}

GffList::GffList(Raw::GffField const& rawField, Raw::Gff const& rawGff)
{
    ASSERT(rawField.m_Type == Raw::GffField::Type::List);

    Raw::GffField::Type_List list = rawGff.ConstructList(rawField);

    for (std::size_t i = 0; i < list.m_Elements.size(); ++i)
    {
        std::uint32_t offsetIntoStructArray = list.m_Elements[i];
        ASSERT(offsetIntoStructArray < rawGff.m_Structs.size());
        m_Structs.emplace_back(GffStruct(rawGff.m_Structs[offsetIntoStructArray], rawGff));
    }
}

std::vector<GffStruct> const& GffList::GetStructs() const
{
    return m_Structs;
}

Gff::Gff(Raw::Gff const& rawGff) : m_TopLevelStruct(rawGff.m_Structs[0], rawGff)
{
}

GffStruct const& Gff::GetTopLevelStruct() const
{
    return m_TopLevelStruct;
}

}