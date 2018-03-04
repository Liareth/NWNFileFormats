#include "FileFormats/Bif/Bif_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Bif::Raw {

bool Bif::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Bif* out)
{
    ASSERT(bytes);
    ASSERT(bytesCount);
    ASSERT(out);

    std::memcpy(&out->m_Header, bytes, sizeof(out->m_Header));

    if (std::memcmp(out->m_Header.m_FileType, "BIFF", 4) != 0 ||
        std::memcmp(out->m_Header.m_Version, "V1  ", 4) != 0)
    {
        return false;
    }

    out->ReadVariableResourceTable(bytes);
    out->ReadFixedResourceTable(bytes);
    out->ReadDataBlock(bytes, bytesCount);

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

void Bif::ReadDataBlock(std::byte const* data, std::size_t bytesCount)
{
    std::uint32_t offset = m_Header.m_VariableTableOffset;
    offset += m_Header.m_VariableResourceCount * sizeof(BifVariableResource);
    offset += m_Header.m_FixedResourceCount * sizeof(BifFixedResource);

    std::size_t count = bytesCount - offset;
    ReadGenericOffsetable(data + offset, count, m_DataBlock);
}

}
