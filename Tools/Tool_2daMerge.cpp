#include "FileFormats/2da.hpp"
#include "Utility/Assert.hpp"

namespace {

int TwoDAMerge(char* base, char* other, char* out)
{
    using namespace FileFormats::TwoDA;

    Raw::TwoDA baseTwoDARaw;

    if (!Raw::TwoDA::ReadFromFile(base, &baseTwoDARaw))
    {
        std::printf("Failed to load base 2da from %s.\n", base);
        return 1;
    }

    Friendly::TwoDA baseTwoDA(std::move(baseTwoDARaw));

    // Iterate over the base 2da and warn if any rows are misnumbered.

    {
        std::uint32_t i = 0;
        for (const Friendly::TwoDARow& row : baseTwoDA)
        {
            if (row.RowId() != i++)
            {
                std::printf("Warning: Row %u with ID %u - be careful - the row IDs may be off in this file!\n", i, row.RowId());
                break;
            }
        }
    }

    Raw::TwoDA otherTwoDARaw;

    if (!Raw::TwoDA::ReadFromFile(other, &otherTwoDARaw))
    {
        std::printf("Failed to load other 2da from %s.\n", other);
        return 1;
    }

    Friendly::TwoDA otherTwoDA(std::move(otherTwoDARaw));

    ASSERT(baseTwoDA.GetColumnNames().size() == otherTwoDA.GetColumnNames().size());

    std::vector<const Friendly::TwoDARow*> needToMerge;

    // Write over anything we've changed.
    for (const Friendly::TwoDARow& row : otherTwoDA)
    {
        std::uint32_t rowId = row.RowId();
        if (static_cast<int>(rowId) < std::end(baseTwoDA) - std::begin(baseTwoDA))
        {
            auto iterIntoBase = std::begin(baseTwoDA) + rowId;
            for (std::size_t i = 0; i < iterIntoBase->Size(); ++i)
            {
                (*iterIntoBase)[i] = row[i];
            }
        }
        else
        {
            needToMerge.emplace_back(&row);
        }
    }

    // Insert anything we've added.
    for (const Friendly::TwoDARow* row : needToMerge)
    {
        Friendly::TwoDARow& baseRow = baseTwoDA[row->RowId()];
        for (std::size_t i = 0; i < row->Size(); ++i)
        {
            baseRow[i] = (*row)[i];
        }
    }

    // Then save the 2da back out next to the original.
    if (!baseTwoDA.WriteToFile(out))
    {
        std::printf("Failed to save merged 2da to %s.\n", out);
        return 1;
    }

    return 0;
}

}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::printf("2da_merge [base2dapath] [other2dapath] [out2dapath]\n");
        return 1;
    }

    return TwoDAMerge(argv[1], argv[2], argv[3]);
}
