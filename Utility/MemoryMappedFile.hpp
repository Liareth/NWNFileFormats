#pragma once

#include "Utility/DataBlock.hpp"

#include <cstddef>
#include <memory>

// This class wraps memory-mapped read only access to a file.
// This will map the entire range of the file (0 -> fileSize) into a memory rage represented by GetDataBlock().
// This can later be expanded to support spans within the file and writing too.
class MemoryMappedFile
{
public:
    MemoryMappedFile();
    MemoryMappedFile(MemoryMappedFile&& rhs);
    ~MemoryMappedFile();

    static bool MemoryMap(char const* path, MemoryMappedFile* out);
    NonOwningDataBlock const& GetDataBlock();

private:
    NonOwningDataBlock m_DataBlock;

    struct PlatformImpl;
    std::unique_ptr<PlatformImpl> m_PlatformImpl;
};
