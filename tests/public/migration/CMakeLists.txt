quorumkit_add_public_test(
    NAME test_migration_contract
    DOMAIN migration
    SOURCES test_migration_contract.cpp
    LABELS integration filesystem network serial
    RESOURCE_LOCK port_5726
)
