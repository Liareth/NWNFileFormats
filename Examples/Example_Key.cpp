#include "FileFormats/Key.hpp"
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

int KeyExample(char* path);

int KeyExample(char* path)
{
    std::vector<std::byte> keyData;
    bool file = ReadAllButes(path, &keyData);

    if (!file)
    {
        std::printf("Failed to open file %s.\n", path);
        return 1;
    }

    using namespace FileFormats::Key;

    Raw::Key rawKey;
    bool loaded = Raw::Key::ReadFromBytes(keyData.data(), &rawKey);

    std::printf("Key FileType: %.4s\n", rawKey.m_Header.m_FileType);
    std::printf("Key FileVersion: %.4s\n", rawKey.m_Header.m_FileVersion);
    std::printf("Key BIF Entries: %u\n", rawKey.m_Header.m_BIFCount);
    std::printf("Key Entries: %u\n", rawKey.m_Header.m_KeyCount);

    if (!loaded)
    {
        std::printf("Failed to load the KEY file. Check the FileType and FileVersion and ensure the file is well formed.\n");
        return 1;
    }

    Friendly::Key key(rawKey);

    std::printf("\nReferences BIFs:\n");

    for (Friendly::KeyBifReference const& ref : key.GetReferencedBifs())
    {
        std::printf("\nDrives: %u, Path: %s, Size: %u", ref.m_Drives, ref.m_Path.c_str(), ref.m_FileSize);
    }

    std::printf("\n\nReferenced resources:\n");

    for (Friendly::KeyBifReferencedResource const& res : key.GetReferencedResources())
    {
        ASSERT(res.m_ReferencedBifIndex < key.GetReferencedBifs().size());
        const char* resType = FileFormats::Resource::StringFromResourceType(res.m_ResType);
        std::string const& bifPath = key.GetReferencedBifs()[res.m_ReferencedBifIndex].m_Path;
        std::printf("\n %s.%s %s [%u] ", res.m_ResRef.c_str(), resType, bifPath.c_str(), res.m_ResId);
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return KeyExample(argv[1]);
}
