#pragma once

// This file describes the supported types of resources.
// The data in this file (and the matching .cpp) is auto generated based on the contents
// of tables 1.3.1 and 1.3.2 of the Key/BIF spec document. Refer to it for further information.
// https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_KeyBIF_Format.pdf?api=v2

namespace FileFormats::Resource {

enum class ResourceType
{
    // Invalid resource type
    INVALID = 0xFFFF, // N/A

    // Windows BMP file
    BMP = 1, // binary

    // TGA image format
    TGA = 3, // binary

    // WAV sound file
    WAV = 4, // binary

    // Bioware Packed Layered Texture, used for player character skins, allows for multiple color layers
    PLT = 6, // binary

    // Windows INI file format
    INI = 7, // textini

    // Text file
    TXT = 10, // text

    // Aurora model
    MDL = 2002, // mdl

    // NWScript Source
    NSS = 2009, // text

    // NWScript Compiled Script
    NCS = 2010, // binary

    // BioWare Aurora Engine Area file. Contains information on what tiles are located in an area, as well
    // as other static area properties that cannot change via scripting. For each .are file in a .mod,
    // there must also be a corresponding .git and .gic file having the same ResRef.
    ARE = 2012, // gff

    // BioWare Aurora Engine Tileset
    SET = 2013, // textini

    // Module Info File. See the IFO Format document.
    IFO = 2014, // gff

    // Character/Creature
    BIC = 2015, // gff

    // Walkmesh
    WOK = 2016, // mdl

    // 2-D Array
    TWODA = 2017, // text

    // Extra Texture Info
    TXI = 2022, // text

    // Game Instance File. Contains information for all object instances in an area, and all area properties
    // that can change via scripting.
    GIT = 2023, // gff

    // Item Blueprint
    UTI = 2025, // gff

    // Creature Blueprint
    UTC = 2027, // gff

    // Conversation File
    DLG = 2029, // gff

    // Tile/Blueprint Palette File
    ITP = 2030, // gff

    // Trigger Blueprint
    UTT = 2032, // gff

    // Compressed texture file
    DDS = 2033, // binary

    // Sound Blueprint
    UTS = 2035, // gff

    // Letter-combo probability info for name generation
    LTR = 2036, // binary

    // Generic File Format. Used when undesirable to create a new file extension for a resource, but the resource
    // is a GFF. (Examples of GFFs include itp, utc, uti, ifo, are, git)
    GFF = 2037, // gff

    // Faction File
    FAC = 2038, // gff

    // Encounter Blueprint
    UTE = 2040, // gff

    // Door Blueprint
    UTD = 2042, // gff

    // Placeable Object Blueprint
    UTP = 2044, // gff

    // Default Values file. Used by area properties dialog
    DFT = 2045, // textini

    // Game Instance Comments. Comments on instances are not used by the game, only the toolset, so they are stored
    // in a gic instead of in the git with the other instance properties.
    GIC = 2046, // gff

    // Graphical User Interface layout used by game
    GUI = 2047, // gff

    // Store/Merchant Blueprint
    UTM = 2051, // gff

    // Door walkmesh
    DWK = 2052, // mdl

    // Placeable Object walkmesh
    PWK = 2053, // mdl

    // Journal File
    JRL = 2056, // gff

    // Waypoint Blueprint. See Waypoint GFF document.
    UTW = 2058, // gff

    // Sound Set File. See Sound Set File Format document
    SSF = 2060, // binary

    // Script Debugger File
    NDB = 2064, // binary

    // Plot Manager file/Plot Instance
    PTM = 2065, // gff

    // Plot Wizard Blueprint
    PTT = 2066 // gff
};

enum class ResourceContentType
{
    // Binary file format. Details vary widely as to implementation
    Binary,

    // Plain text file. For some text resources, it doesn't matter whether lines are terminated by CR + LF or just
    // CR characters, but for other text resources, it might matter. To avoid complications, always use CR + LF line
    // terminators because that at least will work in all cases.
    Text,

    // Windows INI file format. Special case of a text file.
    TextIni,

    // BioWare Generic File Format. See the Generic File Format document.
    Gff,

    // BioWare Aurora model file format. Can be plain text or binary.
    Mdl
};

ResourceContentType ResourceContentTypeFromResourceType(ResourceType res);
ResourceType ResourceTypeFromString(char const* str);
char const* StringFromResourceType(ResourceType res);

}
