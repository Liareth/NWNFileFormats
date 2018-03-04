#include "FileFormats/Bif.hpp"
#include "FileFormats/Key.hpp"
#include "Utility/Assert.hpp"

#include <map>

#if OS_WINDOWS
    #include "Windows.h"
#endif

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

// Recursively make the provided directory.
void RecursivelyEnsureDir(std::string const& dir)
{
#if OS_WINDOWS
    for (std::size_t slashIndex = dir.find_first_of("\\/");
        slashIndex != std::string::npos;
        slashIndex = dir.find_first_of("\\/", slashIndex + 1))
    {
        CreateDirectoryA(dir.substr(0, slashIndex).c_str(), NULL);
    }
#else
    ASSERT_FAIL();
#endif
}

int KeyBifExtractorExample(char* keyPath, char* basePath, char* outPath);

int KeyBifExtractorExample(char* keyPath, char* basePath, char* outPath)
{
    std::vector<std::byte> keyData;
    bool file = ReadAllButes(keyPath, &keyData);

    if (!file)
    {
        std::printf("Failed to open key file %s.\n", keyPath);
        return 1;
    }

    using namespace FileFormats;

    // We need to iterate over each BIF in turn, over all resources, which will allow us to extract the data to the correct file name.
    // The first step is to go over every referenced resource, and sort them into BIF buckets.
    // We'll extract by BIF. This prevents us from having to open all BIFs up front.
    std::map<std::size_t, std::vector<Key::Friendly::KeyBifReferencedResource>> resMap;

    // We also take a copy of the references bifs for use later.
    std::vector<Key::Friendly::KeyBifReference> bifRefs;

    {
        Key::Raw::Key rawKey;
        bool loaded = Key::Raw::Key::ReadFromBytes(keyData.data(), &rawKey);

        if (!loaded)
        {
            std::printf("Failed to load the KEY file.\n");
            return 1;
        }

        Key::Friendly::Key key(rawKey);

        for (Key::Friendly::KeyBifReferencedResource const& res : key.GetReferencedResources())
        {
            resMap[res.m_ReferencedBifIndex].emplace_back(res);
        }

        bifRefs = key.GetReferencedBifs();
    }

    // Iterate over all the buckets now - for each bucket we want to conver to the ID that the BIF will expect.
    for (auto& kvp : resMap)
    {
        for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(kvp.second.size()); ++i)
        {
            kvp.second[i].m_ResId = (i << 20 ) + i;
        }
    }

    // At this point, we have every resource we care about. Now we iterate over every BIF,
    // load it up, get what we want out of it, and close it down again.
    std::size_t totalExtractedResources = 0;
    std::vector<std::byte> bifData; // Stored outside to avoid big reallocations each time.

    for (std::size_t i = 0; i < bifRefs.size(); ++i)
    {
        Key::Friendly::KeyBifReference const& bifref = bifRefs[i];

        std::string bifPath = std::string(basePath) + "/" + bifref.m_Path;
        file = ReadAllButes(bifPath.c_str(), &bifData);
        ASSERT(file);

        if (!file)
        {
            std::printf("Failed to open BIF file %s.\n", bifPath.c_str());
            continue;
        }

        Bif::Raw::Bif rawBif;
        bool loaded = Bif::Raw::Bif::ReadFromBytes(bifData.data(), bifData.size(), &rawBif);
        ASSERT(loaded);

        if (!loaded)
        {
            std::printf("Failed to deserialise the BIF file %s.\n", bifPath.c_str());
            continue;
        }

        // Infer the file name from the path.
        std::string bifFileName = bifPath;
        std::size_t lastSlash = bifFileName.find_last_of("\\/");
        if (lastSlash != std::string::npos)
        {
            bifFileName.erase(0, lastSlash + 1);
        }

        // Grab the directory for this bif. This will be used later.
        std::string bifFolder = std::string(outPath) + "/" + bifFileName + "/";
        RecursivelyEnsureDir(bifFolder);

        Bif::Friendly::Bif bif(rawBif);
        std::size_t extractedResources = 0;

        Bif::Friendly::Bif::BifResourceMap const& bifResMap = bif.GetResources();

        for (Key::Friendly::KeyBifReferencedResource const& bifRefRes : resMap[i])
        {
            // We're iterating over every resource that KEY calls out, and that we've assigned
            // to this BIF's bucket.

            auto resInBif = bifResMap.find(bifRefRes.m_ResId);
            ASSERT(resInBif != std::end(bifResMap));
            std::string resourcePath = bifFolder + bifRefRes.m_ResRef + "." + Resource::StringFromResourceType(bifRefRes.m_ResType);

            FILE* resFile = std::fopen(resourcePath.c_str(), "wb");
            ASSERT(resFile);

            if (!resFile)
            {
                std::printf("Failed to open %s for write.\n", resourcePath.c_str());
                continue;
            }

            std::fwrite(resInBif->second.m_Data.data(), resInBif->second.m_Data.size(), 1, resFile);
            std::fclose(resFile);

            ++extractedResources;
        }

        std::printf("Extracted %zu resources from BIF %s to %s\n", extractedResources, bifPath.c_str(), bifFolder.c_str());
        totalExtractedResources += extractedResources;

        bifData.clear();
    }

    std::printf("Extracted %zu resources total referenced by KEY %s\n", totalExtractedResources, keyPath);

    return 0;
}

}

// Invoked as such (on Windows)
// "G:/GOG Games/NWN Diamond/chitin.key" "G:/GOG Games/NWN Diamond" "G:/OutPath"
// "G:/BeamdogLibrary/00829/data/nwn_base.key" "G:/BeamdogLibrary/00829" "G:/OutPath"
int main(int argc, char** argv)
{
    ASSERT(argc == 4);
    return KeyBifExtractorExample(argv[1], argv[2], argv[3]);
}
