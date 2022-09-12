cmake_minimum_required (VERSION 3.2)
project (secretsrepo)

add_library(
    secretsrepo STATIC
        FileSecretsContainer.cpp
        AWSSMSecretsContainer.cpp
        SecretsRepo.cpp
)

add_dependencies(secretsrepo gitrev awssdk)
TARGET_LINK_LIBRARIES(secretsrepo LINK_PUBLIC ${AWSSDK_LIBRARIES})
target_compile_options(secretsrepo PRIVATE -fPIC)

add_subdirectory(tests)
