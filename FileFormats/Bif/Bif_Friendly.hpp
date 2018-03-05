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

    // Returns a non-owning pointer to the resource data.
    virtual std::byte const* GetData() = 0;

    // Returns the length of the resource data.
    virtual std::size_t GetDataLength() = 0;

    virtual ~BifResource() = 0 {};
};

// A resource that is loaded up front - all bytes.
struct BifFrontLoadedResource : public BifResource
{
    // The raw data associated with this resource.
    std::vector<std::byte> m_Data;

    virtual std::byte const* GetData() override { return m_Data.data(); }
    virtual std::size_t GetDataLength() override { return m_Data.size(); }
    virtual ~BifFrontLoadedResource() {};
};

// A resource that references the bytes from the underlying raw structure.
struct BifStreamedResource : public BifResource
{
    // The raw data associated with this resource.
    // This is a NON-OWNING pointer.
    std::byte const* m_Data;

    // And the length.
    std::size_t m_DataLength;

    virtual std::byte const* GetData() override { return m_Data; }
    virtual std::size_t GetDataLength() override { return m_DataLength; }
    virtual ~BifStreamedResource() {};
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

    using BifResourceMap = std::unordered_map<std::uint32_t, std::unique_ptr<BifResource>>;
    BifResourceMap const& GetResources() const;

private:
    std::optional<Raw::Bif> m_RawBif;

    void ConstructInternal(Raw::Bif const& rawBif);

    // This maps between ID -> { BifResource }
    BifResourceMap m_Resources;
};

}
