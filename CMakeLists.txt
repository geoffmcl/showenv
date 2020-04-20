# showenv - root CMakeLists.txt - 20140108
# 20161030 - Change EXE name, per BITNESS
# 20200420 - Make it an otion to change name, and for MSVC option to use STATIC runtimes
cmake_minimum_required (VERSION 2.8)

project (showenv)

option( SET_EXE_NAME "Set ON to add bitness, 32 or 64, to EXE name" OFF )

# check 64 bit
if( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )
   set( BITNESS 32 )
   set( HAVE_64_BIT 0 )
   message(STATUS "*** Seems sizeof(void *) = 4 == 32-bits")
else()
   if( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
        set( HAVE_64_BIT 1 )
        set( BITNESS 64 )
        message(STATUS "*** Seems sizeof(void *) == 8 == 64-bits")
        if (MSVC)
            add_definitions( -DWIN64=1 )
        endif ()
   else ()
        message(FATAL_ERROR "Variable CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P} NOT 4 or 8! *** FIX ME ***")
   endif ()
endif()

#------------------------------------------------------------------------
# Static Windows Runtime
#   Option to statically link to the Windows runtime. Maybe only 
#   applies to WIN32/MSVC.
#------------------------------------------------------------------------
if (MSVC)
    option( USE_STATIC_RUNTIME "Set ON to change /MD(DLL) to /MT(static)" ON )
    if (USE_STATIC_RUNTIME)
        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()    
        message(STATUS "Using /MT STATIC runtime")
    else ()
        message(STATUS "Using /MD DYNAMIC runtime")
    endif ()
endif ()

set(name showenv)
if (SET_EXE_NAME)
    set(EXE_NAME ${name}${BITNESS})
else ()
    set(EXE_NAME ${name})
endif ()
add_executable(${name} ${name}.cpp ${name}vers.h se_list.h)
if (SET_EXE_NAME)
    set_target_properties(${name} PROPERTIES OUTPUT_NAME ${EXE_NAME} )
endif()
if(MSVC)
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX d)
    install(TARGETS ${name} DESTINATION "C:\\MDOS")
else()    
    install(TARGETS ${name} DESTINATION bin)
endif()

# eof
