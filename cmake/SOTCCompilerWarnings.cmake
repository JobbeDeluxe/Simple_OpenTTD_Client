function(sotc_set_project_warnings target)
    if(MSVC)
        target_compile_options(${target} INTERFACE
            /W4
            /w14242 # conversion, possible loss of data
            /w14254 # conversion, possible loss of data
            /w14263 # member function does not override any base class virtual member function
            /w14265 # class has virtual functions, but destructor is not virtual
            /w14287 # unsigned/negative constant mismatch
            /we4289 # loop control variable used outside for-loop scope
            /wd4345 # behavior change: an object is allocated on the heap
            /w14296 # expression is always true
            /w14311 # pointer truncation from 'type1' to 'type2'
            /w14545 # expression before comma has no effect; expected expression with side-effect
            /w14546 # function call before comma missing argument list
            /w14547 # operator before comma has no effect; expected operator with side-effect
            /w14549 # operator before comma has no effect; expected operator with side-effect
            /w14555 # expression has no effect; expected expression with side-effect
            /w14619 # pragma warning(pop) : file(s) not found
            /w14905 # wide string literal cast to 'LPSTR'
            /w14906 # string literal cast to 'LPWSTR'
            /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
            /permissive-
            /EHsc
        )
    else()
        target_compile_options(${target} INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        )
    endif()
endfunction()
