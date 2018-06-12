
add_executable(test_libhdfs_ops
        ${gs_dimi_test_path}/hdfs/test_libhdfs_ops.c
        )
target_link_libraries(test_libhdfs_ops
        hdfs_static
        ${JAVA_JVM_LIBRARY}
        )


add_executable(test_libhdfs_read
        ${gs_dimi_test_path}/hdfs/test_libhdfs_read.c
        )
target_link_libraries(test_libhdfs_read
        hdfs_static
        ${JAVA_JVM_LIBRARY}
        )


add_executable(test_libhdfs_write
        ${gs_dimi_test_path}/hdfs/test_libhdfs_write.c
        )
target_link_libraries(test_libhdfs_write
        hdfs_static
        ${JAVA_JVM_LIBRARY}
        )


add_executable(test_libhdfs_zerocopy
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/expect.c
        ${gs_dimi_test_path}/hdfs/test_libhdfs_zerocopy.c
        )
target_link_libraries(test_libhdfs_zerocopy
        hdfs_static
        native_mini_dfs
        ${OS_LINK_LIBRARIES}
        )


add_executable(test_htable
        ${gs_hadoop_hdfs_path}/main/native/libhdfs/common/htable.c
        ${gs_dimi_test_path}/hdfs/test_htable.c
        )
target_link_libraries(test_htable
        ${OS_LINK_LIBRARIES}
        )
