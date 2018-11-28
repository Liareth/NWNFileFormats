#include "FileFormats/2da/2da_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"

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
