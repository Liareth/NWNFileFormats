#include "FileFormats/2da/2da_Friendly.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::TwoDA::Friendly {

TwoDARow::TwoDARow(std::uint32_t rowId,
    std::vector<TwoDAEntry>&& data,
    std::unordered_map<std::string, std::size_t> const& columns)
    : m_RowId(rowId),
      m_ColumnNames(columns),
      m_Data(std::forward<std::vector<TwoDAEntry>>(data))
{
}

TwoDAEntry& TwoDARow::operator[](std::size_t column)
{
    ASSERT(column < m_Data.size());
    return m_Data[column];
}

TwoDAEntry& TwoDARow::operator[](std::string const& column)
{
    auto columnName = m_ColumnNames.find(column);
    ASSERT(columnName != std::end(m_ColumnNames));
    return m_Data[columnName->second];
}

TwoDAEntry const& TwoDARow::operator[](std::size_t column) const
{
    ASSERT(column < m_Data.size());
    return m_Data[column];
}

TwoDAEntry const& TwoDARow::operator[](std::string const& column) const
{
    auto columnName = m_ColumnNames.find(column);
    ASSERT(columnName != std::end(m_ColumnNames));
    return m_Data[columnName->second];
}

std::string const& TwoDARow::AsStr(std::size_t column) const
{
    return operator[](column).m_Data;
}

std::string const& TwoDARow::AsStr(std::string const& column) const
{
    return operator[](column).m_Data;
}

std::int32_t TwoDARow::AsInt(std::size_t column) const
{
    return atoi(operator[](column).m_Data.c_str());
}

std::int32_t TwoDARow::AsInt(std::string const& column) const
{
    return atoi(operator[](column).m_Data.c_str());
}

float TwoDARow::AsFloat(std::size_t column) const
{
    return static_cast<float>(atof(operator[](column).m_Data.c_str()));
}

float TwoDARow::AsFloat(std::string const& column) const
{
    return static_cast<float>(atof(operator[](column).m_Data.c_str()));
}

bool TwoDARow::IsEmpty(std::size_t column) const
{
    return operator[](column).m_IsEmpty;
}

bool TwoDARow::IsEmpty(std::string const& column) const
{
    return operator[](column).m_IsEmpty;
}

std::uint32_t TwoDARow::RowId() const
{
    return m_RowId;
}

TwoDARow::TwoDAEntries::iterator TwoDARow::begin()
{
    return std::begin(m_Data);
}

TwoDARow::TwoDAEntries::iterator TwoDARow::end()
{
    return std::end(m_Data);
}

TwoDARow::TwoDAEntries::const_iterator TwoDARow::begin() const
{
    return std::cbegin(m_Data);
}

TwoDARow::TwoDAEntries::const_iterator TwoDARow::end() const
{
    return std::cend(m_Data);
}

std::size_t TwoDARow::Size() const
{
    return m_Data.size();
}

TwoDA::TwoDA(Raw::TwoDA const& raw2da)
{
    // Line 1 we don't care about ...
    // Line 2 has default values. TODO
    // Line 3 has all the columns.
    ASSERT(raw2da.m_Lines.size() >= 3);

    std::size_t start_line = 1;
    // Find first nonempty row (it will contain column labels)
    for (std::size_t i = start_line; i < raw2da.m_Lines.size(); ++i)
    {
        if (raw2da.m_Lines[i].m_Tokens.size() > 0)
        {
            start_line = i;
            break;
        }
    }

    // Iterate over all of the column names and set up the map.
    for (std::size_t i = 0; i < raw2da.m_Lines[start_line].m_Tokens.size(); ++i)
    {
        std::string const& token = raw2da.m_Lines[start_line].m_Tokens[i];
        m_ColumnNames[token] = i;
    }

    // Iterate over all of the entries and set them up.
    for (std::size_t i = start_line + 1; i < raw2da.m_Lines.size(); ++i)
    {
        std::vector<TwoDAEntry> entries;
        std::vector<Raw::TwoDAToken> const& tokens = raw2da.m_Lines[i].m_Tokens;

        if (tokens.empty())
        {
            // Non-conforming row - but done a lot in the base game. Just skip it.
            continue;
        }

        // Some 2da files have more 1 empty rows, resulting in i=3 => row where
        // first token is column name
        std::uint32_t rowId;
        try
        {
            // We store the row ID - this isn't necessarily to be used by the user,
            // but could store funky stuff that we might want to access.
            rowId = std::stoul(tokens[0]);
        }
        catch (std::invalid_argument& e)
        {
            continue;
        }

        // Skip the first token (which is the row number) when setting this up.
        for (std::size_t j = 1; j < m_ColumnNames.size() + 1; ++j)
        {
            TwoDAEntry entry;

            if (j < tokens.size())
            {
                entry.m_IsEmpty = false;
                entry.m_Data = tokens[j];
            }
            else
            {
                entry.m_IsEmpty = true;
            }

            entries.emplace_back(std::move(entry));
        }

        m_Rows.emplace_back(rowId, std::move(entries), m_ColumnNames);
    }
}

std::string const& TwoDA::AsStr(std::size_t row, std::size_t column) const
{
    return m_Rows[row].AsStr(column);
}

std::string const& TwoDA::AsStr(std::size_t row, std::string const& column) const
{
    return m_Rows[row].AsStr(column);
}

std::int32_t TwoDA::AsInt(std::size_t row, std::size_t column) const
{
    return m_Rows[row].AsInt(column);
}

std::int32_t TwoDA::AsInt(std::size_t row, std::string const& column) const
{
    return m_Rows[row].AsInt(column);
}

float TwoDA::AsFloat(std::size_t row, std::size_t column) const
{
    return m_Rows[row].AsFloat(column);
}

float TwoDA::AsFloat(std::size_t row, std::string const& column) const
{
    return m_Rows[row].AsFloat(column);
}

TwoDARow& TwoDA::operator[](std::size_t row)
{
    std::size_t rowSize = m_Rows.size();

    if (row < rowSize)
    {
        return m_Rows[row];
    }

    std::size_t rowsToAdd = row - rowSize;
    for (std::size_t i = 0; i <= rowsToAdd; ++i)
    {
        std::vector<TwoDAEntry> entries;

        for (std::size_t j = 1; j < m_ColumnNames.size() + 1; ++j)
        {
            TwoDAEntry entry;
            entry.m_Data = "****";
            entry.m_IsEmpty = false;
            entries.emplace_back(std::move(entry));
        }

        m_Rows.emplace_back(static_cast<std::uint32_t>(rowSize + i), std::move(entries), m_ColumnNames);
    }

    return m_Rows[row];
}


TwoDA::TwoDARows::iterator TwoDA::begin()
{
    return std::begin(m_Rows);
}

TwoDA::TwoDARows::iterator TwoDA::end()
{
    return std::end(m_Rows);
}

TwoDA::TwoDARows::const_iterator TwoDA::begin() const
{
    return std::cbegin(m_Rows);
}

TwoDA::TwoDARows::const_iterator TwoDA::end() const
{
    return std::cend(m_Rows);
}

std::size_t TwoDA::Size() const
{
    return m_Rows.size();
}

std::unordered_map<std::string, std::size_t> const& TwoDA::GetColumnNames() const
{
    return m_ColumnNames;
}

bool TwoDA::WriteToFile(char const* path) const
{
    Raw::TwoDA rawTwoDA;

    Raw::TwoDALine firstLine;
    Raw::TwoDALine secondLine;
    firstLine.m_Tokens.emplace_back("2DA V2.0");
    rawTwoDA.m_Lines.emplace_back(std::move(firstLine));
    rawTwoDA.m_Lines.emplace_back(std::move(secondLine));

    std::vector<std::string> columnNames;

    // Convert column names from map to a flat vector.
    for (auto& kvp : m_ColumnNames)
    {
        if (columnNames.size() < kvp.second + 1)
        {
            columnNames.resize(kvp.second + 1);
        }
        columnNames[kvp.second] = kvp.first;
    }

    Raw::TwoDALine columns;

    for (const std::string& columnName : columnNames)
    {
        columns.m_Tokens.emplace_back(columnName);
    }

    rawTwoDA.m_Lines.emplace_back(std::move(columns));

    for (std::size_t rowId = 0; rowId < m_Rows.size(); ++rowId)
    {
        Raw::TwoDALine line;
        line.m_Tokens.emplace_back(std::to_string(rowId));

        for (const TwoDAEntry& entry : m_Rows[rowId])
        {
            if (!entry.m_IsEmpty)
            {
                line.m_Tokens.emplace_back(entry.m_Data);
            }
        }

        rawTwoDA.m_Lines.emplace_back(std::move(line));
    }

    return rawTwoDA.WriteToFile(path);
}

}
