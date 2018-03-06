#include "FileFormats/Key.hpp"
#include "Utility/Assert.hpp"

namespace {

int KeyExample(char* path);

int KeyExample(char* path)
{
    using namespace FileFormats::Key;

    Raw::Key rawKey;
    bool loaded = Raw::Key::ReadFromFile(path, &rawKey);

    std::printf("Key FileType: %.4s\n", rawKey.m_Header.m_FileType);
    std::printf("Key FileVersion: %.4s\n", rawKey.m_Header.m_FileVersion);
    std::printf("Key BIF Entries: %u\n", rawKey.m_Header.m_BIFCount);
    std::printf("Key Entries: %u\n", rawKey.m_Header.m_KeyCount);

    if (!loaded)
    {
        std::printf("Failed to load the KEY file. Check the FileType and FileVersion and ensure the file is well formed.\n");
        return 1;
    }

    Friendly::Key key(std::move(rawKey));

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
        std::printf("\n %s.%s %s [%zu (%u) | %u] ", res.m_ResRef.c_str(), resType, bifPath.c_str(),
            res.m_ReferencedBifIndex, res.m_ReferencedBifResId, res.m_ResId);
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);
    return KeyExample(argv[1]);
}
