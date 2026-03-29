quorumkit_add_internal_test(
    NAME test_file_system_adaptor
    DOMAIN storage
    SOURCES test_file_system_adaptor.cpp
    LABELS unit filesystem
)

quorumkit_add_internal_test(
    NAME test_fsync
    DOMAIN storage
    SOURCES test_fsync.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_log
    DOMAIN storage
    SOURCES test_log.cpp
    LABELS integration filesystem slow
)

quorumkit_add_internal_test(
    NAME test_log_entry
    DOMAIN storage
    SOURCES test_log_entry.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_log_manager
    DOMAIN storage
    SOURCES test_log_manager.cpp
    LABELS unit filesystem
)

quorumkit_add_internal_test(
    NAME test_memory_storage
    DOMAIN storage
    SOURCES test_memory_storage.cpp
    LABELS integration
)

quorumkit_add_internal_test(
    NAME test_protobuf_file
    DOMAIN storage
    SOURCES test_protobuf_file.cpp
    LABELS unit filesystem
)

quorumkit_add_internal_test(
    NAME test_snapshot_executor
    DOMAIN storage
    SOURCES test_snapshot_executor.cpp
    LABELS integration filesystem network slow serial
    RESOURCE_LOCK port_6500
)
