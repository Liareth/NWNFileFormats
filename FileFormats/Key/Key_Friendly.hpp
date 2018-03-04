#pragma once

#include <string>
#include <unordered_map>

#include "FileFormats/Key/Key_Raw.hpp"

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

// This is a user friendly wrapper around the Key data.
class Key
{
public:
    Key(Raw::Key const& rawKey);

    std::vector<KeyBifReference> const& GetReferencedBifs() const;

    using KeyResourceMap = std::unordered_map<std::string, std::unordered_map<Resource::ResourceType, std::uint32_t>>;
    KeyResourceMap const& GetReferencedResources() const;

private:
    std::vector<KeyBifReference> m_ReferencedBifs;

    // This maps between resref -> { type -> resource ID }
    KeyResourceMap m_ReferencedResources;
};

}
