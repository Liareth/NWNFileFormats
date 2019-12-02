#include "FileFormats/Erf.hpp"
#include "Utility/Assert.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <vector>

namespace
{

int pack_erf(const char* out_path, const std::vector<const char*>& files)
{
    using namespace FileFormats::Erf;

    Friendly::Erf erf;

    Raw::ErfLocalisedString desc;
    desc.m_LanguageId = 0;
    desc.m_String = "ERFPacked\nhttps://github.com/Liareth/NWNFileFormats\nThis ERF was packed with the pack_erf tool from the NWNFileFormats library.";
    erf.GetDescriptions().emplace_back(std::move(desc));

    for (const char* file : files)
    {
        std::filesystem::path file_path(file);

        std::uintmax_t len = std::filesystem::file_size(file_path);
        std::unique_ptr<OwningDataBlock> db = std::make_unique<OwningDataBlock>();
        db->m_Data.resize(len);

        FILE* f = std::fopen(file, "rb");
        ASSERT(f);

        if (f)
        {
            std::fread(db->m_Data.data(), len, 1, f);
            std::fclose(f);
        }
        else
        {
            continue;
        }

        Friendly::ErfResource res;
        res.m_ResRef = file_path.stem().string();
        res.m_ResType = FileFormats::Resource::ResourceTypeFromString(file_path.extension().string().substr(1).c_str());
        res.m_DataBlock = std::move(db);
        erf.GetResources().emplace_back(std::move(res));
    }

    std::string ext = std::filesystem::path(out_path).extension().string().substr(1);
    std::transform(std::begin(ext), std::end(ext), std::begin(ext), ::toupper);
    std::memcpy(erf.GetFileType(), ext.c_str(), 3);
    erf.GetFileType()[3] = ' ';

    return !erf.WriteToFile(out_path);
}

}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::printf("erf_extractor [out_file] [files...]\n");
        return 1;
    }

    std::vector<const char*> paths;

    for (int i = 2; i < argc; ++i)
    {
        paths.emplace_back(argv[i]);
    }

    return pack_erf(argv[1], paths);
}
