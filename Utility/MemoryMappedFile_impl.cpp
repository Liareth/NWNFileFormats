#include "MemoryMappedFile_impl.hpp"
#include "Utility/Assert.hpp"

#if OS_LINUX
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

MemoryMappedFile_impl::MemoryMappedFile_impl()
#if OS_WINDOWS
    : m_File(INVALID_HANDLE_VALUE), m_MemoryMap(INVALID_HANDLE_VALUE), m_Ptr(NULL)
#else
    : m_FileDescriptor(-1), m_Ptr(nullptr), m_PtrLength(0)
#endif
{ }

MemoryMappedFile_impl::~MemoryMappedFile_impl()
{
#if OS_WINDOWS
    if (m_Ptr != NULL)
    {
        UnmapViewOfFile(m_Ptr);
    }

    if (m_MemoryMap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_MemoryMap);
    }

    if (m_File != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_File);
    }
#else
    if (m_Ptr == MAP_FAILED)
    {
        munmap(m_Ptr, m_PtrLength);
    }

    if (m_FileDescriptor != -1)
    {
        close(m_FileDescriptor);
    }
#endif
}

bool MemoryMappedFile_impl::Map(char const* path, NonOwningDataBlock* out)
{
    ASSERT(path);
    ASSERT(out);

#if OS_WINDOWS
    m_File = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_File == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    m_MemoryMap = CreateFileMapping(m_File, NULL, PAGE_READONLY, 0, 0, NULL);
    if (m_MemoryMap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    m_Ptr = MapViewOfFile(m_MemoryMap, FILE_MAP_READ, 0, 0, 0);
    if (!m_Ptr)
    {
        return false;
    }

    out->m_Data = static_cast<std::byte*>(m_Ptr);
    out->m_DataLength = GetFileSize(m_File, NULL);
#else
    m_FileDescriptor = open(path, O_RDONLY);
    if (m_FileDescriptor == -1)
    {
        return false;
    }

    struct stat statBuffer;
    if (stat(path, &statBuffer) == -1)
    {
        return false;
    }

    m_PtrLength = statBuffer.st_size;
    m_Ptr = mmap(nullptr, m_PtrLength, PROT_READ, MAP_PRIVATE, m_FileDescriptor, 0);

    if (m_Ptr == MAP_FAILED)
    {
        return false;
    }

    out->m_Data = static_cast<std::byte*>(m_Ptr);
    out->m_DataLength = m_PtrLength;
#endif

    return true;
}
