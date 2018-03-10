#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace FileFormats::TwoDA::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_2DA_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.

// NOTE: A 2da is a bit different to most other file types. This is not a binary file format - this is a text formats.
// Therefore, the raw structure will just contain the lines belonging to the file in whitespace-delimited fashion.

// A 2da file is a plain-text file that describes a 2-dimensional array of data.
//
// In BioWare's games, 2da files serve many purposes, and are often crucial to the proper functioning of
// the game and tools. They describe many aspects of the rules and game engine.
//
// Although 2da files are plain text files, they are structured according to a set of rules that must be
// followed in order for the game and tools to read them correctly.

// Data types
// There are three types of data that may be present in a 2da. All data under a given column must be of the
// same type. The data types are:
//
// · String: a string can be any arbitrary sequence of characters. However, if the string contains spaces,
// then it must be enclosed by quotation mark characters (") because otherwise, the text after the space
// will be considered to be belong to the next column. The string itself can never contain a quotation
// mark.
// · Integer: an integer can be up to 32-bits in size, although the application reading the integer entry is
// free to assume that the value is actually of a smaller type. For example, boolean values are stored in
// a 2da as integers, so the column for a boolean property should only contain 0s or 1s.
// · Float: a 32-bit floating point value.
//
// The 2da format does not include data type information for each column because the application that
// reads the data from the 2da already knows what datatype to assume each column contains.
//
// Blank (****) entries
// The special character sequence **** indicates that an entry contains no data, or the value is not
// applicable. Note that this character sequence contains exactly 4 asterisk characters, no more and no less.
// When deleting a row from a 2da file, all columns in that row should be filled with ****s.
//
// The **** value is also used to indicate "N/A".
//
// An attempt to read a String from a **** entry should return an empty string (""). An attempt to read an
// Integer or Float should return 0. The programming function that performed the reading operation should
// indicate that the read attempt failed so that that application knows that the entry value is no ordinary ""
// or 0.

using TwoDAToken = std::string;

struct TwoDALine
{
    // Whitespace separating columns
    // Each column is separated by one or more spaces. The exact number of spaces does not matter, so long
    // as there is at least one space character. The columns do not have to line up exactly, as shown by row 3
    // in the example above.
    //
    // Important: do not use tab characters to separate columns.
    //
    // First column
    // The first column always contains the row number, with the first row being numbered 0, and all
    // subsequent rows incrementing upward from there.
    //
    // The first column is the only column that does not have a heading.
    //
    // Note that the numbering in the first column is for the convenience of the person reading or editing the
    // 2da file. The game and tools automatically keep track of the index of each row, so if a row is numbered
    // incorrectly, the game and tools will still use the correct number for the row index. Nevertheless, it is a
    // good habit to make sure that rows are numbered correctly to avoid confusion.
    //
    // Column names
    // All columns after the first one must have a heading. The heading can be in upper or lower case letters
    // and may contain underscores.

    std::vector<TwoDAToken> m_Tokens;
};

struct TwoDA
{
    // Line 1 - file format version
    // The first line of a 2da file describes the version of the 2da format followed by the 2da file. The current
    // version header at the time of this writing is: 2DA V2.0
    //
    // Line 2 - blank or optional default
    // The second line of a 2da file is usually empty.
    // Optionally, it can specify a default value for all entries in the file. The syntax is: DEFAULT: <text>
    // where <text> is the default value to use. Note that the default text is subject to the same whitespace
    // rules as any other column in a 2da. A string containing spaces must therefore be enclosed by quotation
    // marks.
    //
    // The default value will be returned when a requested row and column has no data, such as when asking
    // for data from a row that does not exist. For String requests, the default text is returned as a string. For
    // Integer or Floating point requests, the default will be converted to an Integer or Floating point value as
    // appropriate. If the default string cannot be converted to a numerical type, the return value will be 0. The
    // programming function that reads the 2da entry should indicate that the read attempt failed.
    //
    // The default value is not returned when a requested entry is ****. An entry that contains **** will return
    // a blank string or zero.
    //
    // Line 3 - column names
    // The third line of a 2da file contains the names of each column. Each column name is separated from the
    // others by one or more space characters. The exact number of spaces does not matter, so long as there is
    // at least one.
    //
    // A column name contains alphanumeric characters or underscores, and can begin with any of these
    // characters (ie., not restricted to starting with a letter).
    //
    // Lines 4 to infinity - row data
    // All lines after and including line 4 are data rows.
    //
    // Each column in a row is separated from the other columns by one or more space characters. When
    // viewing the contents of a 2da using a fixed-width font, the columns in each row do not have to visually
    // line up with the columns in the other rows, but for ease of reading, it is best to line them up anyway.
    // The very first column in a row is the row's index. The first data row (line 4) has an index of 0, the
    // second data row (line 5) has an index of 1, and so on.
    //
    // Every row must contain the exact same number of columns are there are column names given in line 3,
    // plus one (since the index column has no name).
    //
    // If the data for a column is a string that contains spaces, then the data for that column should begin with
    // a quotation mark and end with a quotation mark. Otherwise, the text after the space will be considered
    // to belong to the next column. Because of how quotation marks are handled, a string entry in a 2da can
    // never contain actually quotation marks itself

    std::vector<TwoDALine> m_Lines;

    // Constructs a 2da from a non-owning pointer. Memory usage may be high.
    static bool ReadFromBytes(std::byte const* bytes, std::size_t bytesCount, TwoDA* out);

    // Constructs a 2da from a vector of bytes which we have taken ownership of. Memory usage will be moderate.
    static bool ReadFromByteVector(std::vector<std::byte>&& bytes, TwoDA* out);

    // Constructs a 2da from a file. The file with be memory mapped so memory usage will be ideal.
    static bool ReadFromFile(char const* path, TwoDA* out);

private:
    bool ConstructInternal(std::byte const* bytes, std::size_t bytesCount);
};

}
