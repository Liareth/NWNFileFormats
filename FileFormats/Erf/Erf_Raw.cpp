#include "FileFormats/Erf/Erf_Raw.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Erf::Raw {

bool Erf::ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Erf* out)
{
    ASSERT(bytes);
    ASSERT(bytesCount);
    ASSERT(out);

    std::memcpy(&out->m_Header, bytes, sizeof(out->m_Header));

    if (std::memcmp(out->m_Header.m_Version, "V1.0", 4) != 0)
    {
        return false;
    }

    out->ReadLocalisedStrings(bytes);
    out->ReadKeys(bytes);
    out->ReadResources(bytes);
    out->ReadResourceData(bytes, bytesCount);

    ASSERT(out->m_Keys.size() == out->m_Resources.size());

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

void Erf::ReadResourceData(std::byte const* data, std::size_t bytesCount)
{
    std::uint32_t offset = m_Header.m_OffsetToResourceList + (sizeof(ErfResource) * m_Header.m_EntryCount); // End of resources block
    std::size_t count = bytesCount - offset; // The remaining bytes of the file.
    ReadGenericOffsetable(data + offset, count, m_ResourceData);
}

}
