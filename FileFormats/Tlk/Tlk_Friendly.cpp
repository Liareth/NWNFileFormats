#include "FileFormats/Tlk/Tlk_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <cstring>
#include <string>

namespace FileFormats::Tlk::Friendly {

Tlk::Tlk()
{ }

Tlk::Tlk(Raw::Tlk const& rawTlk)
{
    m_LanguageId = rawTlk.m_Header.m_LanguageID;

    for (std::size_t i = 0; i < rawTlk.m_StringData.size(); ++i)
    {
        Raw::TlkStringData const& data = rawTlk.m_StringData[i];

        TlkEntry tlkEntry;

        if (data.m_Flags & Raw::TlkStringData::TEXT_PRESENT)
        {
            ASSERT(data.m_OffsetToString + data.m_StringSize <= rawTlk.m_StringEntries.size());
            tlkEntry.m_String = std::string(reinterpret_cast<char const*>(rawTlk.m_StringEntries.data() + data.m_OffsetToString), data.m_StringSize);
        }

        if (data.m_Flags & Raw::TlkStringData::SND_PRESENT)
        {
            tlkEntry.m_SoundResRef = std::string(data.m_SoundResRef, strnlen(data.m_SoundResRef, 16));
        }

        if (data.m_Flags & Raw::TlkStringData::SNDLENGTH_PRESENT)
        {
            tlkEntry.m_SoundLength = data.m_SoundLength;
        }

        m_TlkMap.insert(std::make_pair(static_cast<StrRef>(i), std::move(tlkEntry)));
    }
}

std::string const& Tlk::operator[](StrRef strref) const
{
    static const std::string s_EmptyString = "";
    auto entry = m_TlkMap.find(strref);

    if (entry == std::end(m_TlkMap) || !entry->second.m_String.has_value())
    {
        return s_EmptyString;
    }

    return entry->second.m_String.value();
}

void Tlk::Set(StrRef strref, TlkEntry value)
{
    m_TlkMap[strref] = value;
}

std::uint32_t Tlk::GetLanguageId() const
{
    return m_LanguageId;
}

void Tlk::SetLanguageId(std::uint32_t id)
{
    m_LanguageId = id;
}

Tlk::TlkMapType::const_iterator Tlk::begin() const
{
    return std::cbegin(m_TlkMap);
}

Tlk::TlkMapType::const_iterator Tlk::end() const
{
    return std::cend(m_TlkMap);
}

bool Tlk::WriteToFile(const char* path) const
{
    Raw::Tlk rawTlk;

    std::memcpy(rawTlk.m_Header.m_FileType, "TLK ", 4);
    std::memcpy(rawTlk.m_Header.m_FileVersion, "V3.0 ", 4);
    rawTlk.m_Header.m_LanguageID = m_LanguageId;

    std::uint32_t stringOffset = 0;

    for (const auto& entry : m_TlkMap)
    {
        Raw::TlkStringData stringData;

        std::uint32_t flags = 0;

        stringData.m_OffsetToString = 0;
        stringData.m_StringSize = 0;

        if (entry.second.m_String.has_value())
        {
            flags |= Raw::TlkStringData::StringFlags::TEXT_PRESENT;

            const std::string& str = entry.second.m_String.value();
            stringData.m_OffsetToString = stringOffset;
            stringData.m_StringSize = static_cast<std::uint32_t>(str.size());

            rawTlk.m_StringEntries.resize(stringOffset + stringData.m_StringSize);
            std::memcpy(rawTlk.m_StringEntries.data() + stringOffset, str.c_str(), stringData.m_StringSize);

            stringOffset += stringData.m_StringSize;
        }

        std::memset(stringData.m_SoundResRef, 0, 16);

        if (entry.second.m_SoundResRef.has_value())
        {
            flags |= Raw::TlkStringData::StringFlags::SND_PRESENT;
            const char* str = entry.second.m_SoundResRef->c_str();
            std::memcpy(stringData.m_SoundResRef, str, strnlen(str, 16));
        }

        stringData.m_SoundLength = 0;

        if (entry.second.m_SoundLength.has_value())
        {
            flags |= Raw::TlkStringData::StringFlags::SNDLENGTH_PRESENT;
            stringData.m_SoundLength = entry.second.m_SoundLength.value();
        }

        stringData.m_Flags = static_cast<Raw::TlkStringData::StringFlags>(flags);

        stringData.m_VolumeVariance = 0;
        stringData.m_PitchVariance = 0;

        rawTlk.m_StringData.emplace_back(std::move(stringData));
    }

    rawTlk.m_Header.m_StringCount = static_cast<std::uint32_t>(rawTlk.m_StringData.size());
    rawTlk.m_Header.m_StringEntriesOffset = sizeof(rawTlk.m_Header) + (sizeof(rawTlk.m_StringData[0]) * rawTlk.m_Header.m_StringCount);

    return rawTlk.WriteToFile(path);
}

}
