set(COMPILE_FLAGS "-fsycl -Wall ${WIN_FLAG}")
set(LINK_FLAGS "-fsycl")

include(FetchContent)

# Download and configure the CLI11 library
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v1.9.1
)
FetchContent_MakeAvailable(cli11)

# Download and configure the fmt library
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.0
)
FetchContent_MakeAvailable(fmt)

add_executable(CommandLineInterfaceExample main.cpp)
target_link_libraries(CommandLineInterfaceExample PRIVATE CLI11::CLI11 fmt::fmt)

add_executable(
    sycl-word-cloud
    parallel-word-cloud.cpp
    ../utils/utils.cpp
)

set_target_properties(sycl-word-cloud PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
set_target_properties(sycl-word-cloud PROPERTIES LINK_FLAGS "${LINK_FLAGS}")