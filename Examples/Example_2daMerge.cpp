#include "FileFormats/2da.hpp"
#include "Utility/Assert.hpp"

namespace {

int TwoDAMerge(char* base, char* other, char* out)
{
    using namespace FileFormats::TwoDA;

    Raw::TwoDA baseTwoDARaw;
    Raw::TwoDA::ReadFromFile(base, &baseTwoDARaw);
    Friendly::TwoDA baseTwoDA(std::move(baseTwoDARaw));

    Raw::TwoDA otherTwoDARaw;
    Raw::TwoDA::ReadFromFile(other, &otherTwoDARaw);
    Friendly::TwoDA otherTwoDA(std::move(otherTwoDARaw));

    ASSERT(baseTwoDA.GetColumnNames().size() == otherTwoDA.GetColumnNames().size());

    std::vector<const Friendly::TwoDARow*> needToMerge;

    // Write over anything we've changed.
    for (const Friendly::TwoDARow& row : otherTwoDA)
    {
        std::uint32_t rowId = row.RowId();
        if (rowId < std::end(baseTwoDA) - std::begin(baseTwoDA))
        {
            auto iterIntoBase = std::begin(baseTwoDA) + rowId;
            for (std::size_t i = 0; i < iterIntoBase->size(); ++i)
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
        for (std::size_t i = 0; i < row->size(); ++i)
        {
            baseRow[i] = (*row)[i];
        }
    }

    // Then save the 2da back out next to the original.
    baseTwoDA.WriteToPath(out);
    return 0;
}

}

int main(int argc, char** argv)
{
    ASSERT(argc == 4);
    return TwoDAMerge(argv[1], argv[2], argv[3]);
}
