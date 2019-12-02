#include "FileFormats/Erf/Erf_Raw.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile.hpp"
#include "Utility/RAIIWrapper.hpp"

#include <cstring>

namespace {

template <typename T>
void ReadGenericOffsetable(std::byte const* bytesWithInitialOffset, std::size_t count, std::vector<T>& out)
{
    out.resize(count);
    std::memcpy(out.data(), bytesWithInitialOffset, count * sizeof(T));
}

}

namespace FileFormats::Erf::Raw {

bool Erf::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Erf* out)
{
    ASSERT(bytes);
    ASSERT(bytesCount);
    ASSERT(out);

    if (!out->ConstructInternal(bytes))
    {
        return false;
    }

    std::uint32_t offset = out->m_Header.m_OffsetToResourceList + (sizeof(ErfResource) * out->m_Header.m_EntryCount); // End of resources block

    std::unique_ptr<OwningDataBlock> owningBlock = std::make_unique<OwningDataBlock>();
    ReadGenericOffsetable(bytes + offset, bytesCount - offset, owningBlock->m_Data);
    out->m_ResourceData = std::move(owningBlock);

    return true;
}

bool Erf::ReadFromByteVector(std::vector<std::byte>&& bytes, Erf* out)
{
    ASSERT(!bytes.empty());
    ASSERT(out);

    if (!out->ConstructInternal(bytes.data()))
    {
        return false;
    }

    std::uint32_t offset = out->m_Header.m_OffsetToResourceList + (sizeof(ErfResource) * out->m_Header.m_EntryCount); // End of resources block

    std::unique_ptr<NonOwningDataBlock> nonOwningBlock = std::make_unique<NonOwningDataBlock>();
    nonOwningBlock->m_Data = bytes.data() + offset;
    nonOwningBlock->m_DataLength = bytes.size() - offset;
    out->m_ResourceData = std::move(nonOwningBlock);

    using StorageType = std::vector<std::byte>;
    out->m_DataBlockStorage = std::make_unique<RAIIWrapper<StorageType>>(std::forward<StorageType>(bytes));

    return true;
}

bool Erf::ReadFromFile(char const* path, Erf* out)
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

    std::uint32_t offset = out->m_Header.m_OffsetToResourceList + (sizeof(ErfResource) * out->m_Header.m_EntryCount); // End of resources block

    std::unique_ptr<NonOwningDataBlock> nonOwningBlock = std::make_unique<NonOwningDataBlock>();
    nonOwningBlock->m_Data = memmapped.GetData() + offset;
    nonOwningBlock->m_DataLength = memmapped.GetDataLength() - offset;
    out->m_ResourceData = std::move(nonOwningBlock);

    out->m_DataBlockStorage = std::make_unique<RAIIWrapper<MemoryMappedFile>>(std::move(memmap));

    return true;
}

bool Erf::WriteToFile(char const* path) const
{
    ASSERT(path);

    FILE* outFile = std::fopen(path, "wb");

    if (outFile)
    {
        std::fwrite(&m_Header, sizeof(m_Header), 1, outFile);

        for (const ErfLocalisedString& str : m_LocalisedStrings)
        {
            std::fwrite(&str.m_LanguageId, sizeof(str.m_LanguageId), 1, outFile);
            uint32_t len = (std::uint32_t)str.m_String.size();
            std::fwrite(&len, sizeof(len), 1, outFile);
            std::fwrite(str.m_String.c_str(), len, 1, outFile);
        }

        std::fwrite(m_Keys.data(), m_Keys.size() * sizeof(ErfKey), 1, outFile);
        std::fwrite(m_Resources.data(), m_Resources.size() * sizeof(ErfResource), 1, outFile);
        std::fwrite(m_ResourceData->GetData(), m_ResourceData->GetDataLength(), 1, outFile);

        std::fclose(outFile);
        return true;
    }

    return false;
}

bool Erf::ConstructInternal(std::byte const* bytes)
{
    std::memcpy(&m_Header, bytes, sizeof(m_Header));

    if (std::memcmp(m_Header.m_Version, "V1.0", 4) != 0)
    {
        return false;
    }

    ReadLocalisedStrings(bytes);
    ReadKeys(bytes);
    ReadResources(bytes);

    ASSERT(m_Keys.size() == m_Resources.size());

    return true;
}

void Erf::ReadLocalisedStrings(std::byte const* data)
{
    std::byte const* ptr = data + m_Header.m_OffsetToLocalizedString;

    for (std::size_t i = 0; i < m_Header.m_LanguageCount; ++i)
    {
        ErfLocalisedString str;
        std::memcpy(&str.m_LanguageId, ptr, sizeof(str.m_LanguageId));
        ptr += sizeof(str.m_LanguageId);

        std::uint32_t strSize;
        std::memcpy(&strSize, ptr, sizeof(strSize));
        ptr += sizeof(strSize);

        str.m_String = std::string(reinterpret_cast<char const*>(ptr), strSize);

        m_LocalisedStrings.emplace_back(str);
    }
}

void Erf::ReadKeys(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_OffsetToKeyList;
    std::uint32_t count = m_Header.m_EntryCount;
    ReadGenericOffsetable(data + offset, count, m_Keys);
}

void Erf::ReadResources(std::byte const* data)
{
    std::uint32_t offset = m_Header.m_OffsetToResourceList;
    std::uint32_t count = m_Header.m_EntryCount;
    ReadGenericOffsetable(data + offset, count, m_Resources);
}

}
