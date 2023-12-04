cmake_minimum_required(VERSION 3.0.0)
#find_package(SDL2 REQUIRED)

project(example VERSION 0.0.1)

set(headers tsf.h tml.h examples/minisdl_audio.h)
# set(sources examples/minisdl_audio.c examples/example3.c)
set(sources main.c)
add_executable(example_exe ${sources})
target_compile_options(example_exe PRIVATE -Wall)
target_link_libraries(example_exe PRIVATE m dl pthread SDL2)