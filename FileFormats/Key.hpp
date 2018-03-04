#pragma once

// This file provides read-only access to KEY data.
// In the FileFormats::Key::Raw namespace is located Key, which wraps the raw data structure.
// In the FileFormats::Key::Friendly namespace is located Key, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your KEY file into memory.
// Step 2: Construct a Key as such: FileFormats::Key::Raw::Key::ReadFromBytes(bytes);
// Step 3: If user friendly access is desired, construct a Key from FileFormats::Key::Friendly::Key(rawKey).
// - Referenced BIFs can be accessed by .GetReferencedBifs().
// - Referenced resources can be accessed by .GetReferencedResources(). They are bucketed as such - resources[resref][type] -> id.
// - Refer to Example_Key.cpp if the usage is unclear.
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_KeyBIF_Format.pdf?api=v2

#include "FileFormats/Key/Key_Raw.hpp"
#include "FileFormats/Key/Key_Friendly.hpp"
