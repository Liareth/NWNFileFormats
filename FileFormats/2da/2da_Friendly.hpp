#pragma once

#include "FileFormats/2da/2da_Raw.hpp"

#include <unordered_map>

namespace FileFormats::TwoDA::Friendly {

struct TwoDAEntry
{
    // The data contained in this entry.
    std::string m_Data;

    // This entry is an empty value.
    bool m_IsEmpty;
};

class TwoDARow
{
public:
    TwoDARow(std::uint32_t rowId,
        std::vector<TwoDAEntry>&& data,
        std::unordered_map<std::string, std::size_t> const& columns);

    // Operator[] returns the rows directly.
    TwoDAEntry const& operator[](std::size_t column) const;
    TwoDAEntry const& operator[](std::string const& column) const;

    // These functions can be used to extract the value as the specified type.
    std::string const& AsStr(std::size_t column) const;
    std::string const& AsStr(std::string const& column) const;

    std::int32_t AsInt(std::size_t column) const;
    std::int32_t AsInt(std::string const& column) const;

    float AsFloat(std::size_t column) const;
    float AsFloat(std::string const& column) const;

    std::uint32_t RowId() const;

    using TwoDAEntries = std::vector<TwoDAEntry>;
    TwoDAEntries::const_iterator begin() const;
    TwoDAEntries::const_iterator end() const;
    std::size_t size() const;

private:
    std::uint32_t m_RowId;
    std::unordered_map<std::string, std::size_t> const& m_ColumnNames;
    std::vector<TwoDAEntry> m_Data;
};

class TwoDA
{
public:
    TwoDA(Raw::TwoDA const& raw2da);

    // These functions can be used to extract the value as the specified type.
    std::string const& AsStr(std::size_t row, std::size_t column) const;
    std::string const& AsStr(std::size_t row, std::string const& column) const;

    std::int32_t AsInt(std::size_t row, std::size_t column) const;
    std::int32_t AsInt(std::size_t row, std::string const& column) const;

    float AsFloat(std::size_t row, std::size_t column) const;
    float AsFloat(std::size_t row, std::string const& column) const;

    // Operator[] returns the row directly.
    TwoDARow const& operator[](std::size_t i) const;

    // This is just a flat vector of rows.
    using TwoDARows = std::vector<TwoDARow>;
    TwoDARows::const_iterator begin() const;
    TwoDARows::const_iterator end() const;
    std::size_t size() const;

    // The column map is a map, where the index contains the name of the column.
    std::unordered_map<std::string, std::size_t> const& GetColumnNames() const;

private:
    TwoDARows m_Rows;
    std::unordered_map<std::string, std::size_t> m_ColumnNames;
};

}
