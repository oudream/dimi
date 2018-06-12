
mc_gcl_psm_include()
mc_gcl_shm_include()
mc_gcl_vdi_daemon_heartbeat_include()

set(gsl_gcl_rthd_hdfs_files
        gcl_rthd_hdfs/main.cpp
        gcl_rthd_hdfs/rtdb_logfile.h
        gcl_rthd_hdfs/rtdb_logfile.cpp
        gcl_rthd_hdfs/rtdb_service.h
        gcl_rthd_hdfs/rtdb_service.cpp
        )

mc_merge_file_path(${gs_dimi_main_path} "${gsl_gcl_rthd_hdfs_files}" gsl_gcl_rthd_hdfs_filepaths)

add_executable(gcl_rthd_hdfs
        ${gsl_gcl_rthd_hdfs_filepaths}
        ${gsl_gcl_shm_filepaths}
        ${gsl_gcl_psm_filepaths}
        ${gsl_gcl_vdi_daemon_heartbeat_filepaths}
        )

add_dependencies(gcl_rthd_hdfs
        ccxx
        hdfs_static
        )

target_link_libraries(gcl_rthd_hdfs
        ccxx
        hdfs_static
        ${JAVA_JVM_LIBRARY}
        )

set_target_properties(gcl_rthd_hdfs PROPERTIES FOLDER dimi/main)
