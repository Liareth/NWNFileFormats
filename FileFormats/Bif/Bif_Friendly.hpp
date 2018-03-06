#pragma once

#include "FileFormats/Bif/Bif_Raw.hpp"

#include <memory>
#include <optional>
#include <unordered_map>

namespace FileFormats::Bif::Friendly {

struct BifResource
{
    // The actual ID listed in the BIF.
    std::uint32_t m_ResId;

    // The resource type.
    Resource::ResourceType m_ResType;

    // The underlying data for this resource.
    std::unique_ptr<DataBlock> m_DataBlock;
};

// This is a user friendly wrapper around the Bif data.
// NOTE: We ignore the fixed resource table as it is not implemented per the spec.
class Bif
{
public:
    // This constructs a friendly BIF from a raw BIF.
    Bif(Raw::Bif const& rawBif);

    // This constructs a friendly BIF from a raw BIF whose ownership has been passed to us.
    // If ownership of the Bif is passed to us, we construct streamed resources, thus lowering
    // the memory usage significantly.
    Bif(Raw::Bif&& rawBif);

    using BifResourceMap = std::unordered_map<std::uint32_t, BifResource>;
    BifResourceMap const& GetResources() const;

private:
    std::optional<Raw::Bif> m_RawBif;

    void ConstructInternal(Raw::Bif const& rawBif);

    // This maps between ID -> { BifResource }
    BifResourceMap m_Resources;
};

}
