include_guard(GLOBAL)

function(sotc_configure_dependencies target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "sotc_configure_dependencies expected an existing target")
    endif()

    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(CURL CONFIG REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(spdlog CONFIG REQUIRED)

    if(SOTC_USE_OPENSSL)
        find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
        set(_sotc_enable_openssl TRUE)
    else()
        find_package(OpenSSL COMPONENTS SSL Crypto)
        set(_sotc_enable_openssl FALSE)
    endif()

    target_link_libraries(${target} INTERFACE
        SDL2::SDL2
        CURL::libcurl
        ZLIB::ZLIB
        spdlog::spdlog
    )

    if(TARGET SDL2::SDL2main)
        target_link_libraries(${target} INTERFACE SDL2::SDL2main)
    endif()

    if(TARGET SDL2::SDL2_image)
        target_link_libraries(${target} INTERFACE SDL2::SDL2_image)
    endif()

    if(TARGET SDL2::SDL2_ttf)
        target_link_libraries(${target} INTERFACE SDL2::SDL2_ttf)
    endif()

    if(_sotc_enable_openssl AND OpenSSL_FOUND)
        target_compile_definitions(${target} INTERFACE SOTC_HAS_OPENSSL=1)
        target_link_libraries(${target} INTERFACE OpenSSL::SSL OpenSSL::Crypto)
    else()
        target_compile_definitions(${target} INTERFACE SOTC_HAS_OPENSSL=0)
    endif()
endfunction()
