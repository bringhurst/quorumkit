quorumkit_add_legacy_test(
    NAME test_file_service
    DOMAIN storage
    SOURCES test_file_service.cpp
    LABELS integration filesystem network
)

quorumkit_add_legacy_test(
    NAME test_meta
    DOMAIN storage
    SOURCES test_meta.cpp
    LABELS integration filesystem
)

quorumkit_add_legacy_test(
    NAME test_snapshot
    DOMAIN storage
    SOURCES test_snapshot.cpp
    LABELS integration filesystem network slow serial
    RESOURCE_LOCK port_6006
)
