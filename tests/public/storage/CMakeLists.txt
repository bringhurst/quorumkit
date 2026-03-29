quorumkit_add_public_test(
    NAME test_storage
    DOMAIN storage
    SOURCES test_storage.cpp
    LABELS integration filesystem
)

quorumkit_add_public_test(
    NAME test_snapshot_storage_contract
    DOMAIN storage
    SOURCES test_snapshot_storage_contract.cpp
    LABELS integration filesystem network serial
    RESOURCE_LOCK port_5736
)
