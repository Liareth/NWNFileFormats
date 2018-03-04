#include "FileFormats/Resource.hpp"
#include "Utility/Assert.hpp"

#include <string>

namespace FileFormats::Resource {

ResourceContentType ResourceContentTypeFromResourceType(ResourceType res)
{
    if (res == ResourceType::BMP) return ResourceContentType::Binary;
    else if (res == ResourceType::NCS) return ResourceContentType::Binary;
    else if (res == ResourceType::DDS) return ResourceContentType::Binary;
    else if (res == ResourceType::LTR) return ResourceContentType::Binary;
    else if (res == ResourceType::SSF) return ResourceContentType::Binary;
    else if (res == ResourceType::NDB) return ResourceContentType::Binary;
    else if (res == ResourceType::TGA) return ResourceContentType::Binary;
    else if (res == ResourceType::WAV) return ResourceContentType::Binary;
    else if (res == ResourceType::PLT) return ResourceContentType::Binary;
    else if (res == ResourceType::DAT) return ResourceContentType::Binary;
    else if (res == ResourceType::SHD) return ResourceContentType::Binary; // I think? Could be text. TODO
    else if (res == ResourceType::WBM) return ResourceContentType::Binary;
    else if (res == ResourceType::IDS) return ResourceContentType::Binary;
    else if (res == ResourceType::ERF) return ResourceContentType::Binary;
    else if (res == ResourceType::BIF) return ResourceContentType::Binary;
    else if (res == ResourceType::KEY) return ResourceContentType::Binary;
    else if (res == ResourceType::TXT) return ResourceContentType::Text;
    else if (res == ResourceType::NSS) return ResourceContentType::Text;
    else if (res == ResourceType::TWODA) return ResourceContentType::Text;
    else if (res == ResourceType::TXI) return ResourceContentType::Text;
    else if (res == ResourceType::SET) return ResourceContentType::TextIni;
    else if (res == ResourceType::DFT) return ResourceContentType::TextIni;
    else if (res == ResourceType::INI) return ResourceContentType::TextIni;
    else if (res == ResourceType::ARE) return ResourceContentType::Gff;
    else if (res == ResourceType::IFO) return ResourceContentType::Gff;
    else if (res == ResourceType::BIC) return ResourceContentType::Gff;
    else if (res == ResourceType::GIT) return ResourceContentType::Gff;
    else if (res == ResourceType::UTI) return ResourceContentType::Gff;
    else if (res == ResourceType::UTC) return ResourceContentType::Gff;
    else if (res == ResourceType::DLG) return ResourceContentType::Gff;
    else if (res == ResourceType::ITP) return ResourceContentType::Gff;
    else if (res == ResourceType::UTT) return ResourceContentType::Gff;
    else if (res == ResourceType::UTS) return ResourceContentType::Gff;
    else if (res == ResourceType::GFF) return ResourceContentType::Gff;
    else if (res == ResourceType::FAC) return ResourceContentType::Gff;
    else if (res == ResourceType::UTE) return ResourceContentType::Gff;
    else if (res == ResourceType::UTD) return ResourceContentType::Gff;
    else if (res == ResourceType::UTP) return ResourceContentType::Gff;
    else if (res == ResourceType::GIC) return ResourceContentType::Gff;
    else if (res == ResourceType::GUI) return ResourceContentType::Gff;
    else if (res == ResourceType::UTM) return ResourceContentType::Gff;
    else if (res == ResourceType::JRL) return ResourceContentType::Gff;
    else if (res == ResourceType::UTW) return ResourceContentType::Gff;
    else if (res == ResourceType::PTM) return ResourceContentType::Gff;
    else if (res == ResourceType::PTT) return ResourceContentType::Gff;
    else if (res == ResourceType::BAK) return ResourceContentType::Gff;
    else if (res == ResourceType::XBC) return ResourceContentType::Gff;
    else if (res == ResourceType::MDL) return ResourceContentType::Mdl;
    else if (res == ResourceType::WOK) return ResourceContentType::Mdl;
    else if (res == ResourceType::DWK) return ResourceContentType::Mdl;
    else if (res == ResourceType::PWK) return ResourceContentType::Mdl;

    ASSERT_FAIL_MSG("Unknown resource type.");
    return ResourceContentType::Binary;
}

ResourceType ResourceTypeFromString(char const* str)
{
#if CMP_MSVC
    #define CASE_INSENSITIVE_CMP _stricmp
#else
    #define CASE_INSENSITIVE_CMP stricmp
#endif

    if (CASE_INSENSITIVE_CMP(str, "bmp") == 0) return ResourceType::BMP;
    else if (CASE_INSENSITIVE_CMP(str, "tga") == 0) return ResourceType::TGA;
    else if (CASE_INSENSITIVE_CMP(str, "wav") == 0) return ResourceType::WAV;
    else if (CASE_INSENSITIVE_CMP(str, "plt") == 0) return ResourceType::PLT;
    else if (CASE_INSENSITIVE_CMP(str, "ini") == 0) return ResourceType::INI;
    else if (CASE_INSENSITIVE_CMP(str, "txt") == 0) return ResourceType::TXT;
    else if (CASE_INSENSITIVE_CMP(str, "mdl") == 0) return ResourceType::MDL;
    else if (CASE_INSENSITIVE_CMP(str, "nss") == 0) return ResourceType::NSS;
    else if (CASE_INSENSITIVE_CMP(str, "ncs") == 0) return ResourceType::NCS;
    else if (CASE_INSENSITIVE_CMP(str, "are") == 0) return ResourceType::ARE;
    else if (CASE_INSENSITIVE_CMP(str, "set") == 0) return ResourceType::SET;
    else if (CASE_INSENSITIVE_CMP(str, "ifo") == 0) return ResourceType::IFO;
    else if (CASE_INSENSITIVE_CMP(str, "bic") == 0) return ResourceType::BIC;
    else if (CASE_INSENSITIVE_CMP(str, "wok") == 0) return ResourceType::WOK;
    else if (CASE_INSENSITIVE_CMP(str, "2da") == 0) return ResourceType::TWODA;
    else if (CASE_INSENSITIVE_CMP(str, "txi") == 0) return ResourceType::TXI;
    else if (CASE_INSENSITIVE_CMP(str, "git") == 0) return ResourceType::GIT;
    else if (CASE_INSENSITIVE_CMP(str, "uti") == 0) return ResourceType::UTI;
    else if (CASE_INSENSITIVE_CMP(str, "utc") == 0) return ResourceType::UTC;
    else if (CASE_INSENSITIVE_CMP(str, "dlg") == 0) return ResourceType::DLG;
    else if (CASE_INSENSITIVE_CMP(str, "itp") == 0) return ResourceType::ITP;
    else if (CASE_INSENSITIVE_CMP(str, "utt") == 0) return ResourceType::UTT;
    else if (CASE_INSENSITIVE_CMP(str, "dds") == 0) return ResourceType::DDS;
    else if (CASE_INSENSITIVE_CMP(str, "uts") == 0) return ResourceType::UTS;
    else if (CASE_INSENSITIVE_CMP(str, "ltr") == 0) return ResourceType::LTR;
    else if (CASE_INSENSITIVE_CMP(str, "gff") == 0) return ResourceType::GFF;
    else if (CASE_INSENSITIVE_CMP(str, "fac") == 0) return ResourceType::FAC;
    else if (CASE_INSENSITIVE_CMP(str, "ute") == 0) return ResourceType::UTE;
    else if (CASE_INSENSITIVE_CMP(str, "utd") == 0) return ResourceType::UTD;
    else if (CASE_INSENSITIVE_CMP(str, "utp") == 0) return ResourceType::UTP;
    else if (CASE_INSENSITIVE_CMP(str, "dft") == 0) return ResourceType::DFT;
    else if (CASE_INSENSITIVE_CMP(str, "gic") == 0) return ResourceType::GIC;
    else if (CASE_INSENSITIVE_CMP(str, "gui") == 0) return ResourceType::GUI;
    else if (CASE_INSENSITIVE_CMP(str, "utm") == 0) return ResourceType::UTM;
    else if (CASE_INSENSITIVE_CMP(str, "dwk") == 0) return ResourceType::DWK;
    else if (CASE_INSENSITIVE_CMP(str, "pwk") == 0) return ResourceType::PWK;
    else if (CASE_INSENSITIVE_CMP(str, "jrl") == 0) return ResourceType::JRL;
    else if (CASE_INSENSITIVE_CMP(str, "utw") == 0) return ResourceType::UTW;
    else if (CASE_INSENSITIVE_CMP(str, "ssf") == 0) return ResourceType::SSF;
    else if (CASE_INSENSITIVE_CMP(str, "ndb") == 0) return ResourceType::NDB;
    else if (CASE_INSENSITIVE_CMP(str, "ptm") == 0) return ResourceType::PTM;
    else if (CASE_INSENSITIVE_CMP(str, "ptt") == 0) return ResourceType::PTT;
    else if (CASE_INSENSITIVE_CMP(str, "bak") == 0) return ResourceType::BAK;
    else if (CASE_INSENSITIVE_CMP(str, "dat") == 0) return ResourceType::DAT;
    else if (CASE_INSENSITIVE_CMP(str, "shd") == 0) return ResourceType::SHD;
    else if (CASE_INSENSITIVE_CMP(str, "xbc") == 0) return ResourceType::XBC;
    else if (CASE_INSENSITIVE_CMP(str, "wbm") == 0) return ResourceType::WBM;
    else if (CASE_INSENSITIVE_CMP(str, "ids") == 0) return ResourceType::IDS;
    else if (CASE_INSENSITIVE_CMP(str, "erf") == 0) return ResourceType::ERF;
    else if (CASE_INSENSITIVE_CMP(str, "bif") == 0) return ResourceType::BIF;
    else if (CASE_INSENSITIVE_CMP(str, "key") == 0) return ResourceType::KEY;

    ASSERT_FAIL_MSG("Unknown resource type.");
    return ResourceType::INVALID;

#undef CASE_INSENSITIVE_CMP
}

char const* StringFromResourceType(ResourceType res)
{
    if (res == ResourceType::BMP) return "bmp";
    else if (res == ResourceType::TGA) return "tga";
    else if (res == ResourceType::WAV) return "wav";
    else if (res == ResourceType::PLT) return "plt";
    else if (res == ResourceType::INI) return "ini";
    else if (res == ResourceType::TXT) return "txt";
    else if (res == ResourceType::MDL) return "mdl";
    else if (res == ResourceType::NSS) return "nss";
    else if (res == ResourceType::NCS) return "ncs";
    else if (res == ResourceType::ARE) return "are";
    else if (res == ResourceType::SET) return "set";
    else if (res == ResourceType::IFO) return "ifo";
    else if (res == ResourceType::BIC) return "bic";
    else if (res == ResourceType::WOK) return "wok";
    else if (res == ResourceType::TWODA) return "2da";
    else if (res == ResourceType::TXI) return "txi";
    else if (res == ResourceType::GIT) return "git";
    else if (res == ResourceType::UTI) return "uti";
    else if (res == ResourceType::UTC) return "utc";
    else if (res == ResourceType::DLG) return "dlg";
    else if (res == ResourceType::ITP) return "itp";
    else if (res == ResourceType::UTT) return "utt";
    else if (res == ResourceType::DDS) return "dds";
    else if (res == ResourceType::UTS) return "uts";
    else if (res == ResourceType::LTR) return "ltr";
    else if (res == ResourceType::GFF) return "gff";
    else if (res == ResourceType::FAC) return "fac";
    else if (res == ResourceType::UTE) return "ute";
    else if (res == ResourceType::UTD) return "utd";
    else if (res == ResourceType::UTP) return "utp";
    else if (res == ResourceType::DFT) return "dft";
    else if (res == ResourceType::GIC) return "gic";
    else if (res == ResourceType::GUI) return "gui";
    else if (res == ResourceType::UTM) return "utm";
    else if (res == ResourceType::DWK) return "dwk";
    else if (res == ResourceType::PWK) return "pwk";
    else if (res == ResourceType::JRL) return "jrl";
    else if (res == ResourceType::UTW) return "utw";
    else if (res == ResourceType::SSF) return "ssf";
    else if (res == ResourceType::NDB) return "ndb";
    else if (res == ResourceType::PTM) return "ptm";
    else if (res == ResourceType::PTT) return "ptt";
    else if (res == ResourceType::BAK) return "bak";
    else if (res == ResourceType::DAT) return "dat";
    else if (res == ResourceType::SHD) return "shd";
    else if (res == ResourceType::XBC) return "xbc";
    else if (res == ResourceType::WBM) return "wbm";
    else if (res == ResourceType::IDS) return "ids";
    else if (res == ResourceType::ERF) return "erf";
    else if (res == ResourceType::BIF) return "bif";
    else if (res == ResourceType::KEY) return "key";

    ASSERT_FAIL_MSG("Unknown resource type.");
    return nullptr;
}

}