#pragma once

#include "FileFormats/Bif/Bif_Friendly.hpp"
#include "Utility/Assert.hpp"

namespace FileFormats::Bif::Friendly {

Bif::Bif(Raw::Bif const& rawBif)
{
    ASSERT(!rawBif.m_Header.m_FixedResourceCount);

    // Calculate the offset into the data block manually ...
    std::size_t offsetToDataBlock = rawBif.m_Header.m_VariableTableOffset;
    offsetToDataBlock += rawBif.m_VariableResourceTable.size() * sizeof(Raw::BifVariableResource);
    offsetToDataBlock += rawBif.m_FixedResourceTable.size() * sizeof(Raw::BifFixedResource);

    for (Raw::BifVariableResource const& rawRes : rawBif.m_VariableResourceTable)
    {
        ASSERT(m_Resources.find(rawRes.m_Id) == std::end(m_Resources));

        BifResource res;
        res.m_ResType = rawRes.m_ResourceType;

        std::size_t offsetToData = rawRes.m_Offset - offsetToDataBlock;
        ASSERT(offsetToData + rawRes.m_FileSize <= rawBif.m_DataBlock.size());

        res.m_Data.resize(rawRes.m_FileSize);
        std::memcpy(res.m_Data.data(), rawBif.m_DataBlock.data() + offsetToData, rawRes.m_FileSize);

        m_Resources.insert(std::make_pair(rawRes.m_Id, std::move(res)));
    }
}

Bif::BifResourceMap const& Bif::GetResources() const
{
    return m_Resources;
}

}
