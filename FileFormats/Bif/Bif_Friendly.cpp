#include "FileFormats/Bif/Bif_Friendly.hpp"
#include "Utility/Assert.hpp"

#include <cstring>

namespace FileFormats::Bif::Friendly {

Bif::Bif(Raw::Bif const& rawBif)
{
    ConstructInternal(rawBif);
}

Bif::Bif(Raw::Bif&& rawBif) : m_RawBif(std::forward<Raw::Bif>(rawBif))
{
    ConstructInternal(m_RawBif.value());
}

void Bif::ConstructInternal(Raw::Bif const& rawBif)
{
    ASSERT(!rawBif.m_Header.m_FixedResourceCount);

    // Calculate the offset into the data block manually ...
    std::size_t offsetToDataBlock = rawBif.m_Header.m_VariableTableOffset;
    offsetToDataBlock += rawBif.m_VariableResourceTable.size() * sizeof(Raw::BifVariableResource);
    offsetToDataBlock += rawBif.m_FixedResourceTable.size() * sizeof(Raw::BifFixedResource);

    for (Raw::BifVariableResource const& rawRes : rawBif.m_VariableResourceTable)
    {
        ASSERT(m_Resources.find(rawRes.m_Id) == std::end(m_Resources));

        std::unique_ptr<BifResource> res;

        std::size_t offsetToData = rawRes.m_Offset - offsetToDataBlock;
        ASSERT(offsetToData + rawRes.m_FileSize <= rawBif.m_DataBlock.size());

        if (m_RawBif.has_value())
        {
            std::unique_ptr<BifStreamedResource> streamedRes = std::make_unique<BifStreamedResource>();
            streamedRes->m_Data = rawBif.m_DataBlock.data() + offsetToData;
            streamedRes->m_DataLength = rawRes.m_FileSize;
            res = std::move(streamedRes);
        }
        else
        {
            std::unique_ptr<BifFrontLoadedResource> frontLoadedRes = std::make_unique<BifFrontLoadedResource>();
            frontLoadedRes->m_Data.resize(rawRes.m_FileSize);
            std::memcpy(frontLoadedRes->m_Data.data(), rawBif.m_DataBlock.data() + offsetToData, rawRes.m_FileSize);
            res = std::move(frontLoadedRes);
        }

        res->m_ResId = rawRes.m_Id;
        res->m_ResType = rawRes.m_ResourceType;

        // The spec outlines this the m_ReferencedBifResId as (x << 20) + y, where y is the index, and x = y normally and 0
        // for patch BIFs. However, none of the BIFs present in 1.69 or 1.74 seem to follow this rule - x always equals y.
        //
        // There exists no entry that specifies x. In the spec, it also states that the game doesn't care about the mismatch
        // between x and y.
        //
        // Therefore, I'm just going to do what the game does - mask out anything higher than 0x00003FFF
        // (bottom fourteen bits) so we can just completely ignore the x value.
        //
        // We'll use this as the index into the BIF while maintaining the original ID as an entry field.

        std::uint32_t keyAddressableId = rawRes.m_Id & 0x00003FFF;

        m_Resources.insert(std::make_pair(keyAddressableId, std::move(res)));
    }
}

Bif::BifResourceMap const& Bif::GetResources() const
{
    return m_Resources;
}

}
