#include "FileFormats/Bif.hpp"
#include "Utility/Assert.hpp"

namespace {

int BifExample(char* path);

int BifExample(char* path)
{
    using namespace FileFormats::Bif;

    Raw::Bif rawBif;
    bool loaded = Raw::Bif::ReadFromFile(path, &rawBif);

    // Alternatively, we could have loaded the file ourselves and use 'ReadFromByteVector' or 'ReadFromBytes'.

    std::printf("BIF FileType: %.4s\n", rawBif.m_Header.m_FileType);
    std::printf("BIF Version: %.4s\n", rawBif.m_Header.m_Version);
    std::printf("BIF Variable resources: %u\n", rawBif.m_Header.m_VariableResourceCount);
    std::printf("BIF Fixed resources: %u\n", rawBif.m_Header.m_FixedResourceCount);

    if (!loaded)
    {
        std::printf("Failed to load the BIF file. Check the FileType and Version and ensure the file is well formed.\n");
        return 1;
    }

    // Remember to use std::move here.
    // If you don't, you're gonna use double the memory because everything is going to get copied.
    Friendly::Bif bif(std::move(rawBif));

    std::printf("\nResources:\n");

    for (auto const& kvp : bif.GetResources())
    {
        // kvp.first = id
        // kvp.second = Friendly::BifResource
        std::printf("\n%s [%u | %u]: %zu bytes", StringFromResourceType(kvp.second.m_ResType), kvp.first, kvp.second.m_ResId, kvp.second.m_DataBlock->GetDataLength());
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return BifExample(argv[1]);
}