#include "FileFormats/Erf/Erf_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <algorithm>
#include <cstring>

namespace FileFormats::Erf::Friendly {

Erf::Erf()
{ }

Erf::Erf(Raw::Erf const& rawErf)
{
    ConstructInternal(rawErf);
}

Erf::Erf(Raw::Erf&& rawErf) : m_RawErf(std::forward<Raw::Erf>(rawErf))
{
    ConstructInternal(m_RawErf.value());
}

std::vector<Raw::ErfLocalisedString>& Erf::GetDescriptions()
{
    return m_Descriptions;
}

std::vector<Raw::ErfLocalisedString> const& Erf::GetDescriptions() const
{
    return m_Descriptions;
}

std::vector<ErfResource>& Erf::GetResources()
{
    return m_Resources;
}

std::vector<ErfResource> const& Erf::GetResources() const
{
    return m_Resources;
}

char* Erf::GetFileType()
{
    return m_FileType;
}

const char* Erf::GetFileType() const
{
    return m_FileType;
}

bool Erf::WriteToFile(char const* path) const
{
    Raw::Erf raw_erf;

    std::memcpy(raw_erf.m_Header.m_FileType, m_FileType, 4);
    std::memcpy(raw_erf.m_Header.m_Version, "V1.0", 4);

    raw_erf.m_Header.m_LanguageCount = (std::uint32_t)m_Descriptions.size();
    raw_erf.m_Header.m_EntryCount = (std::uint32_t)m_Resources.size();

    raw_erf.m_Header.m_LocalizedStringSize = 0;
    for (const Raw::ErfLocalisedString& str : m_Descriptions)
    {
        raw_erf.m_Header.m_LocalizedStringSize += sizeof(str.m_LanguageId)
            + sizeof(std::uint32_t) // string size
            + (std::uint32_t)str.m_String.size(); // string
    }

    std::uint32_t offset_str_table = sizeof(raw_erf.m_Header);
    std::uint32_t offset_keys = offset_str_table + raw_erf.m_Header.m_LocalizedStringSize;
    std::uint32_t offset_resources = offset_keys + (raw_erf.m_Header.m_EntryCount * sizeof(Raw::ErfKey));
    std::uint32_t offset_to_res_data = offset_resources + (raw_erf.m_Header.m_EntryCount * sizeof(Raw::ErfResource));

    raw_erf.m_Header.m_OffsetToLocalizedString = offset_str_table;
    raw_erf.m_Header.m_OffsetToKeyList = offset_keys;
    raw_erf.m_Header.m_OffsetToResourceList = offset_resources;
    raw_erf.m_Header.m_BuildYear = 666;
    raw_erf.m_Header.m_BuildDay = 333;
    raw_erf.m_Header.m_DescriptionStrRef = ~0u;
    std::memset(raw_erf.m_Header.m_Reserved, 0, sizeof(raw_erf.m_Header.m_Reserved));

    raw_erf.m_LocalisedStrings = m_Descriptions;

    {
        uint32_t total_data_size = 0;

        for (const ErfResource& friendly_res : m_Resources)
        {
            total_data_size += (std::uint32_t)friendly_res.m_DataBlock->GetDataLength();
        }

        std::unique_ptr<OwningDataBlock> data = std::make_unique<OwningDataBlock>();
        data->m_Data.resize(total_data_size);
        raw_erf.m_ResourceData = std::move(data);
    }

    for (std::uint32_t i = 0, offset_in_res_data = 0; i < raw_erf.m_Header.m_EntryCount; ++i)
    {
        const ErfResource& friendly_res = m_Resources[i];

        Raw::ErfKey key;
        std::memset(key.m_ResRef, 0, sizeof(key.m_ResRef));
        std::memcpy(key.m_ResRef, friendly_res.m_ResRef.c_str(), friendly_res.m_ResRef.size());
        key.m_ResId = i;
        key.m_ResType = friendly_res.m_ResType;
        std::memset(key.m_Reserved, 0, sizeof(key.m_Reserved));
        raw_erf.m_Keys.emplace_back(std::move(key));

        Raw::ErfResource res;
        res.m_OffsetToResource = offset_to_res_data + offset_in_res_data;
        res.m_ResourceSize = (std::uint32_t)friendly_res.m_DataBlock->GetDataLength();
        raw_erf.m_Resources.emplace_back(std::move(res));

        std::memcpy((void*)(raw_erf.m_ResourceData->GetData() + offset_in_res_data), friendly_res.m_DataBlock->GetData(), res.m_ResourceSize);
        offset_in_res_data += res.m_ResourceSize;
    }

    return raw_erf.WriteToFile(path);
}

void Erf::ConstructInternal(Raw::Erf const& rawErf)
{
    // First - copy in the descriptions. This one is simple.
    m_Descriptions = rawErf.m_LocalisedStrings;

    ASSERT(rawErf.m_Resources.size() == rawErf.m_Header.m_EntryCount);
    ASSERT(rawErf.m_Keys.size() == rawErf.m_Header.m_EntryCount);

    // Second - iterate over every entry, then set them up in a user friendly way.
    for (std::size_t i = 0; i < rawErf.m_Header.m_EntryCount; ++i)
    {
        Raw::ErfKey const& rawKey = rawErf.m_Keys[i];
        Raw::ErfResource const& rawRes = rawErf.m_Resources[i];

        std::string resref = std::string(rawKey.m_ResRef, rawKey.m_ResRef + strnlen(rawKey.m_ResRef, 16));

        // NWN is case insensitive and cases are mixed like crazy in the official modules.
        // We just do the conversion to lower here to simplify things.
        std::transform(std::begin(resref), std::end(resref), std::begin(resref), ::tolower);

        ErfResource resource;
        resource.m_ResRef = std::move(resref);
        resource.m_ResType = rawKey.m_ResType;
        resource.m_ResourceId = rawKey.m_ResId;

        // Per the spec, the resourceID should match exactly the order that the resources are present in the resource block.
        // We assert here to ensure that is actually the case.
        ASSERT(resource.m_ResourceId == i);

        // This gives us the offset to the start of the resource data block.
        std::uint32_t offsetToEndOfResources = rawErf.m_Header.m_OffsetToResourceList + (sizeof(Raw::ErfResource) * rawErf.m_Header.m_EntryCount); // End of resources block
        std::size_t offsetIntoResourceData = rawRes.m_OffsetToResource - offsetToEndOfResources;
        ASSERT(offsetIntoResourceData < rawErf.m_ResourceData->GetDataLength());

        if (m_RawErf.has_value())
        {
            std::unique_ptr<NonOwningDataBlock> db = std::make_unique<NonOwningDataBlock>();
            db->m_Data = rawErf.m_ResourceData->GetData() + offsetIntoResourceData;
            db->m_DataLength = rawRes.m_ResourceSize;
            resource.m_DataBlock = std::move(db);
        }
        else
        {
            std::unique_ptr<OwningDataBlock> db = std::make_unique<OwningDataBlock>();
            db->m_Data.resize(rawRes.m_ResourceSize);
            std::memcpy(db->m_Data.data(), rawErf.m_ResourceData->GetData() + offsetIntoResourceData, rawRes.m_ResourceSize);
            resource.m_DataBlock = std::move(db);
        }

        m_Resources.emplace_back(std::move(resource));
    }
}

}
