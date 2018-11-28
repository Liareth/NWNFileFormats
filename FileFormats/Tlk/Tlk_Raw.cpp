#include "FileFormats/Tlk/Tlk_Raw.hpp"
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

namespace FileFormats::Tlk::Raw {

bool Tlk::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Tlk* out)
{
    ASSERT(bytes);
    ASSERT(out);
    return out->ConstructInternal(bytes, bytesCount);
}

bool Tlk::ReadFromByteVector(std::vector<std::byte>&& bytes, Tlk* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);
    return out->ConstructInternal(bytes.data(), bytes.size());
}

bool Tlk::ReadFromFile(char const* path, Tlk* out)
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

bool Tlk::WriteToFile(char const* path)
{
    ASSERT(path);

    FILE* outFile = std::fopen(path, "wb");

    if (outFile)
    {
        std::fwrite(&m_Header, sizeof(m_Header), 1, outFile);
        std::fwrite(m_StringData.data(), sizeof(m_StringData[0]), m_StringData.size(), outFile);
        std::fwrite(m_StringEntries.data(), sizeof(m_StringEntries[0]), m_StringEntries.size(), outFile);
        std::fclose(outFile);
        return true;
    }

    return false;
}

bool Tlk::ConstructInternal(std::byte const* bytes, std::size_t bytesCount)
{
    ASSERT(bytes);

    std::memcpy(&m_Header, bytes, sizeof(m_Header));

    if (std::memcmp(m_Header.m_FileType, "TLK ", 4) != 0 ||
        std::memcmp(m_Header.m_FileVersion, "V3.0", 4) != 0)
    {
        return false;
    }

    ReadStringData(bytes);
    ReadStringEntries(bytes, bytesCount);

    return true;
}

void Tlk::ReadStringData(std::byte const* data)
{
    std::uint32_t offset = sizeof(m_Header);
    std::uint32_t count = m_Header.m_StringCount;
    ReadGenericOffsetable(data + offset, count, m_StringData);
}

void Tlk::ReadStringEntries(std::byte const* data, std::size_t bytesCount)
{
    std::uint32_t offset = m_Header.m_StringEntriesOffset;
    std::size_t count = bytesCount - offset;
    ReadGenericOffsetable(data + offset, count, m_StringEntries);
}

}
