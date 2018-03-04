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

        std::string resref = std::string(rawKey.m_ResRef, rawKey.m_ResRef + strnlen_s(rawKey.m_ResRef, 16));

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

        // This gives us an offset into the vector of raw data.
        std::size_t offsetIntoResourceData = rawRes.m_OffsetToResource - offsetToEndOfResources;
        ASSERT(offsetIntoResourceData < rawErf.m_ResourceData.size());

        resource.m_Data.resize(rawRes.m_ResourceSize);
        std::memcpy(resource.m_Data.data(), rawErf.m_ResourceData.data() + offsetIntoResourceData, rawRes.m_ResourceSize);

        m_Resources.emplace_back(std::move(resource));
    }
}

std::vector<Raw::ErfLocalisedString> const& Erf::GetDescriptions() const
{
    return m_Descriptions;
}

std::vector<ErfResource> const& Erf::GetResources() const
{
    return m_Resources;
}

}
