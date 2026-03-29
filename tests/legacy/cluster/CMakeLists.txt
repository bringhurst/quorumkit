quorumkit_add_legacy_test(
    NAME test_cli
    DOMAIN cluster
    SOURCES test_cli.cpp
    LABELS integration network serial
    RESOURCE_LOCK port_9500
)

quorumkit_add_legacy_test(
    NAME test_leader_lease
    DOMAIN cluster
    SOURCES test_leader_lease.cpp
    LABELS integration network serial
    RESOURCE_LOCK ports_5006_5015
)

quorumkit_add_legacy_test(
    NAME test_node
    DOMAIN cluster
    SOURCES test_node.cpp
    LABELS integration network slow serial
    RESOURCE_LOCK ports_5006_5015
)
