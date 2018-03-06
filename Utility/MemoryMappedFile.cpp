#include "Utility/MemoryMappedFile.hpp"
#include "Utility/Assert.hpp"
#include "Utility/MemoryMappedFile_impl.hpp"

MemoryMappedFile::MemoryMappedFile() { }
MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& rhs) : m_DataBlock(std::move(rhs.m_DataBlock)), m_PlatformImpl(std::move(rhs.m_PlatformImpl)) { }
MemoryMappedFile::~MemoryMappedFile() { }

bool MemoryMappedFile::MemoryMap(char const* path, MemoryMappedFile* out)
{
    out->m_PlatformImpl = std::make_unique<MemoryMappedFile_impl>();
    return out->m_PlatformImpl->Map(path, &out->m_DataBlock);
}

NonOwningDataBlock const& MemoryMappedFile::GetDataBlock()
{
    return m_DataBlock;
}
