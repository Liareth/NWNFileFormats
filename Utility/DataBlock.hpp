#pragma once

#include <cstddef>
#include <vector>

// This describes a block of data - like an array_view or a span<t> or something like that.
// It can be an owning or non-owning.
struct DataBlock
{
    // Returns a non-owning pointer to the data.
    virtual std::byte const* GetData() const = 0;

    // Returns the length of the data.
    virtual std::size_t GetDataLength() const  = 0;

    virtual ~DataBlock() {};
};

struct OwningDataBlock : public DataBlock
{
    // The raw data associated with this.
    std::vector<std::byte> m_Data;

    virtual std::byte const* GetData() const override { return m_Data.data(); }
    virtual std::size_t GetDataLength() const override { return m_Data.size(); }
    virtual ~OwningDataBlock() {};
};

struct NonOwningDataBlock : public DataBlock
{
    // The raw data associated with this. This is a NON-OWNING pointer.
    std::byte const* m_Data;

    // And the length.
    std::size_t m_DataLength;

    virtual std::byte const* GetData() const override { return m_Data; }
    virtual std::size_t GetDataLength() const override { return m_DataLength; }
    virtual ~NonOwningDataBlock() {};
};
