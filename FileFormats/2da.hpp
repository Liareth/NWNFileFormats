#pragma once

// This file provides read-only access to 2DA data.
// In the FileFormats::TwoDA::Raw namespace is located TwoDA, which wraps the raw data structure.
// In the FileFormats::TwoDA::Friendly namespace is located TwoDA, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your 2DA file into memory.
// Step 2: Construct a TwoDa as such: FileFormats::TwoDa::Raw::TwoDa::ReadFromBytes(bytes, totalBytes);
// Step 3: If user friendly access is desired, construct a TwoDa from FileFormats::TwoDa::Friendly::TwoDa(raw2Da).
// - You can access rows and columns directly: twoda[0]["LABEL"];
// - You can iterate over the collection: refer to Example_2da.cpp.
// - You can extract the string, int, or float representation with the appropriate functions.
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_2DA_Format.pdf?api=v2

#include "FileFormats/2da/2da_Raw.hpp"
#include "FileFormats/2da/2da_Friendly.hpp"
