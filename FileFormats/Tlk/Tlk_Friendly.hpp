#pragma once

#include "FileFormats/Tlk/Tlk_Raw.hpp"

#include <cstddef>
#include <map>
#include <optional>
#include <string>

namespace FileFormats::Tlk::Friendly {

using StrRef = std::uint32_t;

struct TlkEntry
{
    std::optional<std::string> m_String;
    std::optional<std::string> m_SoundResRef;
    std::optional<float> m_SoundLength;
};

class Tlk
{
public:
    Tlk();
    Tlk(Raw::Tlk const& rawKey);

    // We use a map rather than unordered_map here because it's more user friendly to iterate from 0 -> max.
    using TlkMapType = std::map<StrRef, TlkEntry>;

    // Returns the string associated with the strref, or empty string ("").
    std::string const& operator[](StrRef strref) const;

    TlkEntry* Get(StrRef strref) const;
    void Set(StrRef strref, TlkEntry value);

    std::uint32_t GetLanguageId() const;
    void SetLanguageId(std::uint32_t id);

    TlkMapType::const_iterator begin() const;
    TlkMapType::const_iterator end() const;

    bool WriteToFile(const char* path) const;

private:
    std::uint32_t m_LanguageId;
    TlkMapType m_TlkMap;
};

}
