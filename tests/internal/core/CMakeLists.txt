quorumkit_add_internal_test(
    NAME test_ballot
    DOMAIN core
    SOURCES test_ballot.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_ballot_box
    DOMAIN core
    SOURCES test_ballot_box.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_checksum
    DOMAIN core
    SOURCES test_checksum.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_configuration
    DOMAIN core
    SOURCES test_configuration.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_fsm_caller
    DOMAIN core
    SOURCES test_fsm_caller.cpp
    LABELS integration network serial
    RESOURCE_LOCK port_6006
)

quorumkit_add_internal_test(
    NAME test_repeated_timer_task
    DOMAIN core
    SOURCES test_repeated_timer_task.cpp
    LABELS unit
)

quorumkit_add_internal_test(
    NAME test_throttle
    DOMAIN core
    SOURCES test_throttle.cpp
    LABELS unit slow
)

quorumkit_add_internal_test(
    NAME test_util
    DOMAIN core
    SOURCES test_util.cpp
    LABELS integration
)
