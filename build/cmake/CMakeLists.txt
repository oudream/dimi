#cmake . -G "Visual Studio 10 2010" --build "C:\ddd\ccpp\gcl3\build\cmake" -B"C:\ddd\ccpp\gcl3\build\cmake\ccpro-vs2010"
#cmake . -G "Visual Studio 12 2013" --build "C:\ddd\ccpp\gcl3\build\cmake" -B"C:\ddd\ccpp\gcl3\build\cmake\ccpro-vs2013"
#cmake . -G "Visual Studio 14 2015" --build "C:\ddd\ccpp\gcl3\build\cmake" -B"C:\ddd\ccpp\gcl3\build\cmake\ccpro-vs2015"
#-DQT_QMAKE_EXECUTABLE:FILEPATH=F:/qt5.5.1mingw4.9/5.5/mingw492_32/bin/qmake.exe
#-DCMAKE_PREFIX_PATH:FILEPATH=F:/qt5.5.1mingw4.9/5.5/mingw492_32

cmake_minimum_required(VERSION 3.0)

project(DIMI1 VERSION 1.0.1 LANGUAGES CXX C)

###qt
###if enable qt, then config "build/ccpro/local.config.cmake" 's CMAKE_PREFIX_PATH to your dir of qt
set(gs_project_enable_qt 1)
set(gs_project_enable_qt 0)
#set(gs_project_qt_version 4)

###depend
#CMAKE_CURRENT_SOURCE_DIR (current file path)
#set(gs_project_desploy_path "f:/gcl3deploy")
set(gs_project_path ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(gs_project_desploy_path ${CMAKE_CURRENT_SOURCE_DIR}/../deploy)
set(gs_project_3rd_path ${gs_project_path}/3rd)
set(gs_project_src_path ${gs_project_path}/src)
set(gs_project_include_path ${gs_project_path}/include)
set(gs_project_lib_path ${gs_project_path}/lib)

if (UNIX)
    set(gs_project_output_path_debug ${gs_project_desploy_path}/bin_unix_d)
    set(gs_project_output_path_release ${gs_project_desploy_path}/bin_unix)
else()
    if (WIN32)
        set(gs_project_output_path_debug ${gs_project_desploy_path}/bin_win32_d)
        set(gs_project_output_path_release ${gs_project_desploy_path}/bin_win32)
        if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
            #        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++")
            #        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
            set(gs_project_output_path_debug ${gs_project_desploy_path}/bin_win32_gnu_d)
            set(gs_project_output_path_release ${gs_project_desploy_path}/bin_win32_gnu)
        endif()
        link_directories(${gs_project_lib_path}/win32)
    else()
        set(gs_project_output_path_debug ${gs_project_deploy_path}/bin_win64_d)
        set(gs_project_output_path_release ${gs_project_deploy_path}/bin_win64)
        if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
            #        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -static-libstdc++")
            #        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
            set(gs_project_output_path_debug ${gs_project_deploy_path}/bin_win64_gnu_d)
            set(gs_project_output_path_release ${gs_project_deploy_path}/bin_win64_gnu)
        endif()
        link_directories(${gs_project_lib_path}/win64)
    endif()
endif()

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${gs_project_output_path_debug}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${gs_project_output_path_release}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${gs_project_output_path_debug}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${gs_project_output_path_release}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${gs_project_output_path_debug}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${gs_project_output_path_release}")

set(CMAKE_CXX_STANDARD 11)

include(common.cmake)
include(local.config.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(${gs_project_include_path})
include_directories(${gs_project_3rd_path})

## lib exe
## ccxx
include(${gs_ccxx_path}/build/cmake/ccxx/ccxx.cmake)
add_subdirectory(ccxx)

## gcl
include(${gs_gcl_path}/build/cmake/gcl/gcl.cmake)
include_directories(${gs_gcl_path}/include)

## hadoop
include(hadoop.cmake)
add_dual_library(hdfs
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/exception.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/jni_helper.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/hdfs.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/common/htable.c
        ${HDFS_OS_DIR}/mutexes.c
        ${HDFS_OS_DIR}/thread_local_storage.c
        )
if (NEED_LINK_DL)
    set(LIB_DL dl)
endif(NEED_LINK_DL)

target_link_dual_libraries(hdfs
        ${JAVA_JVM_LIBRARY}
        ${LIB_DL}
        ${OS_LINK_LIBRARIES}
        )

set(LIBHDFS_VERSION "0.0.0")
set_target_properties(hdfs PROPERTIES
        SOVERSION ${LIBHDFS_VERSION})

add_library(native_mini_dfs
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/native_mini_dfs.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/common/htable.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/exception.c
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/jni_helper.c
        ${HDFS_OS_DIR}/mutexes.c
        ${HDFS_OS_DIR}/thread_local_storage.c
        )
target_link_libraries(native_mini_dfs
        ${JAVA_JVM_LIBRARY}
        ${OS_LINK_LIBRARIES}
        )


## dimi
set(gs_dimi_path ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_subdirectory(dimi)


message(bging 001)
#message(MSVC)
message(end 001)
