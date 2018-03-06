#pragma once

#include "Utility/DataBlock.hpp"

#if OS_WINDOWS
    #include "Windows.h"
#else
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

// This is separated out into an _impl to avoid drawing in system headers for consumers of the API.
class MemoryMappedFile_impl
{
public:
    MemoryMappedFile_impl();
    ~MemoryMappedFile_impl();

    bool Map(char const* path, NonOwningDataBlock* out);

private:

#if OS_WINDOWS
    HANDLE m_File;
    HANDLE m_MemoryMap;
    LPVOID m_Ptr;
#else
    int m_FileDescriptor;
    void* m_Ptr;
    std::size_t m_PtrLength;
#endif
};
