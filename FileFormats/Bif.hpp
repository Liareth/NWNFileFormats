#pragma once

// This file provides read-only access to BIF data.
// In the FileFormats::Bif::Raw namespace is located Bif, which wraps the raw data structure.
// In the FileFormats::Bif::Friendly namespace is located Bif, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your BIF file into memory.
// Step 2: Construct a Bif as such: FileFormats::Bif::Raw::Bif::ReadFromBytes(bytes, totalBytes);
// Step 3: If user friendly access is desired, construct a Bif from FileFormats::Bif::Friendly::Bif(rawBif).
// - The resources can be accessed with .GetResources(). They are bucketed as such - resources[id] -> type / data.
// - Note that we ignore the fixed resource table in the friendly implementation.
// - Refer to Example_Bif.cpp if the usage is unclear.
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_KeyBIF_Format.pdf?api=v2

#include "FileFormats/Bif/Bif_Raw.hpp"
#include "FileFormats/Bif/Bif_Friendly.hpp"
