#include "FileFormats/Key/Key_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"

#include <cstring>

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    out.resize(count);
    std::memcpy(out.data(), bytesWithInitialOffset, count * sizeof(T));
}

}

namespace FileFormats::Key::Raw {

bool Key::ReadFromBytes(std::byte const* bytes, Key* out)
{
    ASSERT(bytes);
    ASSERT(out);
    return out->ConstructInternal(bytes);
}

bool Key::ReadFromByteVector(std::vector<std::byte>&& bytes, Key* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);
    return out->ConstructInternal(bytes.data());
}

bool Key::ReadFromFile(char const* path, Key* out)
{
    ASSERT(path);
    ASSERT(out);

    MemoryMappedFile memmap;
    bool loaded = MemoryMappedFile::MemoryMap(path, &memmap);

    if (!loaded)
    {
        return false;
    }

    return out->ConstructInternal(memmap.GetDataBlock().GetData());
}

bool Key::ConstructInternal(std::byte const* bytes)
{
    ASSERT(bytes);

    std::memcpy(&m_Header, bytes, sizeof(m_Header));

    if (std::memcmp(m_Header.m_FileType, "KEY ", 4) != 0 ||
        std::memcmp(m_Header.m_FileVersion, "V1  ", 4) != 0)
    {
        return false;
    }

    ReadFiles(bytes);
    ReadFilenames(bytes);
    ReadEntries(bytes);

    return true;
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
