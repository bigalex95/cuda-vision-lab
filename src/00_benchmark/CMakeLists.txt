add_executable(benchmark
    main.cpp
    array_ops_cpu.cpp
    array_ops_gpu.cu
)

target_include_directories(benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(benchmark
    ${OpenCV_LIBS}
)
