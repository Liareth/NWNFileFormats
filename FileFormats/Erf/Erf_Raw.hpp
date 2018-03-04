#pragma once

#include "FileFormats/Resource.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace FileFormats::Erf::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_ERF_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.
//
// Note that while these structures are the 'raw types', in some cases I have substituted the <size> <contents>
// pattern with std::vector<> or std::string and I have removed the extra size member. This is to avoid the
// manual memory management requirements, e.g. so we don't have to allocate and deallocate a char array ourselves.

struct ErfHeader
{
    char m_FileType[4]; // "ERF ", "MOD ", "SAV ", "HAK " as appropriate
    char m_Version[4]; // "V1.0"
    std::uint32_t m_LanguageCount; // number of strings in the Localized String Table
    std::uint32_t m_LocalizedStringSize; // total size (bytes) of Localized String Table
    std::uint32_t m_EntryCount; // number of files packed into the ERF
    std::uint32_t m_OffsetToLocalizedString; // from beginning of file, see figure above
    std::uint32_t m_OffsetToKeyList; // from beginning of file, see figure above
    std::uint32_t m_OffsetToResourceList; // from beginning of file, see figure above
    std::uint32_t m_BuildYear; // since 1900
    std::uint32_t m_BuildDay; // since January 1st
    std::uint32_t m_DescriptionStrRef; // strref for file description

    // The Reserved part of the ERF header allows for additional properties to be added to the file format later
    // while maintaining backward compatibility with older ERFs.
    std::byte m_Reserved[116];
};

struct ErfLocalisedString
{
    // The Localized String List is used to provide a description of the ERF. In .mod files, this is where the
    // module description is stored. For example, during the Load Module screen in NWN (a BioWare Aurora
    // Engine game), the module descriptions shown in the upper right corner are taken from the Localized String
    // List. The game obtains the current Language ID from dialog.tlk, and then displays the ERF String whose
    // LanguageID matches the dialog.tlk language ID.

    // The String List contains a series of ERF String elements one after another. Note that each element has
    // a variable size, encoded within the element itself. The LanguageCount from the ERF Header specifies
    // the number of String List Elements.

    // Each String List Element has the following structure:

    std::uint32_t m_LanguageId;
    //std::uint32_t m_StringSize;
    //char m_String[m_StringSize];
    std::string m_String;
};

struct ErfKey
{
    // The ERF Key List specifies the filename and filetype of all the files packed into the ERF.

    // The Key List consists of a series of Key structures one after another. Unlike the String List elements,
    // the Key List elements all have the same size. The EntryCount in the ERF header specifies the number
    // of Keys.

    // Each Key List Element has the following structure:

    char m_ResRef[16]; // Filename
    std::uint32_t m_ResId; // Resource ID, starts at 0 and increments
    Resource::ResourceType m_ResType; // File type
    std::byte m_Reserved[2];

    // The ResRef is the name of the file with no null terminator and in lower case. A ResRef can only
    // contain alphanumeric characters or underscores. It must have 1 to 16 characters, and if it contains less
    // than 16 characters, the remaining ones are nulls.

    // The ResID in the key structure is redundant, because the its possible to get the ResID for any ERF
    // Key by subtracting the OffsetToKeyList from its starting address and dividing by the size of a Key
    // List structure.

    // When a file is extracted from an ERF, the ResRef is the name of the file after it is extracted, and the
    // ResType specifies its file extension. For a list of ResTypes, see the section on ResTypes later in this
    // document.

};

struct ErfResource
{
    // The Resource List looks just like the Key list, except that it has Resource List elements instead of Key
    // List elements. The ERF header's EntryCount specifies the number of elements in both the Key List
    // and the Resource List, and there is a one-to-one correspondence between Keys and Resource List
    // elements

    // Each Resource List Element corresponds to a single file packed into the ERF. The Resource structure
    // specifies where the data for the file begins inside the ERF, and how many bytes of data there are.

    std::uint32_t m_OffsetToResource; // offset to file data from beginning of ERF
    std::uint32_t m_ResourceSize; // number of bytes
};

using ErfResourceData = std::byte;

struct Erf
{
    ErfHeader m_Header;
    std::vector<ErfLocalisedString> m_LocalisedStrings;
    std::vector<ErfKey> m_Keys;
    std::vector<ErfResource> m_Resources;
    std::vector<ErfResourceData> m_ResourceData;

    static bool ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Erf* out);

private:
    void ReadLocalisedStrings(std::byte const* data);
    void ReadKeys(std::byte const* data);
    void ReadResources(std::byte const* data);
    void ReadResourceData(std::byte const* data, std::size_t bytesCount);
};

}