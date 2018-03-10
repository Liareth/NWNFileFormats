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
    for (std::byte const* head = bytes; head < bytes + bytesCount;)
    {
        char const* curLine = reinterpret_cast<char const*>(head); // This points to the first char of this line.
        char const* nextLine = std::strstr(curLine, "\n"); // This points to the last char of this line.

        if (!nextLine)
        {
            // This is the last line, so let's set it up manually.
            nextLine = reinterpret_cast<char const*>(bytes + bytesCount);
        }

        // Increment the next line so it points to the first character of the next line itself.
        nextLine += 1;

        // Calculate the length - excluding line ending tokens.
        std::size_t curLineLen = nextLine - curLine;
        for (char const* probe = nextLine - 1; probe >= curLine; --probe)
        {
            if (*probe == '\r' || *probe == '\n')
            {
                --curLineLen;
            }
            else
            {
                break;
            }
        }

        // Now we have a line: we split up the line into tokens, with anything enclosed in quotes as a token,
        // and if not enclosed in quotes, anything separated by whitespace.
        TwoDALine line;

        if (m_Lines.size() >= 3)
        {
            // If we've already added some lines, we can reserve upfront to make this a little bit faster.
            line.m_Tokens.reserve(m_Lines[2].m_Tokens.size());
        }

        if (curLineLen)
        {
            // Start at the first character of this line and extract all the tokens from it.
            for (char const* tokenHead = curLine; tokenHead < nextLine;)
            {
                // Skip over any whitespace.
                while (*tokenHead == ' ')
                {
                    if (++tokenHead >= nextLine)
                    {
                        break; // Trailing whitespace - we've moved onto the next line. Bail.
                    }
                }

                char const* nextOpeningQuotes = std::strstr(tokenHead, "\"");
                char const* nextWhitespace = std::strstr(tokenHead, " ");

                bool finalColumnInRow = false;

                if (!nextWhitespace || /* Final column in final row */
                    nextWhitespace >= nextLine /* Final column in row */)
                {
                    nextWhitespace = nextWhitespace ? nextLine : nextLine - 1;
                    finalColumnInRow = true;

                    // We need to take steps back to avoid the line endings.
                    while (*(nextWhitespace - 1) == '\n' || *(nextWhitespace - 1) == '\r')
                    {
                        --nextWhitespace;
                        ASSERT(nextWhitespace > tokenHead);
                    }
                }

                const char* data;
                std::size_t len;

                bool quotes = false;

                if (nextOpeningQuotes && nextOpeningQuotes < nextWhitespace)
                {
                    // We're closer to a quotes than a whitespace, which means we're processing a string.
                    char const* nextClosingQuotes = std::strstr(nextOpeningQuotes + 1, "\"");
                    ASSERT(nextClosingQuotes && nextClosingQuotes < nextLine);
                    data = nextOpeningQuotes + 1;
                    len = nextClosingQuotes - nextOpeningQuotes - 1;
                    quotes = true;
                }
                else
                {
                    ASSERT(nextWhitespace <= nextLine);
                    data = tokenHead;
                    len = nextWhitespace - tokenHead;
                }

                ASSERT(data >= curLine);
                ASSERT(data < nextLine);

                line.m_Tokens.emplace_back(data, len);

                if (finalColumnInRow)
                {
                    break; // This is the final column - so bail this loop and move onto the next line.
                }

                tokenHead += len + (quotes ? 2 : 1); // If quote - skip the closing quote as well as moving on.
            }
        }

        m_Lines.emplace_back(std::move(line));
        head = reinterpret_cast<std::byte const*>(nextLine);
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
