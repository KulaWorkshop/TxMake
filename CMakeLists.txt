cmake_minimum_required(VERSION 3.10)

project(txmake)

set(txdump_path "src/txdump")
set(txmake_path "src/txmake")
set(txgenerate_path "src/txgenerate")

add_executable(txdump 
    ${txdump_path}/clut.cpp
    ${txdump_path}/color.cpp
    ${txdump_path}/constants.cpp
    ${txdump_path}/fog.cpp
    ${txdump_path}/main.cpp
    ${txdump_path}/skybox.cpp
    ${txdump_path}/texture.cpp
    ${txdump_path}/utils.cpp
)

add_executable(txgenerate
    ${txgenerate_path}/clut.cpp
    ${txgenerate_path}/generate.cpp
    ${txgenerate_path}/main.cpp
    ${txgenerate_path}/quantize.cpp
    ${txgenerate_path}/tileset.cpp
    ${txgenerate_path}/utils.cpp
)

add_executable(txmake 
    ${txmake_path}/clut.cpp
    ${txmake_path}/main.cpp
    ${txmake_path}/tileset.cpp
    ${txmake_path}/utils.cpp
    ${txmake_path}/config.cpp
)

target_include_directories(txdump PRIVATE ghc/include lib ${txdump_path}/include)
target_include_directories(txmake PRIVATE ghc/include toml lib ${txmake_path}/include)
target_include_directories(txgenerate PRIVATE ghc/include lib ${txgenerate_path}/include)

target_link_directories(txdump PRIVATE lib)
target_link_directories(txmake PRIVATE lib)
target_link_directories(txgenerate PRIVATE lib)

if(WIN32 AND MSVC)
    target_link_libraries(txdump PRIVATE FreeImage)
    target_link_libraries(txmake PRIVATE FreeImage)
    target_link_libraries(txgenerate PRIVATE "libimagequant-msvc.lib" FreeImage ws2_32 bcrypt userenv)

    add_custom_command(TARGET txmake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/lib/FreeImage.dll
        $<TARGET_FILE_DIR:txmake>
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(txdump PRIVATE "libfreeimage.a")
    target_link_libraries(txmake PRIVATE "libfreeimage.a")
    target_link_libraries(txgenerate PRIVATE "libimagequant-linux.a" "libfreeimage.a")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_custom_command(TARGET txmake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/txtemplate.dat        $<TARGET_FILE_DIR:txmake>
)