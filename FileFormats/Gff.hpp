#pragma once

// This file provides read-only access to GFF data.
// In the FileFormats::Gff::Raw namespace is located Gff, which wraps the raw data structure.
// In the FileFormats::Gff::Friendly namespace is located Gff, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your GFF file into memory.
// Step 2: Construct a Gff as such: FileFormats::Gff::Raw::Gff::ReadFromBytes(bytes);
// - You can browse the loaded field format and extract fields using the ConstructX functions.
// Step 3: If user friendly access to fields is desired, construct a Gff from FileFormats::Gff::Friendly::Gff(rawGff).
// - You can access the top level struct with GetTopLevelStruct().
// - You can access fields with GetTopLevelStruct().ReadField<Type_CExoString>("FIELD_NAME").
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_GFF_Format.pdf?api=v2

#include "FileFormats/Gff/Gff_Raw.hpp"
#include "FileFormats/Gff/Gff_Friendly.hpp"
