quorumkit_add_public_test(
    NAME test_wire_contract
    DOMAIN wire
    SOURCES test_wire_contract.cpp
    LABELS unit
)

quorumkit_add_public_test(
    NAME test_snapshot_copy_wire_contract
    DOMAIN wire
    SOURCES test_snapshot_copy_wire_contract.cpp
    LABELS integration filesystem network serial
    RESOURCE_LOCK port_5746
)
