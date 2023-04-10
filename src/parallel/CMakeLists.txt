set(COMPILE_FLAGS "-fsycl -Wall ${WIN_FLAG}")
set(LINK_FLAGS "-fsycl")



add_executable(
    sycl-word-cloud
    parallel-word-cloud.cpp
    ../utils/utils.cpp
)

set_target_properties(sycl-word-cloud PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
set_target_properties(sycl-word-cloud PROPERTIES LINK_FLAGS "${LINK_FLAGS}")