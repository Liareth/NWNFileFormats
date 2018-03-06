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

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return TlkExample(argv[1]);
}
