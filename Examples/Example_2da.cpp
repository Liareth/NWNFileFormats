#include "FileFormats/2da.hpp"
#include "Utility/Assert.hpp"

namespace {

int TwoDAExample(char* path);

int TwoDAExample(char* path)
{
    using namespace FileFormats::TwoDA;

    Raw::TwoDA raw2da;
    bool loaded = Raw::TwoDA::ReadFromFile(path, &raw2da);

    std::printf("2DA Lines: %zu\n", raw2da.m_Lines.size());

    if (!loaded)
    {
        std::printf("Failed to load the 2DA file.\n");
        return 1;
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return TwoDAExample(argv[1]);
}
