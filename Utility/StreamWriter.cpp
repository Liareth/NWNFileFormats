#include "Utility/StreamWriter.hpp"
#include "Utility/Assert.hpp"
#include <cstring>

FileStreamWriter::FileStreamWriter(FILE* file)
    : m_File(file)
{
    ASSERT(m_File);
}

void FileStreamWriter::Write(void const* data, size_t len)
{
    std::fwrite((void*)data, len, 1, m_File);
}

MemoryStreamWriter::MemoryStreamWriter(std::byte* memory)
    : m_Memory(memory), m_Offset(0)
{ }

void MemoryStreamWriter::Write(void const* data, size_t len)
{
    std::memcpy(m_Memory + m_Offset, data, len);
    m_Offset += len;
}

SizeStreamWriter::SizeStreamWriter()
    : m_Size(0)
{ }

void SizeStreamWriter::Write(void const*, size_t len)
{
    m_Size += len;
}

size_t SizeStreamWriter::GetSize() const
{
    return m_Size;
}
