#include "FileFormats/Bif/Bif_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"
#include "Utility/RAIIWrapper.hpp"

#include <cstring>

namespace FileFormats::Bif::Raw {

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    out.resize(count);
    std::memcpy(out.data(), bytesWithInitialOffset, count * sizeof(T));
}

}

bool Bif::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Bif* out)
{
    ASSERT(bytes);
    ASSERT(bytesCount);
    ASSERT(out);

    if (!out->ConstructInternal(bytes))
    {
        return false;
    }

    std::uint32_t offset = out->m_Header.m_VariableTableOffset;
    offset += out->m_Header.m_VariableResourceCount * sizeof(BifVariableResource);
    offset += out->m_Header.m_FixedResourceCount * sizeof(BifFixedResource);

    std::unique_ptr<OwningDataBlock> owningBlock = std::make_unique<OwningDataBlock>();
    ReadGenericOffsetable(bytes + offset, bytesCount - offset, owningBlock->m_Data);
    out->m_DataBlock = std::move(owningBlock);

    return true;
}

bool Bif::ReadFromByteVector(std::vector<std::byte>&& bytes, Bif* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);

    if (!out->ConstructInternal(bytes.data()))
    {
        return false;
    }

    std::uint32_t offset = out->m_Header.m_VariableTableOffset;
    offset += out->m_Header.m_VariableResourceCount * sizeof(BifVariableResource);
    offset += out->m_Header.m_FixedResourceCount * sizeof(BifFixedResource);

    std::unique_ptr<NonOwningDataBlock> nonOwningBlock = std::make_unique<NonOwningDataBlock>();
    nonOwningBlock->m_Data = bytes.data() + offset;
    nonOwningBlock->m_DataLength = bytes.size() - offset;
    out->m_DataBlock = std::move(nonOwningBlock);

    using StorageType = std::vector<std::byte>;
    out->m_DataBlockStorage = std::make_unique<RAIIWrapper<StorageType>>(std::forward<StorageType>(bytes));

    return true;
}

bool Bif::ReadFromFile(char const* path, Bif* out)
{
    ASSERT(path);
    ASSERT(out);

    MemoryMappedFile memmap;
    bool loaded = MemoryMappedFile::MemoryMap(path, &memmap);

    if (!loaded)
    {
        return false;
    }

    DataBlock const& memmapped = memmap.GetDataBlock();

    if (!out->ConstructInternal(memmapped.GetData()))
    {
        return false;
    }

    std::uint32_t offset = out->m_Header.m_VariableTableOffset;
    offset += out->m_Header.m_VariableResourceCount * sizeof(BifVariableResource);
    offset += out->m_Header.m_FixedResourceCount * sizeof(BifFixedResource);

    std::unique_ptr<NonOwningDataBlock> nonOwningBlock = std::make_unique<NonOwningDataBlock>();
    nonOwningBlock->m_Data = memmapped.GetData() + offset;
    nonOwningBlock->m_DataLength = memmapped.GetDataLength() - offset;
    out->m_DataBlock = std::move(nonOwningBlock);

    out->m_DataBlockStorage = std::make_unique<RAIIWrapper<MemoryMappedFile>>(std::move(memmap));

    return true;
}

bool Bif::ConstructInternal(std::byte const* bytes)
{
    ASSERT(bytes);

    std::memcpy(&m_Header, bytes, sizeof(m_Header));

    if (std::memcmp(m_Header.m_FileType, "BIFF", 4) != 0 ||
        std::memcmp(m_Header.m_Version, "V1  ", 4) != 0)
    {
        return false;
    }

    ReadVariableResourceTable(bytes);
    ReadFixedResourceTable(bytes);

    return true;
}

void Bif::ReadVariableResourceTable(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_VariableTableOffset;
    std::uint32_t count = m_Header.m_VariableResourceCount;
    ReadGenericOffsetable(data + offset, count, m_VariableResourceTable);
}

void Bif::ReadFixedResourceTable(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_VariableTableOffset;
    offset += m_Header.m_VariableResourceCount * sizeof(BifVariableResource);

    std::uint32_t count = m_Header.m_FixedResourceCount;
    ReadGenericOffsetable(data + offset, count, m_FixedResourceTable);
}

}
