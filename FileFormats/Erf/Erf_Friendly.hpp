#pragma once

#include "FileFormats/Erf/Erf_Raw.hpp"

namespace FileFormats::Erf::Friendly {

struct ErfResource
{
    // The file name of the resource.
    std::string m_ResRef;

    // The type of the resource.
    Resource::ResourceType m_ResType;

    // This is mostly redundant - but could be useful somewhere.
    std::uint32_t m_ResourceId;

    // The binary data.
    std::vector<std::byte> m_Data;
};

// This is a user friendly wrapper around the Erf data.
class Erf
{
public:
    Erf(Raw::Erf const& rawErf);

    std::vector<Raw::ErfLocalisedString> const& GetDescriptions() const;
    std::vector<ErfResource> const& GetResources() const;

private:
    // This is a vector of localised descriptions for the ERF resource.
    std::vector<Raw::ErfLocalisedString> m_Descriptions;

    // A vector of resources contained within this ERF.
    std::vector<ErfResource> m_Resources;
};

}
