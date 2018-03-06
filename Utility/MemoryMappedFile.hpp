#pragma once

#include "Utility/DataBlock.hpp"

#include <cstddef>
#include <memory>

class MemoryMappedFile_impl;

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
    std::unique_ptr<MemoryMappedFile_impl> m_PlatformImpl;
};
