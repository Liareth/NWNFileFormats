add_executable(2da_merge Tool_2daMerge.cpp)
target_link_libraries(2da_merge FileFormats)
set_target_properties(2da_merge PROPERTIES FOLDER "Tools")

add_executable(key_bif_extractor Tool_KeyBifExtractor.cpp)
target_link_libraries(key_bif_extractor FileFormats)
set_target_properties(key_bif_extractor PROPERTIES FOLDER "Tools")
