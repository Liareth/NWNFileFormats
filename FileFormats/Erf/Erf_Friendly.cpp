#pragma once

#include "FileFormats/Erf/Erf_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <algorithm>

namespace FileFormats::Erf::Friendly {

// This is a user friendly wrapper around the Erf data.

Erf::Erf(Raw::Erf const& rawErf)
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

        // Construct the resref and convert it to lower-case.
        // NWN is case insensitive and cases are mixed like crazy in the official modules.
        // We just do the conversion to lower here to simplify things.
        std::string resref = std::string(rawKey.m_ResRef, 16);
        std::transform(std::begin(resref), std::end(resref), std::begin(resref), ::tolower);

        std::unordered_map<Resource::ResourceType, ErfResource>& bucket = m_Resources[resref];

        // Make sure we don't already have a resource sharing this resref of the same type.
        // This shouldn't be possible in a well-formed ERF.
        ASSERT(bucket.find(rawKey.m_ResType) == std::end(bucket));

        ErfResource resource;
        resource.m_ResourceId = rawKey.m_ResId;

        // This gives us the offset to the start of the resource data block.
        std::uint32_t offsetToEndOfResources = rawErf.m_Header.m_OffsetToResourceList + (sizeof(Raw::ErfResource) * rawErf.m_Header.m_EntryCount); // End of resources block

        // This gives us an offset into the vector of raw data.
        std::size_t offsetIntoResourceData = rawRes.m_OffsetToResource - offsetToEndOfResources;
        ASSERT(offsetIntoResourceData < rawErf.m_ResourceData.size());

        resource.m_Data.resize(rawRes.m_ResourceSize);
        std::memcpy(resource.m_Data.data(), rawErf.m_ResourceData.data() + offsetIntoResourceData, rawRes.m_ResourceSize);

        bucket.insert(std::make_pair(rawKey.m_ResType, std::move(resource)));
    }
}

std::vector<Raw::ErfLocalisedString> const& Erf::GetDescriptions() const
{
    return m_Descriptions;
}

Erf::ErfResourceMap const& Erf::GetResources() const
{
    return m_Resources;
}

}
