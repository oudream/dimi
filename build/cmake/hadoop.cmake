
set(gs_hadoop_hdfs_path ${gs_hadoop_path}/hadoop-hdfs-project/hadoop-hdfs/src)

include(${gs_hadoop_path}/hadoop-common-project/hadoop-common/src/JNIFlags.cmake NO_POLICY_SCOPE)

# Compile a library with both shared and static variants
function(add_dual_library LIBNAME)
    add_library(${LIBNAME} SHARED ${ARGN})
    add_library(${LIBNAME}_static STATIC ${ARGN})
    # Linux builds traditionally ship a libhdfs.a (static linking) and libhdfs.so
    # (dynamic linking).  On Windows, we cannot use the same base name for both
    # static and dynamic, because Windows does not use distinct file extensions
    # for a statically linked library vs. a DLL import library.  Both use the
    # .lib extension.  On Windows, we'll build the static library as
    # hdfs_static.lib.
    if (NOT WIN32)
        set_target_properties(${LIBNAME}_static PROPERTIES OUTPUT_NAME ${LIBNAME})
    endif (NOT WIN32)
endfunction(add_dual_library)

# Link both a static and a dynamic target against some libraries
function(target_link_dual_libraries LIBNAME)
    target_link_libraries(${LIBNAME} ${ARGN})
    target_link_libraries(${LIBNAME}_static ${ARGN})
endfunction(target_link_dual_libraries)

function(output_directory TGT DIR)
    SET_TARGET_PROPERTIES(${TGT} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${DIR}")
    SET_TARGET_PROPERTIES(${TGT} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${DIR}")
    SET_TARGET_PROPERTIES(${TGT} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${DIR}")
endfunction(output_directory TGT DIR)

function(dual_output_directory TGT DIR)
    output_directory(${TGT} "${DIR}")
    output_directory(${TGT}_static "${DIR}")
endfunction(dual_output_directory TGT DIR)

# Flatten a list into a string.
function(FLATTEN_LIST INPUT SEPARATOR OUTPUT)
    string (REPLACE ";" "${SEPARATOR}" _TMPS "${INPUT}")
    set (${OUTPUT} "${_TMPS}" PARENT_SCOPE)
endfunction()

# Check to see if our compiler and linker support the __thread attribute.
# On Linux and some other operating systems, this is a more efficient
# alternative to POSIX thread local storage.
INCLUDE(CheckCSourceCompiles)
CHECK_C_SOURCE_COMPILES("int main(void) { static __thread int i = 0; return 0; }" HAVE_BETTER_TLS)

# Check to see if we have Intel SSE intrinsics.
CHECK_C_SOURCE_COMPILES("#include <emmintrin.h>\nint main(void) { __m128d sum0 = _mm_set_pd(0.0,0.0); return 0; }" HAVE_INTEL_SSE_INTRINSICS)

# Check if we need to link dl library to get dlopen.
# dlopen on Linux is in separate library but on FreeBSD its in libc
INCLUDE(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(dl dlopen "" NEED_LINK_DL)

find_package(JNI REQUIRED)
if (NOT GENERATED_JAVAH)
    # Must identify where the generated headers have been placed
    MESSAGE(FATAL_ERROR "You must set the CMake variable GENERATED_JAVAH")
endif (NOT GENERATED_JAVAH)

if (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")

    # Set warning level 4.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")

    # Skip "unreferenced formal parameter".
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100")

    # Skip "conditional expression is constant".
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4127")

    # Skip deprecated POSIX function warnings.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")

    # Skip CRT non-secure function warnings.  If we can convert usage of
    # strerror, getenv and ctime to their secure CRT equivalents, then we can
    # re-enable the CRT non-secure function warnings.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

    # Omit unneeded headers.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWIN32_LEAN_AND_MEAN")

    set(HDFS_OS_DIR ${gs_hadoop_hdfs_path}/main/native/libhdfs/os/windows)
#    set(OUT_DIR target/bin)
else (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_REENTRANT -D_GNU_SOURCE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    set(HDFS_OS_DIR ${gs_hadoop_hdfs_path}/main/native/libhdfs/os/posix)
    set(OS_LINK_LIBRARIES pthread)
#    set(OUT_DIR target/usr/local/lib)
endif (WIN32)

add_definitions(-DLIBHDFS_DLL_EXPORT)

include_directories(
        ${GENERATED_JAVAH}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${JNI_INCLUDE_DIRS}
        ${gs_hadoop_hdfs_path}/main/native
        ${gs_hadoop_hdfs_path}/main/native/libhdfs
        ${HDFS_OS_DIR}
)

set(_FUSE_DFS_VERSION 0.1.0)
CONFIGURE_FILE(${gs_hadoop_hdfs_path}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)



