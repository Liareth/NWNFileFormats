#include "MemoryMappedFile.hpp"

#if OS_WINDOWS
    #include "Windows.h"
#else
#endif

// An RAII wrapper around the naked resources exposed by the platform.
struct MemoryMappedFile::PlatformImpl
{
#if OS_WINDOWS
    HANDLE m_File;
    HANDLE m_MemoryMap;
    PlatformImpl() : m_File(INVALID_HANDLE_VALUE), m_MemoryMap(INVALID_HANDLE_VALUE) { }
    ~PlatformImpl()
    {
        if (m_File != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_File);
        }

        if (m_MemoryMap != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_MemoryMap);
        }
    }
#else
#endif
};

MemoryMappedFile::MemoryMappedFile() { }
MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& rhs) : m_DataBlock(std::move(rhs.m_DataBlock)), m_PlatformImpl(std::move(rhs.m_PlatformImpl)) { }
MemoryMappedFile::~MemoryMappedFile() { }

bool MemoryMappedFile::MemoryMap(char const* path, MemoryMappedFile* out)
{
    out->m_PlatformImpl = std::make_unique<PlatformImpl>();

#if OS_WINDOWS
    out->m_PlatformImpl->m_File = CreateFile(path, GENERIC_READ, FILE_SHARE_READ,  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (out->m_PlatformImpl->m_File == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    out->m_PlatformImpl->m_MemoryMap = CreateFileMapping(out->m_PlatformImpl->m_File, NULL, PAGE_READONLY, 0, 0, NULL);
    if (out->m_PlatformImpl->m_MemoryMap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LPVOID ptr = MapViewOfFile(out->m_PlatformImpl->m_MemoryMap, FILE_MAP_READ, 0, 0, 0);
    if (!ptr)
    {
        return false;
    }

    out->m_DataBlock.m_Data = static_cast<std::byte*>(ptr);
    out->m_DataBlock.m_DataLength = GetFileSize(out->m_PlatformImpl->m_File, NULL);
#else
    ASSERT_FAIL();
#endif

    return true;
}

NonOwningDataBlock const& MemoryMappedFile::GetDataBlock()
{
    return m_DataBlock;
}
