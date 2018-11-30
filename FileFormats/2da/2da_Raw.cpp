#include "FileFormats/2da/2da_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"

#include <algorithm>

#include <cstring>

namespace FileFormats::TwoDA::Raw {

bool TwoDA::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, TwoDA* out)
{
    ASSERT(bytes);
    ASSERT(out);
    return out->ConstructInternal(bytes, bytesCount);
}

bool TwoDA::ReadFromByteVector(std::vector<std::byte>&& bytes, TwoDA* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);
    return out->ConstructInternal(bytes.data(), bytes.size());
}

bool TwoDA::ReadFromFile(char const* path, TwoDA* out)
{
    ASSERT(path);
    ASSERT(out);

    MemoryMappedFile memmap;
    bool loaded = MemoryMappedFile::MemoryMap(path, &memmap);

    if (!loaded)
    {
        return false;
    }

    return out->ConstructInternal(memmap.GetDataBlock().GetData(), memmap.GetDataBlock().GetDataLength());
}

bool TwoDA::WriteToFile(char const* path) const
{
    ASSERT(path);

    FILE* outFile = std::fopen(path, "wb");

    if (outFile)
    {
        // First and second lines - write them out manually.
        const char* twoDaVersion = "2DA V2.0\n\n";
        std::fwrite(twoDaVersion, std::strlen(twoDaVersion), 1, outFile);

        // For each column now, find the greatest width.
        std::vector<std::size_t> columnWidths;
        columnWidths.resize(m_Lines[2].m_Tokens.size() + 1);

        // The column line we handle with a special case, since it's missing row number.
        for (std::size_t i = 2; i < columnWidths.size(); ++i)
        {
            columnWidths[i] = m_Lines[2].m_Tokens[i - 1].size();
        }

        for (std::size_t i = 3; i < m_Lines.size(); ++i)
        {
            for (std::size_t j = 0; j < columnWidths.size(); ++j)
            {
                const TwoDALine& line = m_Lines[i];
                if (j < line.m_Tokens.size())
                {
                    const std::string& token = line.m_Tokens[j];
                    std::size_t tokenSize = token.size();
                    if (token.find(" ") != std::string::npos)
                    {
                        // Account for quotes
                        tokenSize += 2;
                    }
                    columnWidths[j] = std::max(columnWidths[j], tokenSize);
                }
            }
        }

        // Manually print the columns.
        for (std::size_t i = 0; i < columnWidths.size(); ++i)
        {
            const char* str = i == 0 ? "" : m_Lines[2].m_Tokens[i - 1].c_str();
            std::fprintf(outFile, "%-*s", static_cast<int>(columnWidths[i]), str);

            if (i != columnWidths.size() - 1)
            {
                std::fwrite(" ", 1, 1, outFile);
            }
        }

        std::fwrite("\n", 1, 1, outFile);

        // Iterate each row and write it.
        for (std::size_t i = 3; i < m_Lines.size(); ++i)
        {
            for (std::size_t j = 0; j < columnWidths.size(); ++j)
            {
                std::string str;

                const TwoDALine& line = m_Lines[i];
                if (j < line.m_Tokens.size())
                {
                    str = line.m_Tokens[j].c_str();
                    if (str.find(" ") != std::string::npos)
                    {
                        str = "\"" + str + "\"";
                    }
                }

                std::fprintf(outFile, "%-*s", static_cast<int>(columnWidths[j]), str.c_str());
                if (j != columnWidths.size() - 1)
                {
                    std::fwrite(" ", 1, 1, outFile);
                }
            }

            if (i != m_Lines.size())
            {
                std::fwrite("\n", 1, 1, outFile);
            }
        }

        std::fclose(outFile);
        return true;
    }

    return false;
}

bool TwoDA::ConstructInternal(std::byte const* bytes, std::size_t bytesCount)
{
    // This code is slower than it should be, and allocates.
    // It is possible to do all the parsing in-place but it is difficult to handle the edge cases
    // where the spec has been violated (e.g. wrong line endings, tab characters, pointless trailing white space).

    std::vector<char> flattenedLines;
    flattenedLines.resize(bytesCount);
    std::memcpy(flattenedLines.data(), bytes, bytesCount);

    std::vector<std::string> lines;

    // First pass - split into lines.

    for (const char* head = flattenedLines.data(),
        *end = flattenedLines.data() + flattenedLines.size(),
        *tail = head; head < end;)
    {
        while (*head++ != '\n' && head < end) {}
        lines.emplace_back(std::string(tail, head - tail));
        tail = head++;
    }

    flattenedLines.clear();

    // Second pass -
    // 1. Convert tabs to spaces
    // 2. Remove /r
    // 3. Remove /n

    for (std::string& line : lines)
    {
        for (auto iter = std::rbegin(line); iter != std::rend(line); ++iter)
        {
            char ch = *iter;

            if (ch == '\n' || ch == '\r')
            {
                line.erase(std::next(iter).base());
            }
            else if (ch == '\t')
            {
                *iter = ' ';
            }
        }
    }

    // Third pass - now that we have lines set up, we can tokenize them.
    // Anything surrounded by quotes is allowed whitespace, otherwise whitespace is the delimiter.

    for (const std::string& line : lines)
    {
        TwoDALine twoDALine;

        for (auto head = std::cbegin(line),
            end = std::cend(line), tail = head;
            head != end;)
        {
            bool doingQuotes = false;

            while (head != end)
            {
                char ch = *head++;

                if (ch == '"')
                {
                    doingQuotes = !doingQuotes;
                }

                if (!doingQuotes && ch == ' ')
                {
                    break;
                }
            }

            if (*tail != ' ')
            {
                auto finalTail = tail;
                auto finalHead = head;

                if (head != end)
                {
                    finalHead -= 1;
                }

                if (*finalTail == '"')
                {
                    finalTail += 1;
                    finalHead -= 1;
                }

                twoDALine.m_Tokens.emplace_back(std::string(finalTail, finalHead));
            }

            tail = head;
        }

        m_Lines.emplace_back(std::move(twoDALine));
    }

    ASSERT(m_Lines.size() >= 3);

    ASSERT(m_Lines[0].m_Tokens.size() == 2);
    ASSERT(m_Lines[0].m_Tokens[0].size() == 3);
    ASSERT(m_Lines[0].m_Tokens[1].size() == 4);

    if (std::memcmp(m_Lines[0].m_Tokens[0].c_str(), "2DA", 3) != 0 ||
        std::memcmp(m_Lines[0].m_Tokens[1].c_str(), "V2.0", 4) != 0)
    {
        return false;
    }

    return true;
}

}
