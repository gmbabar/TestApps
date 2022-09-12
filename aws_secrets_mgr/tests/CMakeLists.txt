cmake_minimum_required (VERSION 3.2)
project (secretsrepo_tests)

add_executable(
    secretsrepo_tests
        SecretsRepoTests.cpp
)

configure_file(test_secret.txt test_secret.txt COPYONLY)

ADD_DEPENDENCIES(secretsrepo_tests gitrev)
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC secretsrepo)
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${AWSSDK_LIBRARIES})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${OPENSSL_LIBRARIES})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${CURL_LIBRARIES})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC dl)
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(secretsrepo_tests LINK_PUBLIC ${LIB_RT})
