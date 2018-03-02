#include "FileFormats/Gff.hpp"
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

}

int main(int argc, char** argv)
{
    ASSERT(argc == 2);

    std::vector<std::byte> gffData;
    bool file = ReadAllButes(argv[1], &gffData);

    if (!file)
    {
        std::printf( "Failed to open file %s.\n", argv[1]);
        return 1;
    }

    FileFormats::Gff::Raw::Gff rawGff;
    bool loaded = FileFormats::Gff::Raw::Gff::ReadFromBytes(gffData.data(), &rawGff);

    std::printf("GFF FileType: %.4s\n", rawGff.m_Header.m_FileType);
    std::printf("GFF FileVersion: %.4s\n", rawGff.m_Header.m_FileVersion);

    if (!loaded)
    {
        std::printf("Failed to load the GFF file. Check the FileType and FileVersion and ensure the file is well formed.\n");
        return 1;
    }

    FileFormats::Gff::Friendly::Gff gff(rawGff);

    using namespace FileFormats::Gff::Friendly;

    Type_CExoString tag;
    if (gff.GetTopLevelStruct().ReadField("Tag", &tag))
    {
        std::printf("\nTag: %s\n", tag.m_String.c_str());
    }

    Type_List varTable;
    if (gff.GetTopLevelStruct().ReadField("VarTable", &varTable))
    {
        std::vector<Type_Struct> const& entries = varTable.GetStructs();

        for (std::size_t i = 0; i < entries.size(); ++i)
        {
            GffStruct const& entry = entries[i];
            std::printf("\nVariable #%llu\n", i);

            Type_CExoString name;
            entry.ReadField("Name", &name);
            std::printf(" Name: %s\n", name.m_String.c_str());

            Type_DWORD type;
            entry.ReadField("Type", &type);
            std::printf(" Type: %u\n", type);

            if (type == 1)
            {
                Type_INT value;
                entry.ReadField("Value", &value);
                std::printf(" Value: %i\n", value);
            }
            else if (type == 2)
            {
                Type_FLOAT value;
                entry.ReadField("Value", &value);
                std::printf(" Value: %f\n", value);
            }
            else if (type == 3)
            {
                Type_CExoString value;
                entry.ReadField("Value", &value);
                std::printf(" Value: %s\n", value.m_String.c_str());
            }
            else if (type == 4)
            {
                Type_DWORD value;
                entry.ReadField("Value", &value);
                std::printf(" Value: %x\n", value);
            }
            else if (type == 5)
            {
                std::printf(" Value: <location>\n");
            }
            else
            {
                std::printf(" Value: <unknown>\n");
                ASSERT_FAIL();
            }
        }
    }

    std::printf("\nGFF dumped\n");
}