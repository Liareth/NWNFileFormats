#pragma once

#include "FileFormats/Resource.hpp"
#include "Utility/DataBlock.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <memory>
#include <vector>

namespace FileFormats::Bif::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_KeyBIF_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.

// A BIF contains mutliple resources (files). It does not contain information about each resource's name,
// and therefore requires its KEY file.

struct BifHeader
{
    char m_FileType[4]; // "BIFF"
    char m_Version[4]; // "V1"
    std::uint32_t m_VariableResourceCount; // Number of variable resources in this file.
    std::uint32_t m_FixedResourceCount; // Number of fixed resources in this file.
    std::uint32_t m_VariableTableOffset; // Byte position of the Variable Resource Table from beginning of this file.
};

struct BifVariableResource
{
    // The Variable Resource Table has a number of entries equal to the Variable Resource Count
    // specified in the Header

    // A unique ID number. It is generated as follows:
    // Variable ID = (x << 20) + y
    // (<< means bit shift left)
    // y = [Index of this Resource Entry in the BIF]
    // In the BIFs included with the game CDs, x = y.
    // In the patch BIFs, x = 0.
    // This discrepancy in x values does not matter to the game or toolset because
    // their resource manager system doesn't care about the value of x in a BIF.
    std::uint32_t m_Id;

    // The location of the variable resource data. This is a byte
    // offset from the beginning of the BIF file into the Variable Resource Data block.
    std::uint32_t m_Offset;

    // File size of this resource. Specifies the number of bytes
    // in the Variable Resource Data block that belong to this resource.
    std::uint32_t m_FileSize;

    // Resource type of this resource
    Resource::ResourceType m_ResourceType;
};

struct BifFixedResource
{
    // NOTE: This block is actually not implemented. Support for Fixed Resources is available, as the
    // offset is left in the BIF header, but there is currently nothing implemented. As a result, there is no
    // existing data type for this. Below is what would conceptually become the Fixed resource table.
    //
    // The Fixed Resource Table has a number of entries equal to the Fixed Resource Count specified in
    // the Header. If it has one or more elements, it is located immediately after the end of the Variable
    // Resource Table. If there are no fixed resources, then this block is not present at all and the
    // Variable Resource Data block immediately follows the Variable Resource Table.

    // A unique ID number. It is generated as follows:
    // Variable ID = (x << 20) + y
    // (<< means bit shift left)
    // y = [Index of this Resource Entry in the BIF]
    // In the BIFs included with the game CDs, x = y.
    // In the patch BIFs, x = 0.
    // This discrepancy in x values does not matter to the
    // game or toolset because their resource manager
    // system doesn't care about the value of x in a BIF
    std::uint32_t m_ID;

    // The location of the fixed resource data. This is a byte
    // offset from the beginning of the BIF file into the Fixed
    // Resource Data block.
    std::uint32_t m_Offset;

    // Number of parts
    std::uint32_t m_PartCount;

    // File size of this resource
    std::uint32_t m_FileSize;

    // Resource type of this resource
    std::uint32_t m_ResourceType;
};

using BifDataBlock = DataBlock;

class Bif
{
public:
    BifHeader m_Header;
    std::vector<BifVariableResource> m_VariableResourceTable;
    std::vector<BifFixedResource> m_FixedResourceTable;

    // NOTE: In the spec, this is separated into a variable resource data block and a fixed resource data block.
    // Unfortunately, there's nothing in the header that allows us to observe the size of each of these blocks.
    // Therefore, I am just rolling each block into one big vector for the purposes of this.
    std::unique_ptr<BifDataBlock> m_DataBlock;

    // Constructs a Bif from a non-owning pointer. Memory usage may be high.
    static bool ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, Bif* out);

    // Constructs a Bif from a vector of bytes which we have taken ownership of. Memory usage will be moderate.
    static bool ReadFromByteVector(std::vector<std::byte>&& bytes, Bif* out);

    // Constructs a Bif from a file. The file with be memory mapped so memory usage will be ideal.
    static bool ReadFromFile(char const* path, Bif* out);

private:

    // This is an RAII wrapper around the various methods of loading a BIF that we have.
    // - If by bytes, this is nullptr.
    // - If by byte vector, this will contain the vector.
    // - If by file, this will contain a handle to the file (since we're memory mapping).
    struct BifDataBlockStorage { virtual ~BifDataBlockStorage() = 0 {} };
    template <typename T> struct BifDataBlockStorageRAII;
    std::unique_ptr<BifDataBlockStorage> m_DataBlockStorage;

    bool ConstructInternal(std::byte const* bytes);
    void ReadVariableResourceTable(std::byte const* data);
    void ReadFixedResourceTable(std::byte const* data);
};

}
