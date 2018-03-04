add_library(FileFormats STATIC

    Bif.hpp
    Bif/Bif_Raw.cpp Bif/Bif_Raw.hpp
    Bif/Bif_Friendly.cpp Bif/Bif_Friendly.hpp

    Erf.hpp
    Erf/Erf_Raw.cpp Erf/Erf_Raw.hpp
    Erf/Erf_Friendly.cpp Erf/Erf_Friendly.hpp

    Gff.hpp
    Gff/Gff_Raw.cpp Gff/Gff_Raw.hpp
    Gff/Gff_Friendly.cpp Gff/Gff_Friendly.hpp

    Key.hpp
    Key/Key_Raw.cpp Key/Key_Raw.hpp
    Key/Key_Friendly.cpp Key/Key_Friendly.hpp

    Resource.cpp Resource.hpp
)

target_link_libraries(FileFormats Utility)
