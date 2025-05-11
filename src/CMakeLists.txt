# Library.

set(LIB_NAME lib${CMAKE_PROJECT_NAME})

add_library(${LIB_NAME}
    file.c
    patch.c
)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${LIB_NAME}
    zstd
)

# Executable.

set(EXE_NAME ${CMAKE_PROJECT_NAME})

add_executable(${EXE_NAME}
    main.c
)

target_link_libraries(${EXE_NAME}
    ${LIB_NAME}
)
