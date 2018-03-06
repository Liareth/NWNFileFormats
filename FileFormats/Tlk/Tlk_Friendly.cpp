#include "FileFormats/Tlk/Tlk_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <cstring>
#include <string>

namespace FileFormats::Tlk::Friendly {

Tlk::Tlk(Raw::Tlk const& rawKey)
{
    for (std::size_t i = 0; i < rawKey.m_StringData.size(); ++i)
    {
        Raw::TlkStringData const& data = rawKey.m_StringData[i];

        std::string str;

        // It's not clear by the spec how these flags interact - I think the intent is that the game can query for
        // text, sound, or sound length separately using the exact same strref.
        // I'm just doing this for now. We may have to expand this later.

        if (data.m_Flags & Raw::TlkStringData::TEXT_PRESENT)
        {
            ASSERT(data.m_OffsetToString + data.m_StringSize <= rawKey.m_StringEntries.size());
            str = std::string(reinterpret_cast<char const*>(rawKey.m_StringEntries.data() + data.m_OffsetToString), data.m_StringSize);
        }
        else if (data.m_Flags & Raw::TlkStringData::SND_PRESENT)
        {
            str = std::string(data.m_SoundResRef, strnlen(data.m_SoundResRef, 16));
        }
        else
        {
            str = std::to_string(data.m_SoundLength);
        }

        m_TlkMap.insert(std::make_pair(static_cast<StrRef>(i), std::move(str)));
    }
}

std::string const& Tlk::operator[](StrRef strref)
{
    static const std::string s_EmptyString = "";
    auto entry = m_TlkMap.find(strref);
    return entry != std::end(m_TlkMap) ? entry->second : s_EmptyString;
}

Tlk::TlkMapType::const_iterator Tlk::begin() const
{
    return std::cbegin(m_TlkMap);
}

Tlk::TlkMapType::const_iterator Tlk::end() const
{
    return std::cend(m_TlkMap);
}

}
