#include "FileFormats/Key/Key_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <algorithm>

namespace FileFormats::Key::Friendly {

Key::Key(Raw::Key const& rawKey)
{
    // Get the referenced BIFs.
    for (Raw::KeyFile const& rawFile : rawKey.m_Files)
    {
        KeyBifReference reference;
        reference.m_Drives = rawFile.m_Drives;
        reference.m_FileSize = rawFile.m_FileSize;

        std::uint32_t offSetStartIntoFilenameTable = rawKey.m_Header.m_OffsetToFileTable + (rawKey.m_Header.m_BIFCount * sizeof(Raw::KeyFile)); // End of file table
        std::uint32_t offSetIntoFilenameTable = rawFile.m_FilenameOffset - offSetStartIntoFilenameTable;
        ASSERT(offSetStartIntoFilenameTable + offSetIntoFilenameTable + rawFile.m_FilenameSize <= rawKey.m_Header.m_OffsetToKeyTable);

        char const* ptr = rawKey.m_Filenames.data() + offSetIntoFilenameTable;
        reference.m_Path = std::string(ptr, ptr + rawFile.m_FilenameSize);

        m_ReferencedBifs.emplace_back(std::move(reference));
    }

    // Get the references resources.
    for (Raw::KeyEntry const& rawEntry : rawKey.m_Entries)
    {
        std::string resref = std::string(rawEntry.m_ResRef, rawEntry.m_ResRef + strnlen_s(rawEntry.m_ResRef, 16));

        // NWN is case insensitive and cases are mixed like crazy in the official modules.
        // We just do the conversion to lower here to simplify things.
        std::transform(std::begin(resref), std::end(resref), std::begin(resref), ::tolower);

        std::unordered_map<Resource::ResourceType, std::uint32_t>& bucket = m_ReferencedResources[resref];

        // Make sure we don't already have a resource sharing this resref of the same type.
        // This shouldn't be possible in a well-formed ERF.
        ASSERT(bucket.find(rawEntry.m_ResourceType) == std::end(bucket));

        bucket.insert(std::make_pair(rawEntry.m_ResourceType, rawEntry.m_ResID));
    }
}

std::vector<KeyBifReference> const& Key::GetReferencedBifs() const
{
    return m_ReferencedBifs;
}

Key::KeyResourceMap const& Key::GetReferencedResources() const
{
    return m_ReferencedResources;
}

}
