#include "FileFormats/2da.hpp"
#include "FileFormats/Gff.hpp"

#include <format>

#if OS_WINDOWS
    #include "Windows.h"
#else
    #include <sys/stat.h>
#endif

using namespace FileFormats;

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

bool g_any_change = false;

template <typename T>
std::string DiffField(const char* fieldName, T oldVal, T newVal)
{
    if (oldVal != newVal)
    {
        g_any_change = true;
        return std::format("{}: {} -> {}\n", fieldName, oldVal, newVal);
    }

    return std::format("{}: {}\n", fieldName, oldVal);
}

template <typename T>
std::string DiffLocalVar(const Gff::Friendly::GffStruct* oldVariable, const Gff::Friendly::GffStruct* newVariable)
{
    T oldVal, newVal;
    newVariable->ReadField("Value", &newVal);

    if (oldVariable)
    {
        oldVariable->ReadField("Value", &oldVal);
    }
    else
    {
        oldVal = newVal;
    }

    return DiffField("Value", oldVal, newVal);
}

}

int DiffCreatures(const char* firstCreaturePath, const char* secondCreaturePath, const char* outputPath)
{
    Gff::Raw::Gff firstCreatureGffRaw;

    if (!Gff::Raw::Gff::ReadFromFile(firstCreaturePath, &firstCreatureGffRaw))
    {
        std::printf("Failed to load gff from %s.\n", firstCreaturePath);
        return 1;
    }

    Gff::Raw::Gff secondCreatureGffRaw;

    if (!Gff::Raw::Gff::ReadFromFile(secondCreaturePath, &secondCreatureGffRaw))
    {
        std::printf("Failed to load gff from %s.\n", secondCreaturePath);
        return 1;
    }

    Gff::Friendly::Gff firstCreatureGff(std::move(firstCreatureGffRaw));
    Gff::Friendly::Gff secondCreatureGff(std::move(secondCreatureGffRaw));

    std::string output;

    Gff::Friendly::Type_CExoLocString name;
    secondCreatureGff.GetTopLevelStruct().ReadField("FirstName", &name);

    if (!name.m_SubStrings.empty())
    {
        output += std::format("{}\n\n", name.m_SubStrings[0].m_String);
    } 

    Gff::Friendly::Type_BYTE old_str, new_str;
    Gff::Friendly::Type_BYTE old_dex, new_dex;
    Gff::Friendly::Type_BYTE old_con, new_con;
    Gff::Friendly::Type_BYTE old_int, new_int;
    Gff::Friendly::Type_BYTE old_wis, new_wis;
    Gff::Friendly::Type_BYTE old_cha, new_cha;
    Gff::Friendly::Type_BYTE old_ac, new_ac;
    Gff::Friendly::Type_SHORT old_max_hp, new_max_hp;
    
    firstCreatureGff.GetTopLevelStruct().ReadField("Str", &old_str);
    secondCreatureGff.GetTopLevelStruct().ReadField("Str", &new_str);

    firstCreatureGff.GetTopLevelStruct().ReadField("Dex", &old_dex);
    secondCreatureGff.GetTopLevelStruct().ReadField("Dex", &new_dex);

    firstCreatureGff.GetTopLevelStruct().ReadField("Con", &old_con);
    secondCreatureGff.GetTopLevelStruct().ReadField("Con", &new_con);

    firstCreatureGff.GetTopLevelStruct().ReadField("Int", &old_int);
    secondCreatureGff.GetTopLevelStruct().ReadField("Int", &new_int);

    firstCreatureGff.GetTopLevelStruct().ReadField("Wis", &old_wis);
    secondCreatureGff.GetTopLevelStruct().ReadField("Wis", &new_wis);

    firstCreatureGff.GetTopLevelStruct().ReadField("Cha", &old_cha);
    secondCreatureGff.GetTopLevelStruct().ReadField("Cha", &new_cha);

    firstCreatureGff.GetTopLevelStruct().ReadField("NaturalAC", &old_ac);
    secondCreatureGff.GetTopLevelStruct().ReadField("NaturalAC", &new_ac);

    firstCreatureGff.GetTopLevelStruct().ReadField("MaxHitPoints", &old_max_hp);
    secondCreatureGff.GetTopLevelStruct().ReadField("MaxHitPoints", &new_max_hp);

    output += DiffField("Str", old_str, new_str);
    output += DiffField("Dex", old_dex, new_dex);
    output += DiffField("Con", old_con, new_con);
    output += DiffField("Int", old_int, new_int);
    output += DiffField("Wis", old_wis, new_wis);
    output += DiffField("Cha", old_cha, new_cha);
    output += DiffField("NaturalAC", old_ac, new_ac);
    output += DiffField("MaximumHP", old_max_hp, new_max_hp);

    output += "\n";

    Gff::Friendly::Type_List oldVariables;
    Gff::Friendly::Type_List newVariables;

    firstCreatureGff.GetTopLevelStruct().ReadField("VarTable", &oldVariables);
    secondCreatureGff.GetTopLevelStruct().ReadField("VarTable", &newVariables);

    for (const Gff::Friendly::GffStruct& variable : newVariables.GetStructs())
    {
        // This code won't cope correctly with two localvars with same name and different type.

        Gff::Friendly::Type_CExoString variableName;
        variable.ReadField("Name", &variableName);

        Gff::Friendly::Type_DWORD type;
        variable.ReadField("Type", &type);

        const Gff::Friendly::GffStruct* oldVariable = nullptr;

        // Locate the new variable in the old table to see if it's an add.
        for (const Gff::Friendly::GffStruct& oldVariableCandidate : oldVariables.GetStructs())
        {
            Gff::Friendly::Type_CExoString oldVariableName;
            oldVariableCandidate.ReadField("Name", &oldVariableName);

            if (variableName.m_String == oldVariableName.m_String)
            {
                oldVariable = &oldVariableCandidate;
                break;
            }
        }

        output += std::format("{}LocalVar {} ", oldVariable ? "" : "ADDED ", variableName.m_String);

        if (type == 1)
        {
            output += DiffLocalVar<Gff::Friendly::Type_INT>(oldVariable, &variable);
        }
        else if (type == 2)
        {
            output += DiffLocalVar<Gff::Friendly::Type_FLOAT>(oldVariable, &variable);
        }
        else
        {
            output += "UNSUPPORTED TYPE\n";
        }
    }

    if (g_any_change)
    {
        RecursivelyEnsureDir(outputPath);
        FILE* f = fopen(outputPath, "w");

        if (f)
        {
            fprintf(f, "%s", output.c_str());
            fclose(f);
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::printf("diff_creature [first_gff] [second_gff] [output_path]");
        return 1;
    }

    return DiffCreatures(argv[1], argv[2], argv[3]);
}
