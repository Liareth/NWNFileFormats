#include "FileFormats/Erf.hpp"
#include "Utility/Assert.hpp"
#include <unordered_map>

#ifdef _WIN32
    #include "Windows.h"
#else
    #include <dirent.h>
#endif

namespace {

struct FileData
{
    uint32_t m_modified;
    uint32_t m_size;
};

using FileName = std::string;
using FileMap = std::unordered_map<FileName, FileData>;

struct File
{
    FileName m_name;
    FileData m_data;
};

FileMap GetAllFilesInDirectory(std::string directory)
{
    FileMap files;

#ifdef _WIN32
    // Windows needs \* appended to the search.
    directory.append("\\*");

    HANDLE findHandle;
    WIN32_FIND_DATA findData;

    findHandle = FindFirstFileA(directory.c_str(), &findData);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                FileName name = findData.cFileName;
                ASSERT(files.find(name) == files.end());

                FileData data
                {
                    findData.ftLastWriteTime.dwLowDateTime, findData.nFileSizeLow
                };

                files.insert(std::make_pair(std::move(name), std::move(data)));
            }
        } while (FindNextFileA(findHandle, &findData));
    }

    FindClose(findHandle);
#else
    DIR* dir = opendir(directory.c_str());

    if (dir != nullptr)
    {
        dirent* directoryEntry = readdir(dir);

        while (directoryEntry != nullptr)
        {
            if (directoryEntry->d_type == DT_UNKNOWN || directoryEntry->d_type == DT_REG)
            {
                FileName name = directoryEntry->d_name;
                ASSERT(files.find(name) == files.end());

                const std::string fullFilePath = CombinePaths(directory, name);
                struct stat fileStatistics;
                stat(fullFilePath.c_str(), &fileStatistics);

                FileData data
                {
                    static_cast<uint32_t>(fileStatistics.st_mtime),
                    static_cast<uint32_t>(fileStatistics.st_size)
                };

                files.insert(std::make_pair(std::move(name), std::move(data)));
            }

            directoryEntry = readdir(dir);
        }

        closedir(dir);
    }
#endif

    return files;
}

int extract_erf(const char* out_path, const char* in_path)
{
    using namespace FileFormats::Erf;

    for (auto& kvp : GetAllFilesInDirectory(in_path))
    {
        char erf_path[512];
        sprintf(erf_path, "%s/%s", in_path, kvp.first.c_str());

        Raw::Erf erf_raw;
        bool loaded = Raw::Erf::ReadFromFile(erf_path, &erf_raw);

        if (!loaded)
        {
            std::printf("Failed to open %s.", erf_path);
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
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 3);
    return extract_erf(argv[1], argv[2]);
}
