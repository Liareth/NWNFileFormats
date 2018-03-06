#pragma once

#include "FileFormats/Tlk/Tlk_Raw.hpp"

#include <cstddef>
#include <map>

namespace FileFormats::Tlk::Friendly {

using StrRef = std::uint32_t;

class Tlk
{
public:
    Tlk(Raw::Tlk const& rawKey);

    // We use a map rather than unordered_map here because it's more user friendly to iterate from 0 -> max.
    using TlkMapType = std::map<StrRef, std::string>;

    // Returns the string associated with the strref, or empty string ("").
    std::string const& operator[](StrRef strref);

    TlkMapType::const_iterator begin() const;
    TlkMapType::const_iterator end() const;

private:
    TlkMapType m_TlkMap;
};

}
