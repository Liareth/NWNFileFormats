add_executable(2da_merge Tool_2daMerge.cpp)
target_link_libraries(2da_merge FileFormats)
set_target_properties(2da_merge PROPERTIES FOLDER "Tools")

add_executable(diff_creature "Tool_DiffCreature.cpp")
target_link_libraries(diff_creature FileFormats)
set_target_properties(diff_creature PROPERTIES FOLDER "Tools")

add_executable(erf_extractor Tool_ErfExtractor.cpp)
target_link_libraries(erf_extractor FileFormats)
set_target_properties(erf_extractor PROPERTIES FOLDER "Tools")

add_executable(erf_packer Tool_ErfPacker.cpp)
target_link_libraries(erf_packer FileFormats)
set_target_properties(erf_packer PROPERTIES FOLDER "Tools")

if (UNIX)
    target_link_libraries(erf_packer stdc++fs)
endif()

add_executable(key_bif_extractor Tool_KeyBifExtractor.cpp)
target_link_libraries(key_bif_extractor FileFormats)
set_target_properties(key_bif_extractor PROPERTIES FOLDER "Tools")

add_executable(generate_placeable_blueprints Tool_GeneratePlaceableBlueprints.cpp)
target_link_libraries(generate_placeable_blueprints FileFormats)
set_target_properties(generate_placeable_blueprints PROPERTIES FOLDER "Tools")
