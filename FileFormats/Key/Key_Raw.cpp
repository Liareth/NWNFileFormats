#include "FileFormats/Key/Key_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Key::Raw {

bool Key::ReadFromBytes(std::byte const* bytes, Key* out)
{
    ASSERT(bytes);
    ASSERT(out);

    std::memcpy(&out->m_Header, bytes, sizeof(out->m_Header));

    if (std::memcmp(out->m_Header.m_FileType, "KEY ", 4) != 0 ||
        std::memcmp(out->m_Header.m_FileVersion, "V1  ", 4) != 0)
    {
        return false;
    }

    out->ReadFiles(bytes);
    out->ReadFilenames(bytes);
    out->ReadEntries(bytes);

    return true;
}

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    out.resize(count);
    std::memcpy(out.data(), bytesWithInitialOffset, count * sizeof(T));
}

}

void Key::ReadFiles(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_OffsetToFileTable;
    std::uint32_t count = m_Header.m_BIFCount;
    ReadGenericOffsetable(data + offset, count, m_Files);
}

void Key::ReadFilenames(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_OffsetToFileTable + (m_Header.m_BIFCount * sizeof(KeyFile)); // End of file table
    std::uint32_t count = m_Header.m_OffsetToKeyTable - offset; // Between the file table and the key table.
    ReadGenericOffsetable(data + offset, count, m_Filenames);
}

void Key::ReadEntries(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_OffsetToKeyTable;
    std::uint32_t count = m_Header.m_KeyCount;

    data = data + offset;

    for (std::size_t i = 0; i < count; ++i)
    {
        // Because of structure padding we need to serialise this manually.
        // We could avoid this by packing the structure but there doesn't seem to be a modern C++ standard 
        // way of doing this and I don't want to whip out the pragma pack macros for this.
        // We can change this if this operation becomes too slow.

        KeyEntry entry;

        std::memcpy(&entry.m_ResRef, data, sizeof(entry.m_ResRef));
        data += sizeof(entry.m_ResRef);

        std::memcpy(&entry.m_ResourceType, data, sizeof(entry.m_ResourceType));
        data += sizeof(entry.m_ResourceType);

        std::memcpy(&entry.m_ResID, data, sizeof(entry.m_ResID));
        data += sizeof(entry.m_ResID);

        m_Entries.emplace_back(std::move(entry));
    }
}

}
