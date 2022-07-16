#pragma once

#include <cstddef>
#include <cstdio>

// This is a basic interface for stream writing.
// It allows us to write different targets through the same interface.
class IStreamWriter
{
public:
    virtual void Write(void const* data, size_t len) = 0;
};

class FileStreamWriter : public IStreamWriter
{
public:
    FileStreamWriter(FILE* file);
    virtual void Write(void const* data, size_t len) override;

private:
    FILE* m_File;
};

class MemoryStreamWriter : public IStreamWriter
{
public:
    MemoryStreamWriter(std::byte* memory);
    virtual void Write(void const* data, size_t len) override;

private:
    std::byte* m_Memory;
    size_t m_Offset;
};

class SizeStreamWriter : public IStreamWriter
{
public:
    SizeStreamWriter();
    virtual void Write(void const* data, size_t len) override;
    size_t GetSize() const;

private:
    size_t m_Size;
};
