set(COMPILE_FLAGS "-fsycl -Wall ${WIN_FLAG}")
set(LINK_FLAGS "-fsycl")

add_executable(
    v2
    v2.cpp
    ../utils/utils.cpp
)

target_link_libraries(sycl-word-cloud PRIVATE CLI11::CLI11 fmt::fmt)

set_target_properties(v2 PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
set_target_properties(v2 PROPERTIES LINK_FLAGS "${LINK_FLAGS}")