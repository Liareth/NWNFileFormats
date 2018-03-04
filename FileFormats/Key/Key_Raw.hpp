#pragma once

#include "FileFormats/Resource.hpp"

#include <cstdint>
#include <vector>

namespace FileFormats::Key::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_KeyBIF_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.

// A Key file is an index of all the resources contained within a set of BIF files. The key file contains
// information as to which BIFs it indexes for and what resources are contained in those BIFs.

struct KeyHeader
{
    char m_FileType[4]; // "KEY "
    char m_FileVersion[4]; // "V1  "
    std::uint32_t m_BIFCount; // Number of BIF files that this KEY file controls
    std::uint32_t m_KeyCount; // Number of Resources in all BIF files linked to this keyfile
    std::uint32_t m_OffsetToFileTable; // Byte offset of File Table from beginning of this file
    std::uint32_t m_OffsetToKeyTable; // Byte offset of Key Entry Table from beginning of this file
    std::uint32_t m_BuildYear; // Number of years since 1900
    std::uint32_t m_BuildDay; // Number of days since January 1
    std::byte m_Reserved[32]; // 32 bytes Reserved for future use
};

struct KeyFile
{
    // The File Table is a list of all the BIF files that are associated with the key file.
    // The number of elements in the File Table is equal to the BIFCount specified in the Header.
    // Each element in the File Table is a File Entry, and describes a single BIF file.

    // File size of the BIF.
    std::uint32_t m_FileSize;

    // Byte position of the BIF file's filename in this file. Points to a location in the FileName Table.
    std::uint32_t m_FilenameOffset;

    // Number of characters in the BIF's filename.
    std::uint16_t m_FilenameSize;

    // A number that represents which drives the BIF file is located in. Currently each bit represents a
    // drive letter. e.g., bit 0 = HD0, which is the directory where the application was installed.
    std::uint16_t m_Drives;
};

// The Filename Table lists the filenames of all the BIF files associated with the key file.
// Each File Entry in the File Table has a FilenameOffset that indexes into a Filename Entry in the
// Filename Table.

// Filename of the BIF as a non-terminated character string.
// This filename is relative to the the drive where the BIF is located (as specified in the Drives
// portion of the BIF File Entry).
// Each Filename must be unique e.g., data\2da.bif

using KeyFilename = char;

struct KeyEntry
{
    // NOTE: Do not directly memcpy this structure when serialising. It is safe to memcpy between two KeyEntries
    // but not when you wish to read or write directly into bytes.
    // There is extra padding which will mess up your sizes and cause issues.

    // The Key Table is a list of all the resources in all the BIFs associated with this key file.
    // The number of elements in the Key Table is equal to the KeyCount specified in the Header.
    // Each element in the Key Table is a Key Entry, and describes a single resource. A resource may be
    // a Variable Resource, or it may be a Fixed Resource (at this time, all resources are Variable).

    // The filename of the resource item without its extension.
    // The game uses this name to access the resource.
    // Each ResRef must be unique.
    char m_ResRef[16];

    // Resource Type of the Resource.
    Resource::ResourceType m_ResourceType;

    // A unique ID number. It is generated as follows:
    // Variable: ID = (x << 20) + y
    // Fixed: ID = (x << 20) + (y << 14)
    // x = [Index into File Table to specify a BIF]
    // y = [Index into Variable or Fixed Resource Table in BIF]
    // (<< means bit shift left)
    std::uint32_t m_ResID;
};

struct Key
{
    KeyHeader m_Header;
    std::vector<KeyFile> m_Files;
    std::vector<KeyFilename> m_Filenames;
    std::vector<KeyEntry> m_Entries;

    static bool ReadFromBytes(std::byte const* bytes, Key* out);

private:
    void ReadFiles(std::byte const* data);
    void ReadFilenames(std::byte const* data);
    void ReadEntries(std::byte const* data);
};

}
