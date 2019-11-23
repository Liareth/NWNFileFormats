#include "FileFormats/2da.hpp"
#include "FileFormats/Gff.hpp"

#include <set>
#include <cstring>

#if OS_WINDOWS
    #include "Windows.h"
#else
    #include <sys/stat.h>
#endif

namespace {

// Recursively make the provided directory.
void RecursivelyEnsureDir(std::string const& dir)
{
    for (std::size_t slashIndex = dir.find_first_of("\\/");
        slashIndex != std::string::npos;
        slashIndex = dir.find_first_of("\\/", slashIndex + 1))
    {
        std::string dirToMake = dir.substr(0, slashIndex);

#if OS_WINDOWS
        CreateDirectoryA(dirToMake.c_str(), NULL);
#else
        mkdir(dirToMake.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }
}

}

int GeneratePlaceableBlueprints(const char* twoDAPath, const char* blueprintPath, const char* outputFolderPath, const char* labelFilter)
{
    using namespace FileFormats;

    TwoDA::Raw::TwoDA twoDARaw;
    if (!TwoDA::Raw::TwoDA::ReadFromFile(twoDAPath, &twoDARaw))
    {
        std::printf("Failed to load 2da from %s.\n", twoDAPath);
        return 1;
    }

    Gff::Raw::Gff rawGff;
    if (!Gff::Raw::Gff::ReadFromFile(blueprintPath, &rawGff))
    {
        std::printf("Failed to load the GFF file from %s.\n", blueprintPath);
        return 1;
    }

    TwoDA::Friendly::TwoDA twoDA(std::move(twoDARaw));
    Gff::Friendly::Gff gff(std::move(rawGff));

    std::string blueprintPathAsStr = blueprintPath;
    std::size_t lastDot = blueprintPathAsStr.find_last_of('.');

    std::string extension;
    if (lastDot != std::string::npos && lastDot + 1 != blueprintPathAsStr.size())
    {
        extension = blueprintPathAsStr.substr(lastDot + 1, blueprintPathAsStr.size() - lastDot - 1);
    }
    else
    {
        extension = "gff";
    }

    // For each row in the 2da, we need to grab:
    // - RowID -> maps to Appearance in the GFF
    // - Label -> maps to LocName in the GFF
    // - ModelName > maps to the filename and also to TemplateResRef in the GFF

    // Used to enforce based on ModelName that we only have one of the same name.
    std::set<std::string> uniqueFileNameSet;

    for (const TwoDA::Friendly::TwoDARow& row : twoDA)
    {
        std::uint32_t rowId = row.RowId();
        std::string label = row.AsStr("Label");
        std::string modelName = row.AsStr("ModelName");

        if (labelFilter && label.find(labelFilter) == std::string::npos)
        {
            std::printf("%s: Rejecting due to filter mismatch.\n", label.c_str());
            continue;
        }

        Gff::Friendly::Type_DWORD appearance = static_cast<Gff::Friendly::Type_DWORD>(rowId);

        Gff::Friendly::Type_CExoLocString locName;
        locName.m_StringRef = 0xFFFFFFFF;
        locName.m_TotalSize = sizeof(locName.m_StringRef) +
            sizeof(std::uint32_t); // string count

        {
            Gff::Friendly::Type_CExoLocString::SubString substring;
            substring.m_StringID = 0;
            substring.m_String = std::move(label);

            locName.m_TotalSize += sizeof(substring.m_StringID) +
                sizeof(std::uint32_t) + // string size
                static_cast<std::uint32_t>(substring.m_String.size());

            locName.m_SubStrings.emplace_back(std::move(substring));
        }

        if (modelName.size() > 14)
        {
            modelName = modelName.substr(0, 14);
        }

        if (uniqueFileNameSet.find(modelName) != std::end(uniqueFileNameSet))
        {
            std::string modifiedModelName;
            std::uint8_t i = 0;

            do
            {
                modifiedModelName = modelName + std::to_string(i++);
            } while (uniqueFileNameSet.find(modifiedModelName) != std::end(uniqueFileNameSet));

            std::printf("%s: Mapped %s -> %s\n", locName.m_SubStrings[0].m_String.c_str(), modelName.c_str(), modifiedModelName.c_str());

            modelName = modifiedModelName;
        }

        uniqueFileNameSet.insert(modelName);

        Gff::Friendly::Type_CResRef resref;
        resref.m_Size = static_cast<std::uint32_t>(modelName.size());
        std::memcpy(resref.m_String, modelName.data(), resref.m_Size);

        gff.GetTopLevelStruct().WriteField("Appearance", appearance);
        gff.GetTopLevelStruct().WriteField("LocName", locName);
        gff.GetTopLevelStruct().WriteField("TemplateResRef", resref);

        char pathBuffer[1024];
        std::sprintf(pathBuffer, "%s/%s.%s", outputFolderPath, modelName.c_str(), extension.c_str());
        RecursivelyEnsureDir(pathBuffer);

        if (gff.WriteToFile(pathBuffer))
        {
            std::printf("%s: Saved to %s.\n", locName.m_SubStrings[0].m_String.c_str(), pathBuffer);
        }
        else
        {
            std::printf("%s: ERROR: Failed to save to %s.\n", locName.m_SubStrings[0].m_String.c_str(), pathBuffer);
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::printf("key_bif_extractor [2da_path] [blueprint_base_path] [output_path] [optional_label_filter]\n");
        return 1;
    }

    return GeneratePlaceableBlueprints(argv[1], argv[2], argv[3], argc >= 4 ? argv[4] : nullptr);
}
