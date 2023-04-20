find_package(OpenSSL REQUIRED)

# set(SOURCE_FILE run.cpp)
# set(TARGET_NAME word_count)
#set(LINK_FILE_ONE util/file_to_set.cpp)
#set(LINK_FILE_TWO util/hash.cpp)

# This is a Windows-specific flag that enables exception handling in host code
if(WIN32)
    set(WIN_FLAG "/EHsc")
endif()

# 
# SECTION 1
# This section defines rules to create a cpu-gpu make target
# This can safely be removed if your project is only targetting FPGAs
#

set(COMPILE_FLAGS "-fsycl -Wall ${WIN_FLAG}")
set(LINK_FLAGS "-fsycl")

# To compile in a single command:
#    icpx -fsycl <file>.cpp -o <file>.fpga_emu
# CMake executes:
#    [compile] icpx -fsycl -o <file>.cpp.o -c <file>.cpp
#    [link]    icpx -fsycl <file>.cpp.o -o <file>.fpga_emu
add_executable(word_count run.cpp)
target_link_libraries(word_count OpenSSL::Crypto OpenSSL::SSL)
set_target_properties(word_count PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
set_target_properties(word_count PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
add_custom_target(cpu-gpu DEPENDS word_count)
