#include "FileFormats/Erf/Erf_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <algorithm>
#include <cstring>

namespace FileFormats::Erf::Friendly {

Erf::Erf(Raw::Erf const& rawErf)
{
    ConstructInternal(rawErf);
}

Erf::Erf(Raw::Erf&& rawErf) : m_RawErf(std::forward<Raw::Erf>(rawErf))
{
    ConstructInternal(m_RawErf.value());
}

std::vector<Raw::ErfLocalisedString> const& Erf::GetDescriptions() const
{
    return m_Descriptions;
}

std::vector<ErfResource> const& Erf::GetResources() const
{
    return m_Resources;
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
            db->m_Data = rawErf.m_ResourceData->GetData();
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
