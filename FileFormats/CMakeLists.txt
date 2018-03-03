add_library(FileFormats STATIC

    Erf.hpp
    Erf/Erf_Raw.cpp Erf/Erf_Raw.hpp

    Gff.hpp
    Gff/Gff_Friendly.cpp Gff/Gff_Friendly.hpp
    Gff/Gff_Raw.cpp Gff/Gff_Raw.hpp

    Resource.cpp Resource.hpp
)

target_link_libraries(FileFormats Utility)