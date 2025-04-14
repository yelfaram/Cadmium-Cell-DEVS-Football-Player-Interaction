add_executable(${projectName} main.cpp)
target_sources(${projectName} PRIVATE include/data_structures/utils.cpp)
set(CADMIUM_DIR $ENV{CADMIUM})
target_include_directories(${projectName} PUBLIC
    "."
    "include"
    ${CADMIUM_DIR}
    "${CADMIUM_DIR}/../json/include"
)
target_compile_options(${projectName} PUBLIC -std=gnu++2b)