#include "FileFormats/Erf.hpp"
#include "Utility/Assert.hpp"

namespace {

int ErfExample(char* path);

int ErfExample(char* path)
{
    using namespace FileFormats::Erf;

    Raw::Erf rawErf;
    bool loaded = Raw::Erf::ReadFromFile(path, &rawErf);

    std::printf("ERF FileType: %.4s\n", rawErf.m_Header.m_FileType);
    std::printf("ERF Version: %.4s\n", rawErf.m_Header.m_Version);
    std::printf("ERF Entries: %u\n", rawErf.m_Header.m_EntryCount);

    if (!loaded)
    {
        std::printf("Failed to load the ERF file. Check the FileType and Version and ensure the file is well formed.\n");
        return 1;
    }

    Friendly::Erf erf(std::move(rawErf));

    std::vector<Raw::ErfLocalisedString> const& descriptions = erf.GetDescriptions();

    if (descriptions.empty())
    {
        std::printf("\nNo description specified\n");
    }
    else
    {
        // Print the first description - this may not be English but probably will be.
        std::printf("\nDescription: %s\n", descriptions[0].m_String.c_str());
    }

    std::printf("\nResources:\n");

    for (Friendly::ErfResource const& resource : erf.GetResources())
    {
        const char* resType = FileFormats::Resource::StringFromResourceType(resource.m_ResType);
        std::printf("\n %s.%s: %zu bytes [%u] ", resource.m_ResRef.c_str(), resType, resource.m_DataBlock->GetDataLength(), resource.m_ResourceId);
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return ErfExample(argv[1]);
}