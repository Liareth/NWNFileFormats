#pragma once

// This file provides read-only access to GFF data.
// In the FileFormats::Raw namespace is located Gff, which wraps the raw data structure.
// In the FileFormats::Friendly namespace is located Gff, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your GFF file into memory.
// Step 2: Construct a Gff as such: FileFormats::Raw::Gff::ReadFromBytes(bytes);
// - You can browse the loaded field format and extract fields using the ConstructX functions.
// Step 3: If user friendly access to fields in desired, construct a Gff from FileFormats::Friendly::Gff(rawGff).
// - You can access the top level struct with GetTopLevelStruct().
// - You can access fields with GetTopLevelStruct().ReadField<Type_CExoString>("FIELD_NAME").

#include "FileFormats/Gff/Gff_Raw.hpp"
#include "FileFormats/Gff/Gff_Friendly.hpp"
