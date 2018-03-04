#pragma once

#include "FileFormats/Bif/Bif_Raw.hpp"
#include <unordered_map>

namespace FileFormats::Bif::Friendly {

struct BifResource
{
    // The resource type.
    Resource::ResourceType m_ResType;

    // The raw data associated with this resource.
    std::vector<std::byte> m_Data;
};

// This is a user friendly wrapper around the Bif data.
// NOTE: We ignore the fixed resource table as it is not implemented per the spec.
class Bif
{
public:
    Bif(Raw::Bif const& rawBif);

    using BifResourceMap = std::unordered_map<std::uint32_t, BifResource>;
    BifResourceMap const& GetResources() const;

private:
    // This maps between ID -> { BifResource }
    BifResourceMap m_Resources;
};

}
