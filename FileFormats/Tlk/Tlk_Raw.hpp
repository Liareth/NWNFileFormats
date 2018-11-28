#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace FileFormats::Tlk::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_TalkTable_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.

// The talk table file, called dialog.tlk (and dialogf.tlk, containing feminine strings for certain
// languages), contains all the strings that the game will display to the user and which therefore need to be
// translated. Keeping all user-visible strings in the talk table makes it easier to produce multiple language
// versions of the game, because all the other game data files (with the exception of voice-over sound
// files) can remain the same between all language versions of the game. Using the talk table also has the
// advantage of reducing the amount of disk space required to store the game, since text for only one
// language is included.

struct TlkHeader
{
    char m_FileType[4]; // "TLK "
    char m_FileVersion[4]; // "V3.0"
    std::uint32_t m_LanguageID; // Language ID. See Table 3.2.2
    std::uint32_t m_StringCount; // Number of strings in file
    std::uint32_t m_StringEntriesOffset; // Offset from start of file to the String Entry Table
};

struct TlkStringData
{
    // The String Data Table is a list of String Data Elements, each one describing a single string in the
    // dialog.tlk file.
    // The number of elements in the String Data Table is equal to the StringCount specified in the
    // Header of the file. Each element is packed one after another, immediately after the end of the file
    // header.
    // A StringRef is an index into the String Data Table, so StrRef 0 is the first element, StrRef 1 is the
    // second element, and so on.
    // The format of a String Data Element is given in Table 3.3.1

    enum StringFlags : std::uint32_t
    {
        // If flag is set, there is text specified in the file for this StrRef.
        // Use the OffsetToString and StringSize to determine what the text is.
        // If flag is unset, then this StrRef has no text. Return an empty string.
        TEXT_PRESENT      = 0x0001,

        // If flag is set, read the SoundResRef from the file.
        // If flag is unset, SoundResRef is an empty string
        SND_PRESENT       = 0x0002,

        // If flag is set, read the SoundLength from the file.
        // If flag is unset, SoundLength is 0.0 seconds.
        SNDLENGTH_PRESENT = 0x0004
    };

    StringFlags m_Flags; // Flags about this StrRef.
    char m_SoundResRef[16]; // ResRef of the wave file associated with this string Unused characters are nulls.
    std::uint32_t m_VolumeVariance; // not used
    std::uint32_t m_PitchVariance; // not used
    std::uint32_t m_OffsetToString; // Offset from StringEntriesOffset to the beginning of the m_StrRef's text.
    std::uint32_t m_StringSize; // Number of bytes in the string.
    float m_SoundLength; // Duration in seconds of the associated wave file
};

using TlkStringEntry = std::byte;

struct Tlk
{
    TlkHeader m_Header;
    std::vector<TlkStringData> m_StringData;
    std::vector<TlkStringEntry> m_StringEntries;

    // Constructs an Tlk from a non-owning pointer.
    static bool ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Tlk* out);

    // Constructs an Tlk from a vector of bytes which we have taken ownership of.
    static bool ReadFromByteVector(std::vector<std::byte>&& bytes, Tlk* out);

    // Constructs an Tlk from a file.
    static bool ReadFromFile(char const* path, Tlk* out);

    // Writes the raw Tlk to disk.
    bool WriteToFile(char const* path);

private:
    bool ConstructInternal(std::byte const* bytes, std::size_t bytesCount);
    void ReadStringData(std::byte const* data);
    void ReadStringEntries(std::byte const* data, std::size_t bytesCount);
};

}
