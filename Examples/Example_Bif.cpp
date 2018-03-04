#include "FileFormats/Bif.hpp"
#include "Utility/Assert.hpp"

namespace {

bool ReadAllBytes(const char* path, std::vector<std::byte>* out)
{
    FILE* file = std::fopen(path, "rb");

    if (file)
    {
        std::fseek(file, 0, SEEK_END);
        std::size_t fileLen = ftell(file);
        std::fseek(file, 0, SEEK_SET);

        std::size_t outSize = out->size();
        out->resize(outSize + fileLen);
        std::size_t read = std::fread(out->data() + outSize, 1, fileLen, file);
        ASSERT(read == fileLen);

        std::fclose(file);
        return true;
    }

    return false;
}

int BifExample(char* path);

int BifExample(char* path)
{
    std::vector<std::byte> bifData;
    bool file = ReadAllBytes(path, &bifData);

    if (!file)
    {
        std::printf("Failed to open file %s.\n", path);
        return 1;
    }

    using namespace FileFormats::Bif;

    Raw::Bif rawBif;
    bool loaded = Raw::Bif::ReadFromBytes(bifData.data(), bifData.size(), &rawBif);

    std::printf("BIF FileType: %.4s\n", rawBif.m_Header.m_FileType);
    std::printf("BIF Version: %.4s\n", rawBif.m_Header.m_Version);
    std::printf("BIF Variable resources: %u\n", rawBif.m_Header.m_VariableResourceCount);
    std::printf("BIF Fixed resources: %u\n", rawBif.m_Header.m_FixedResourceCount);

    if (!loaded)
    {
        std::printf("Failed to load the BIF file. Check the FileType and Version and ensure the file is well formed.\n");
        return 1;
    }

    Friendly::Bif bif(rawBif);

    std::printf("\nResources:\n");

    for (auto const& kvp : bif.GetResources())
    {
        // kvp.first = id
        // kvp.second = Friendly::BifResource
        std::printf("\n%s [%u | %u]: %zu bytes", StringFromResourceType(kvp.second.m_ResType), kvp.first, kvp.second.m_ResId, kvp.second.m_Data.size());
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return BifExample(argv[1]);
}