#pragma once

#include "FileFormats/Key/Key_Raw.hpp"
#include <string>

namespace FileFormats::Key::Friendly {

struct KeyBifReference
{
    // When m_Drives == 0, this refers to the installation directory of the app.
    // When m_Drives != 0, this is implementation defined.
    std::uint16_t m_Drives;

    // The path (relative to the root of the drive above) of the BIF.
    std::string m_Path;

    // Total byte size of the BIF.
    std::uint32_t m_FileSize;
};

struct KeyBifReferencedResource
{
    // The file name of the resource.
    std::string m_ResRef;

    // The type of the resource.
    Resource::ResourceType m_ResType;

    // The unmodified ResID of the resource.
    std::uint32_t m_ResId;

    // The index into m_ReferencedBifs that this resource is inside.
    std::size_t m_ReferencedBifIndex;
};

// This is a user friendly wrapper around the Key data.
class Key
{
public:
    Key(Raw::Key const& rawKey);

    std::vector<KeyBifReference> const& GetReferencedBifs() const;
    std::vector<KeyBifReferencedResource> const& GetReferencedResources() const;

private:
    std::vector<KeyBifReference> m_ReferencedBifs;
    std::vector<KeyBifReferencedResource> m_ReferencedResources;
};

}
