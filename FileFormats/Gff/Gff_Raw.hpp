#pragma once

#include <cstdint>
#include <vector>

namespace FileFormats::Gff::Raw {

// Refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_GFF_Format.pdf?api=v2
// Any references to sections in code comments below will refer to this file.
//
// Note that while these structures are the 'raw types', in some cases I have substituted the <size> <contents>
// pattern with std::vector<> or std::string and I have removed the extra size member. This is to avoid the
// manual memory management requirements, e.g. so we don't have to allocate and deallocate a char array ourselves.

struct GffHeader
{
    // The GFF header contains a number of values, all of them DWORDs (32-bit unsigned integers). The
    // header contains offset information for all the other sections in the GFF file.
    // Values in the header are as follows, and arranged in the order listed :

    char m_FileType[4]; // 4-char file type string
    char m_FileVersion[4]; // 4-char GFF Version. At the time of writing, the version is "V3.2"

    std::uint32_t m_StructOffset; // Offset of Struct array as bytes from the beginning of the file
    std::uint32_t m_StructCount; // Number of elements in Struct array
    std::uint32_t m_FieldOffset; // Offset of Field array as bytes from the beginning of the file
    std::uint32_t m_FieldCount; // Number of elements in Field array
    std::uint32_t m_LabelOffset; // Offset of Label array as bytes from the beginning of the file
    std::uint32_t m_LabelCount; // Number of elements in Label array
    std::uint32_t m_FieldDataOffset; // Offset of Field Data as bytes from the beginning of the file
    std::uint32_t m_FieldDataCount; // Number of bytes in Field Data block
    std::uint32_t m_FieldIndicesOffset; // Offset of Field Indices array as bytes from the beginning of the file
    std::uint32_t m_FieldIndicesCount; // Number of bytes in Field Indices array
    std::uint32_t m_ListIndicesOffset; // Offset of List Indices array as bytes from the beginning of the file
    std::uint32_t m_ListIndicesCount; // Number of bytes in List Indices array

    // The FileVersion should always be "V3.2" for all GFF files that use the Generic File Format as
    // described in this document. If the FileVersion is different, then the application should abort reading the
    // GFF file.

    // The FileType is a programmer-defined 4-byte character string that identifies the content-type of the
    // GFF file. By convention, it is a 3-letter file extension in all-caps, followed by a space. For example,
    // "DLG ", "ITP ", etc. When opening a GFF file, the application should check the FileType to make
    // sure that the file being opened is of the expected type
};

struct GffStruct
{
    // In a GFF file, Struct Fields are stored differently from other fields. Whereas most Fields are stored in
    // the Field array, Structs are stored in the Struct Array.

    // The very first element in the Struct Array is the Top-Level Struct for the GFF file, and it "contains" all
    // the other Fields, Structs, and Lists. In this sense, the word "contain" refers to conceptual containment
    // (as in Section 2.1) rather than physical containment (as in Section 3). In other words, it does not imply
    // that all the other Fields are physically located inside the Top-Level Struct on disk (in fact, all Structs
    // have the same physical size on disk).

    // Since the Top-Level Struct is always present, every GFF file contains at least one element in the Struct
    // Array.

    // The Struct Array looks like this:

    // Programmer-defined integer ID
    std::uint32_t m_Type;

    // If Struct.FieldCount = 1, this is an index into the Field Array.
    // If Struct.FieldCount > 1, this is a byte offset into the Field Indices
    // array, where there is an array of DWORDs having a number of
    // elements equal to Struct.FieldCount. Each one of these DWORDs
    // is an index into the Field Array.
    std::uint32_t m_DataOrDataOffset;

    // Number of fields in this Struct.
    std::uint32_t m_FieldCount;
};

struct GffList
{
    // The List Indices Array contains a sequence of List elements packed end-to-end.
    // A List is an array of Structs, and being array, its length is variable. The format of a List is as shown
    // below:
    // std::uint32_t m_Size;
    // std::uint32_t m_Elements[m_Size];
    std::vector<std::uint32_t> m_Elements;
};

struct GffField
{
    // The Field Array contains all the Fields in the GFF file except for the Top-Level Struct.
    // Each Field contains the values listed in the table below. All of the values are DWORDs.

    enum Type : std::uint32_t
    {
        // The Field Type specifies what data type the Field stores (recall the data types from Section 2.2). The
        // following table lists the values for each Field type. A datatype is considered complex if it would not fit
        // within a DWORD (4 bytes).

        // NOTE: We define u8/u16/etc which aren't technically part of the standard but make it much easier to understand
        // what each type actually is.
        u8, BYTE = u8,
        CHAR,
        u16, WORD = u16,
        i16, SHORT = i16,
        u32, DWORD = u32,
        i32, INT = i32,

        // A DWORD64 is a 64-bit (8-byte) unsigned integer. As with all integer values in GFF, the least
        // significant byte comes first, and the most significant byte is last.
        u64, DWORD64 = u64, // See *

        // An INT64 is a 64-bit (8-byte) signed integer. As with all integer values in GFF, the least significant
        // byte comes first, and the most significant byte is last.
        i64, INT64 = i64, // See *

        f32, FLOAT = f32,

        // A DOUBLE is a double-precision floating point value, and takes up 8 bytes. It is stored in little-endian
        // byte order, with the least significant byte first.
        // (Both the FLOAT and DOUBLE data types conform to IEEE Standard 754-1985).
        f64, DOUBLE = f64, // See *

        // Refer to Type_CExoString below.
        CExoString, // See *

        // Refer to Type_CResRef below.
        ResRef, // See *

        // Refer to Type_CExoLocString below.
        CExoLocString, // See *

        // Refer to Type_Void below.
        VOID, Binary = VOID, // See *

        // Refer to Type_Struct below.
        Struct, // See **

        // Refer to Type_List below.
        List // See ***
    };

    using Type_BYTE = std::uint8_t;
    using Type_CHAR = char;
    using Type_WORD = std::uint16_t;
    using Type_SHORT = std::int16_t;
    using Type_DWORD = std::uint32_t;
    using Type_INT = std::int32_t;
    using Type_DWORD64 = std::uint64_t;
    using Type_INT64 = std::int64_t;
    using Type_FLOAT = float;
    using Type_DOUBLE = double;

    struct Type_CExoString
    {
        // A CExoString is a simple character string datatype. The figure below shows the layout of a CExoString:
        // std::uint32_t m_Size;
        // char m_String[m_Size];
        std::string m_String;

        // A CExoString begins with a single DWORD (4-byte unsigned integer) which stores the string's Size. It
        // specifies how many characters are in the string. This character-count does not include a null terminator.
        // If we let N equal the number stored in Size, then the next N bytes after the Size are the characters that
        // make up the string. There is no null terminator.
    };

    struct Type_CResRef
    {
        // A CResRef is used to store the name of a file used by the game or toolset. These files may be located in
        // the BIF files in the user's data folder, inside an Encapsulated Resource File (ERF, MOD, or HAK), or in
        // the user's override folder. For efficiency and to reduce network bandwidth, A ResRef can only have up
        // to 16 characters and is not null-terminated. ResRefs are also non-case-sensitive and stored in all-lowercase.
        // The diagram below shows the structure of a CResRef stored in a GFF:
        std::uint8_t m_Size;
        char m_String[16];

        // The first byte is a Size, an unsigned value specifying the number of characters to follow. The Size is 16
        // at most. The character string contains no null terminator.
    };

    struct Type_CExoLocString
    {
        // A CExoLocString is a localized string. It can contain 0 or more CExoStrings, each one for a different
        // language and possibly gender. For a list of language IDs, see Table 2.2b.
        // The figure below shows the layout of a CExoLocString

        struct SubString
        {
            // A LocString SubString has almost the same format as a CExoString, but includes an additional String
            // ID at the beginning
            std::int32_t m_StringID;
            Type_CExoString m_String;

            // The StringID stored in a GFF file does not match up exactly to the LanguageIDs shown in Table 2.2b.
            // Instead, it is 2 times the Language ID, plus the Gender (0 for neutral or masculine, 1 for feminine).
            // If we let N equal the number stored in StringLength, then the N bytes after the StringLength are the
            // characters that make up the string. There is no null terminator.
        };

        // A CExoLocString begins with a single DWORD (4-byte unsigned integer) which stores the total
        // number of bytes in the CExoLocString, not including the first 4 size bytes.

        // The next 4 bytes are a DWORD containing the StringRef of the LocString. The StringRef is an index
        // into the user's dialog.tlk file, which contains a list of almost all the localized text in the game and
        // toolset. If the StringRef is -1 (ie., 0xFFFFFFFF), then the LocString does not reference dialog.tlk at all.

        // The 4 bytes after the StringRef comprise the StringCount, a DWORD that specifies how many
        // SubStrings the LocString contains. The remainder of the LocString is a list of SubStrings.

        std::uint32_t m_TotalSize;
        std::uint32_t m_StringRef;
        // std::uint32_t m_StringCount;
        // SubString m_SubStrings[m_StringCount];
        std::vector<SubString> m_SubStrings;
    };

    struct Type_Void
    {
        // Void data is an arbitrary sequence of bytes to be interpreted by the application in a programmer-defined
        // fashion. The format is shown below:
        // std::uint32_t m_Size;
        // std::byte m_Data[m_Size];
        std::vector<std::byte> m_Data;

        // Size is a DWORD containing the number of bytes of data. The data itself is contained in the N bytes
        // that follow, where N is equal to the Size value.
    };

    // Unlike most of the complex Field data types, a Struct Field's data is located not in the Field Data Block,
    // but in the Struct Array.
    // Normally, a Field's DataOrDataOffset value would be a byte offset into the Field Data Block, but for a
    // Struct, it is an index into the Struct Array.
    // For information on the layout of a Struct, see Section 3.3, with particular attention to Table 3.3.
    using Type_Struct = GffStruct;

    // Unlike most of the complex Field data types, a List Field's data is located not in the Field Data Block,
    // but in the Field Indices Array.
    // The starting address of a List is specified in its Field's DataOrDataOffset value as a byte offset into the
    // Field Indices Array, at which is located a List element. Section 3.8 describes the structure a List
    // element.
    using Type_List = GffList;

    // Data type
    Type m_Type;

    // Index into the Label Array
    std::uint32_t m_LabelIndex;

    // If Field.Type is a simple data type (see table below), then this is
    // the value actual of the field.
    // If Field.Type is a complex data type (see table below), then this is
    // a byte offset into the Field Data block.
    std::uint32_t m_DataOrDataOffset;

    // Non-complex Field data is contained directly within the Field itself, in the DataOrDataOffset member.

    // If the data type is smaller than a DWORD, then the first bytes of the DataOrDataOffset DWORD, up to
    // the size of the data type itself, contain the data value.

    // * If the Field data is complex, then the DataOrDataOffset value is equal to a byte offset from the
    // beginning of the Field Data Block, pointing to the raw bytes that represent the complex data. The exact
    // method of fetching the complex data depends on the actual Field Type, and is described in Section 4.

    // ** As a special case, if the Field Type is a Struct, then DataOrDataOffset is an index into the Struct Array
    // instead of a byte offset into the Field Data Block.

    // *** As another special case, if the Field Type is a List, then DataOrDataOffset is a byte offset from the
    // beginning of the List Indices Array, where there is a DWORD for the size of the array followed by an
    // array of DWORDs. The elements of the array are offsets into the Struct Array. See Section 4.9 for
    // details.
};

struct GffLabel
{
    // A Label is a 16-CHAR array. Unused characters are nulls, but the label itself is non-null-terminated, so
    // a 16-character label would use up all 16 CHARs with no null at the end.

    // The Label Array is a list of all the Labels used in a GFF file.

    // Note that a single Label may be referenced by more than one Field. When multiple Fields have Labels
    // with the exact same text, they share the same Label element instead of each having their own copy.
    // This sharing occurs regardless of what Struct the Field belongs to. All Labels in the Label Array should be
    // unique.

    // Also, the Fields belonging to a Struct must all use different Labels. It is permissible, however, for Fields
    // in two different Structs to use the same Label, regardless of whether one of those Structs is conceptually
    // contained inside the other Struct.
    char m_Label[16];
};

// The Field Data block contains raw data for any Fields that have a complex Field Type, as described in
// Section 3.4. The two exceptions to this rule are Struct and List Fields, which are not stored in the Field
// Data Block.

// The FieldDataCount in the GFF header specifies the number of BYTEs contained in the Field Data
// block.

// The data in the Field Data Block is laid out according to the type of Field that owns each byte of data.
// See Section 4 for details
using GffFieldData = std::byte;

// A Field Index is a DWORD containing the index of the associated Field within the Field array.
// The Field Indices Array is an array of such DWORDs
using GffFieldIndex = std::uint32_t;

//The List Indices Array contains a sequence of List elements packed end - to - end.
// A List is an array of Structs, and being array, its length is variable.
// The first DWORD is the Size of the List, and it specifies how many Struct elements the List contains.
// There are Size DWORDS after that, each one an index into the Struct Array.
using GffListIndex = std::byte;

struct Gff
{
public:
    GffHeader m_Header;
    std::vector<GffStruct> m_Structs;
    std::vector<GffField> m_Fields;
    std::vector<GffLabel> m_Labels;
    std::vector<GffFieldData> m_FieldData;
    std::vector<GffFieldIndex> m_FieldIndices;
    std::vector<GffListIndex> m_ListIndices;

    static bool ReadFromBytes(std::byte const* bytes, Gff* out);

    // Below are functions to construct a type from the provided field.
    GffField::Type_BYTE ConstructBYTE(GffField const& field) const;
    GffField::Type_CHAR ConstructCHAR(GffField const& field) const;
    GffField::Type_WORD ConstructWORD(GffField const& field) const;
    GffField::Type_SHORT ConstructSHORT(GffField const& field) const;
    GffField::Type_DWORD ConstructDWORD(GffField const& field) const;
    GffField::Type_INT ConstructINT(GffField const& field) const;
    GffField::Type_DWORD64 ConstructDWORD64(GffField const& field) const;
    GffField::Type_INT64 ConstructINT64(GffField const& field) const;
    GffField::Type_FLOAT ConstructFLOAT(GffField const& field) const;
    GffField::Type_DOUBLE ConstructDOUBLE(GffField const& field) const;
    GffField::Type_CExoString ConstructCExoString(GffField const& field) const;
    GffField::Type_CResRef ConstructResRef(GffField const& field) const;
    GffField::Type_CExoLocString ConstructCExoLocString(GffField const& field) const;
    GffField::Type_Void ConstructVOID(GffField const& field) const;
    GffField::Type_Struct ConstructStruct(GffField const& field) const;
    GffField::Type_List ConstructList(GffField const& field) const;

private:
    void ReadStructs(std::byte const* data);
    void ReadFields(std::byte const* data);
    void ReadLabels(std::byte const* data);
    void ReadFieldData(std::byte const* data);
    void ReadFieldIndices(std::byte const* data);
    void ReadLists(std::byte const* data);
};

}
