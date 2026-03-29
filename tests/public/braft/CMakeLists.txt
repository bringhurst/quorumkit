quorumkit_add_public_test(
    NAME test_braft_contract
    DOMAIN braft
    SOURCES test_braft_contract.cpp
    LABELS unit
)

quorumkit_add_public_test(
    NAME test_configuration
    DOMAIN braft
    SOURCES test_configuration_contract.cpp
    LABELS unit
)

quorumkit_add_public_test(
    NAME test_braft_runtime_contract
    DOMAIN braft
    SOURCES test_braft_runtime_contract.cpp
    LABELS integration network serial
    RESOURCE_LOCK ports_5706_5718
)
