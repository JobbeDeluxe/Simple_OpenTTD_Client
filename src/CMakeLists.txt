add_library(sotc_core STATIC
    client_app.cpp
)

target_include_directories(sotc_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sotc_core
    PUBLIC
        project_options
        project_warnings
        project_dependencies
)

add_executable(sotc
    main.cpp
)

target_link_libraries(sotc
    PRIVATE
        sotc_core
)
