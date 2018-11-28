#include "FileFormats/Tlk.hpp"
#include "Utility/Assert.hpp"

namespace {

int TlkExample(char* path);

int TlkExample(char* path)
{
    using namespace FileFormats::Tlk;

    Raw::Tlk rawTlk;
    bool loaded = Raw::Tlk::ReadFromFile(path, &rawTlk);

    std::printf("Tlk FileType: %.4s\n", rawTlk.m_Header.m_FileType);
    std::printf("Tlk FileVersion: %.4s\n", rawTlk.m_Header.m_FileVersion);

    if (!loaded)
    {
        std::printf("Failed to load the Tlk file. Check the FileType and FileVersion and ensure the file is well formed.\n");
        return 1;
    }

    Friendly::Tlk tlk(std::move(rawTlk));

    // Grab some strings via direct strref. If it doesn't exist, it returns emptry string.
    std::printf("\n0x00000000: '%s'", tlk[0x00000000].c_str());
    std::printf("\n0x00000010: '%s'", tlk[0x00000010].c_str());
    std::printf("\n0xFFFFFFFF: '%s'", tlk[0xFFFFFFFF].c_str());

    // Print the entire tlk table.
    for (auto const& entry : tlk)
    {
        std::printf("\n%u -> '%s'", entry.first, entry.second.m_String.value_or("****").c_str());
    }

    // Then save the tlk back out next to the original.
    char pathBuffer[1024];
    std::sprintf(pathBuffer, "%s.1", path);
    bool written = tlk.WriteToPath(pathBuffer);
    ASSERT(written);

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return TlkExample(argv[1]);
}
