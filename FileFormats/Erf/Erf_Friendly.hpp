#pragma once

#include "FileFormats/Erf/Erf_Raw.hpp"

#include <optional>

namespace FileFormats::Erf::Friendly {

struct ErfResource
{
    // The file name of the resource.
    std::string m_ResRef;

    // The type of the resource.
    Resource::ResourceType m_ResType;

    // This is mostly redundant - but could be useful somewhere.
    std::uint32_t m_ResourceId;

    // The underlying data for this resource.
    std::unique_ptr<DataBlock> m_DataBlock;
};

// This is a user friendly wrapper around the Erf data.
class Erf
{
public:
    // This constructs a friendly Erf from a raw Erf.
    Erf(Raw::Erf const& rawBif);

    // This constructs a friendly Erf from a raw Erf whose ownership has been passed to us.
    // If ownership of the Erf is passed to us, we construct streamed resources, thus lowering
    // the memory usage significantly.
    Erf(Raw::Erf&& rawBif);

    std::vector<Raw::ErfLocalisedString> const& GetDescriptions() const;
    std::vector<ErfResource> const& GetResources() const;

private:
    std::optional<Raw::Erf> m_RawErf;

    void ConstructInternal(Raw::Erf const& rawErf);

    // This is a vector of localised descriptions for the ERF resource.
    std::vector<Raw::ErfLocalisedString> m_Descriptions;

    // A vector of resources contained within this ERF.
    std::vector<ErfResource> m_Resources;
};

}
