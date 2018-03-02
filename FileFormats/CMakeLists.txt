add_library(FileFormats STATIC
    Gff.hpp
    Gff/Gff_Friendly.cpp Gff/Gff_Friendly.hpp
    Gff/Gff_Raw.cpp Gff/Gff_Raw.hpp)

target_link_libraries(FileFormats Utility)