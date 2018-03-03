#pragma once

// This file provides read-only access to ERF data.
// In the FileFormats::Erf::Raw namespace is located Erf, which wraps the raw data structure.
// In the FileFormats::Erf::Friendly namespace is located Erf, which exposes a much more user friendly structure.
//
// How to use:
//
// Step 1: Load your ERF file into memory.
// Step 2: Construct a Erf as such: FileFormats::Erf::Raw::Erf::ReadFromBytes(bytes, totalBytes);
// Step 3: If user friendly access is desired, construct a Erf from FileFormats::Erf::Friendly::Erf(rawErf).
// - Localised descriptions can be accessed by .GetDescriptions().
// - Resources can be accessed by .GetResources(). They are bucketed as such - resources[resref][type].
// - Refer to Example_Erf.cpp if the usage is unclear.
//
// For further information refer to https://wiki.neverwintervault.org/pages/viewpage.action?pageId=327727
// Specifically, https://wiki.neverwintervault.org/download/attachments/327727/Bioware_Aurora_ERF_Format.pdf?api=v2

#include "FileFormats/Erf/Erf_Raw.hpp"
#include "FileFormats/Erf/Erf_Friendly.hpp"
