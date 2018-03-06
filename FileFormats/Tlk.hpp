#pragma once

// This file provides read-only access to TLK data.
// In the FileFormats::Tlk::Raw namespace is located Tlk, which wraps the raw data structure.
// In the FileFormats::Tlk::Friendly namespace is located Tlk, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your TFF file into memory.
// Step 2: Construct a Tlk as such: FileFormats::Tlk::Raw::Tlk::ReadFromBytes(bytes);
// Step 3: If user friendly access to fields is desired, construct a Tlk from FileFormats::Tlk::Friendly::Tlk(rawTlk).
// - You can get entries via operator[].
// - Use begin/end() (or ranged-based loop) to iterate all entries.
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_TalkTable_Format.pdf?api=v2

#include "FileFormats/Tlk/Tlk_Raw.hpp"
#include "FileFormats/Tlk/Tlk_Friendly.hpp"
