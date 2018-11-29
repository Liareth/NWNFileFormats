add_executable(Example_2da Example_2da.cpp)
target_link_libraries(Example_2da FileFormats)
set_target_properties(Example_2da PROPERTIES FOLDER "Examples")

add_executable(Example_Bif Example_Bif.cpp)
target_link_libraries(Example_Bif FileFormats)
set_target_properties(Example_Bif PROPERTIES FOLDER "Examples")

add_executable(Example_Erf Example_Erf.cpp)
target_link_libraries(Example_Erf FileFormats)
set_target_properties(Example_Erf PROPERTIES FOLDER "Examples")

add_executable(Example_Gff Example_Gff.cpp)
target_link_libraries(Example_Gff FileFormats)
set_target_properties(Example_Gff PROPERTIES FOLDER "Examples")

add_executable(Example_Key Example_Key.cpp)
target_link_libraries(Example_Key FileFormats)
set_target_properties(Example_Key PROPERTIES FOLDER "Examples")

add_executable(Example_Tlk Example_Tlk.cpp)
target_link_libraries(Example_Tlk FileFormats)
set_target_properties(Example_Tlk PROPERTIES FOLDER "Examples")
