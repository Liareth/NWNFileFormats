#include "FileFormats/Erf.hpp"
#include "Utility/Assert.hpp"

namespace
{

int extract_erf(const char* out_path, const char* in_path)
{
    using namespace FileFormats::Erf;

    Raw::Erf erf_raw;
    bool loaded = Raw::Erf::ReadFromFile(in_path, &erf_raw);

    if (!loaded)
    {
        std::printf("Failed to open %s.", in_path);
        return 1;
    }

    Friendly::Erf erf(std::move(erf_raw));

    for (const Friendly::ErfResource& resource : erf.GetResources())
    {
        char path[512];
        sprintf(path, "%s/%s.%s", out_path, resource.m_ResRef.c_str(), FileFormats::Resource::StringFromResourceType(resource.m_ResType));

        FILE* file = std::fopen(path, "wb");
        if (file)
        {
            std::printf("Writing %s.\n", path);
            std::fwrite(resource.m_DataBlock->GetData(), resource.m_DataBlock->GetDataLength(), 1, file);
            std::fclose(file);
        }
        else
        {
            std::printf("Failed to open %s\n", path);
        }
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::printf("erf_extractor [out_dir] [erf_path]\n");
        return 1;
    }

    return extract_erf(argv[1], argv[2]);
}
