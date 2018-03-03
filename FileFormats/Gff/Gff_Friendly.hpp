#pragma once

#include <any>
#include <unordered_map>
#include <vector>

#include "FileFormats/Gff/Gff_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Gff::Friendly {

class GffStruct
{
public:
    GffStruct() = default;

    // This will construct a struct from one struct directly.
    GffStruct(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff);

    // This will construct a struct from one field in the gff - assuming the field is of type struct (e.g. its own entry).
    GffStruct(Raw::GffField const& rawField, Raw::Gff const& rawGff);

    // The field map maps between std::string (field name) -> { Type (gff Type), std::any (type safe variant) }
    using FieldMap = std::unordered_map<std::string, std::pair<Raw::GffField::Type, std::any>>;

    // We expose direct access to the map here. This allows users to iterate over all fields if they need to do so.
    FieldMap const& GetFields() const;

    // The mapping of raw types to return values from ReadField matches the defines in Friendly::Type_*.
    template <typename T>
    bool ReadField(std::string const& fieldName, T* out) const;

private:
    void ConstructInternal(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff);
    void ConstructInternal(std::vector<Raw::GffField> const& rawFields, Raw::Gff const& rawGff);

    // We map between field name -> variant here.
    FieldMap m_Fields;
};

template <typename T>
bool GffStruct::ReadField(std::string const& fieldName, T* out) const
{
    ASSERT(out);

    auto entry = m_Fields.find(fieldName);
    if (entry != std::end(m_Fields))
    {
        Raw::GffField::Type type = entry->second.first;
        std::any const& variant = entry->second.second;

        try
        {
            *out = std::any_cast<T>(variant);
            return true;
        }
        catch (std::bad_cast&)
        {
            ASSERT_FAIL_MSG("Failed to extract field name %s due to a type mismatch. The Gff type stored was %u.", fieldName.c_str(), type);
            return false;
        }
    }

    return false;
}

class GffList
{
public:
    GffList() = default;

    // Constructs a list from the field describing it.
    GffList(Raw::GffField const& rawField, Raw::Gff const& rawGff);

    std::vector<GffStruct> const& GetStructs() const;

private:
    // Note that these are copies of the structs, rather than references.
    std::vector<GffStruct> m_Structs;
};

using Type_BYTE = Raw::GffField::Type_BYTE;
using Type_CHAR = Raw::GffField::Type_CHAR;
using Type_WORD = Raw::GffField::Type_WORD;
using Type_SHORT = Raw::GffField::Type_SHORT;
using Type_DWORD = Raw::GffField::Type_DWORD;
using Type_INT = Raw::GffField::Type_INT;
using Type_DWORD64 = Raw::GffField::Type_DWORD64;
using Type_INT64 = Raw::GffField::Type_INT64;
using Type_FLOAT = Raw::GffField::Type_FLOAT;
using Type_DOUBLE = Raw::GffField::Type_DOUBLE;
using Type_CExoString = Raw::GffField::Type_CExoString;
using Type_CResRef = Raw::GffField::Type_CResRef;
using Type_CExoLocString = Raw::GffField::Type_CExoLocString;
using Type_Void = Raw::GffField::Type_Void;
using Type_Struct = GffStruct;
using Type_List = GffList;

// This is a user friendly wrapper around the Gff data.
class Gff
{
public:
    Gff(Raw::Gff const& rawGff);

    GffStruct const& GetTopLevelStruct() const;

private:
    GffStruct m_TopLevelStruct;
};

}
