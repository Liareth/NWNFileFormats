#pragma once

#include "FileFormats/Erf/Erf_Raw.hpp"
#include <unordered_map>

namespace FileFormats::Erf::Friendly {

struct ErfResource
{
    std::uint32_t m_ResourceId; // This is mostly redundant - but could be useful somewhere.
    std::vector<std::byte> m_Data; // The binary data.
};

// This is a user friendly wrapper around the Erf data.
class Erf
{
public:
    Erf(Raw::Erf const& rawErf);

    std::vector<Raw::ErfLocalisedString> const& GetDescriptions() const;

    using ErfResourceMap = std::unordered_map<std::string, std::unordered_map<Resource::ResourceType, ErfResource>>;
    ErfResourceMap const& GetResources() const;

private:
    // This is a vector of localised descriptions for the ERF resource.
    std::vector<Raw::ErfLocalisedString> m_Descriptions;

    // This maps between resref -> { type -> resource }
    ErfResourceMap m_Resources;
};

}
