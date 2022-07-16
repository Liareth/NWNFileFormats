#pragma once

#include <any>
#include <cstddef>
#include <map>
#include <memory>
#include <vector>

#include "FileFormats/Gff/Gff_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Gff::Friendly {

class GffStruct;
class GffList;

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
using Type_VOID = Raw::GffField::Type_VOID;
using Type_Struct = GffStruct;
using Type_List = GffList;

// Individual functions here rather than a map so users encounter a compile error if they
// try to write an invalid type when calling from a template.
Raw::GffField::Type GetTypeFromType(const Type_BYTE&);
Raw::GffField::Type GetTypeFromType(const Type_CHAR&);
Raw::GffField::Type GetTypeFromType(const Type_WORD&);
Raw::GffField::Type GetTypeFromType(const Type_SHORT&);
Raw::GffField::Type GetTypeFromType(const Type_DWORD&);
Raw::GffField::Type GetTypeFromType(const Type_INT&);
Raw::GffField::Type GetTypeFromType(const Type_DWORD64&);
Raw::GffField::Type GetTypeFromType(const Type_INT64&);
Raw::GffField::Type GetTypeFromType(const Type_FLOAT&);
Raw::GffField::Type GetTypeFromType(const Type_DOUBLE&);
Raw::GffField::Type GetTypeFromType(const Type_CExoString&);
Raw::GffField::Type GetTypeFromType(const Type_CResRef&);
Raw::GffField::Type GetTypeFromType(const Type_CExoLocString&);
Raw::GffField::Type GetTypeFromType(const Type_VOID&);
Raw::GffField::Type GetTypeFromType(const Type_Struct&);
Raw::GffField::Type GetTypeFromType(const Type_List&);

class GffStruct
{
public:
    GffStruct() = default;

    // This will construct a struct from one struct directly.
    GffStruct(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff);

    // This will construct a struct from one field in the gff - assuming the field is of type struct (e.g. its own entry).
    GffStruct(Raw::GffField const& rawField, Raw::Gff const& rawGff);

    // The field map maps between std::string (field name) -> { Type (gff Type), std::any (type safe variant) }
    using FieldMap = std::map<std::string, std::pair<Raw::GffField::Type, std::any>>;

    // We expose direct access to the map here. This allows users to iterate over all fields if they need to do so.
    FieldMap const& GetFields() const;

    // The mapping of raw types to return values from ReadField matches the defines in Friendly::Type_*.
    // This looks up the field in m_Fields and extracts the actual type.
    template <typename T>
    bool ReadField(std::string const& fieldName, T* out) const;

    // Similar to above, except using the iterator.
    template <typename T>
    static bool ReadField(typename FieldMap::const_iterator iter, T* out);

    // Similiar to above, except using the kvp pair.
    template <typename T>
    static bool ReadField(typename FieldMap::value_type const& kvp, T* out);

    // The mapping of raw types to GFF types matches the defines in Friendly::Type_*.
    // This writes into the field the provided value, overwriting it if it already exists.
    template <typename T>
    void WriteField(std::string const& fieldName, T field);

    // Deletes the field if it exists. Does nothing if it does not.
    // Returns whether the field was deleted.
    bool DeleteField(std::string const& fieldName);

    std::uint32_t GetUserDefinedId() const;
    void SetUserDefinedId(std::uint32_t id);

private:
    void ConstructInternal(Raw::GffStruct const& rawStruct, Raw::Gff const& rawGff);
    void ConstructInternal(std::vector<Raw::GffField> const& rawFields, Raw::Gff const& rawGff);

    // We map between field name -> variant here.
    FieldMap m_Fields;

    std::uint32_t m_UserDefinedId;
};

template <typename T>
bool GffStruct::ReadField(std::string const& fieldName, T* out) const
{
    ASSERT(out);

    auto entry = m_Fields.find(fieldName);
    if (entry != std::end(m_Fields))
    {
        return ReadField(entry, out);
    }

    return false;
}

template <typename T>
bool GffStruct::ReadField(typename GffStruct::FieldMap::const_iterator iter, T* out)
{
    ASSERT(out);
    return ReadField(*iter, out);
}

template <typename T>
bool GffStruct::ReadField(typename GffStruct::FieldMap::value_type const& kvp, T* out)
{
    ASSERT(out);

    std::string const& fieldName = kvp.first;
    Raw::GffField::Type type = kvp.second.first;
    std::any const& variant = kvp.second.second;

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

template <typename T>
void GffStruct::WriteField(std::string const& fieldName, T field)
{
    m_Fields[fieldName] = std::make_pair(GetTypeFromType(field), std::move(field));
}

class GffList
{
public:
    GffList() = default;

    // Constructs a list from the field describing it.
    GffList(Raw::GffField const& rawField, Raw::Gff const& rawGff);

    std::vector<GffStruct>& GetStructs();
    std::vector<GffStruct> const& GetStructs() const;

private:
    // Note that these are copies of the structs, rather than references.
    std::vector<GffStruct> m_Structs;
};

// This is a user friendly wrapper around the Gff data.
class Gff
{
public:
    Gff();
    Gff(Raw::Gff const& rawGff);

    GffStruct& GetTopLevelStruct();
    GffStruct const& GetTopLevelStruct() const;

    char* GetFileType();
    const char* GetFileType() const;

    bool WriteToFile(char const* path) const;
    size_t WriteToBytes(std::byte* bytes, size_t max_len) const;

private:
    GffStruct m_TopLevelStruct;
    char m_FileType[4];
};

}
