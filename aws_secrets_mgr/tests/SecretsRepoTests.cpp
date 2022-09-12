#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE commoncpp_tests
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <random>

#include <secretsrepo/SecretsRepo.h>

///
// These tests are more integration than unit tests, as they may depend on
// loading file from the filesystem, and/or external AWS services.
///

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(secretsrepo_tests)

BOOST_AUTO_TEST_CASE(secretsrepo_file)
{
    gdt::SecretsRepo repo;
    BOOST_TEST(!repo.isInitialized());

    std::string errMsg;

    fs::path secPath("test_secret.txt");
    fs::path absSecPath = fs::absolute(secPath);

    BOOST_TEST(fs::is_regular_file(absSecPath));

    std::string config = "file," + absSecPath.string();

    auto res = repo.initialize(config, errMsg);

    BOOST_TEST(res);
    BOOST_TEST("" == errMsg);
    BOOST_TEST(repo.isInitialized());

    std::string secValue = "";
    errMsg = "";

    res = repo.getSecret("ignored", secValue, errMsg);

    BOOST_TEST(res);
    BOOST_TEST("" == errMsg);
    BOOST_TEST(secValue == "swordfish");
}

BOOST_AUTO_TEST_CASE(secretsrepo_awssm)
{
    gdt::SecretsRepo repo;
    BOOST_TEST(!repo.isInitialized());

    std::string errMsg;

    std::string config = "awssm,thisisatestconfig";

    auto res = repo.initialize(config, errMsg);

    BOOST_TEST(res);
    BOOST_TEST("" == errMsg);
    BOOST_TEST(repo.isInitialized());

    std::string secValue = "";
    errMsg = "";

    res = repo.getSecret("test/proto/test_secret", secValue, errMsg);

    BOOST_TEST(res);
    BOOST_TEST("" == errMsg);
    BOOST_TEST(secValue == "swordfish");
}

BOOST_AUTO_TEST_SUITE_END()