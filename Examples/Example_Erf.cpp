#include "FileFormats/Erf.hpp"
#include "Utility/Assert.hpp"

namespace {

bool ReadAllButes(const char* path, std::vector<std::byte>* out)
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

int ErfExample(char* path);

int ErfExample(char* path)
{
    std::vector<std::byte> erfData;
    bool file = ReadAllButes(path, &erfData);

    if (!file)
    {
        std::printf("Failed to open file %s.\n", path);
        return 1;
    }

    FileFormats::Erf::Raw::Erf rawErf;
    bool loaded = FileFormats::Erf::Raw::Erf::ReadFromBytes(erfData.data(), erfData.size(), &rawErf);

    std::printf("ERF FileType: %.4s\n", rawErf.m_Header.m_FileType);
    std::printf("ERF Version: %.4s\n", rawErf.m_Header.m_Version);
    std::printf("ERF Entries: %u\n", rawErf.m_Header.m_EntryCount);

    if (!loaded)
    {
        std::printf("Failed to load the ERF file. Check the FileType and Version and ensure the file is well formed.\n");
        return 1;
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return ErfExample(argv[1]);
}