#include <boost/test/unit_test.hpp>

#include <playchain/playchain_user.h>
#include <playchain/playchain_helper.h>

namespace playchain_user_tests {
using namespace tp;

BOOST_AUTO_TEST_SUITE(playchain_user_tests)

BOOST_AUTO_TEST_CASE(initialize_check)
{
    PlaychainUser user { "alice", PlaychainUserId { 12 }, "5Kf3Z8fUUdrMVqbozbrUVB6mAb2FFXxmcZ4hL6FJFYheD3hSmHW" };

    BOOST_CHECK_EQUAL(user.name(), "alice");
    BOOST_CHECK_EQUAL(user.id(), PlaychainUserId { 12 });
    BOOST_CHECK_EQUAL(user.getSerializedPublicKey(), "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY");
    BOOST_CHECK_EQUAL((PlaychainUser { "bob", PlaychainUserId { 1 }, "5HtDVv4JevGEUjtChnYzqvZNepHqMQPanaR45LyspFQmSnyoVDe" }).getSerializedPublicKey(),
                      "PLC83E5joJjQNJfNfbYYwGhNNY5zkDkkqCNLZVQU47EyJwJwUWzDr");
    BOOST_CHECK_EQUAL((PlaychainUser { "bob", PlaychainUserId { 2 }, "5JMhpyYAJLkDFWkFNqSquXPDwzc8tYT94jznQACykP7qJ6WQFZT" }).getSerializedPublicKey(),
                      "PLC8BBsSDaofVroXqLSP28fFQubFk4J7HGoqTXigHXSu9VKyx5rv7");
}

BOOST_AUTO_TEST_CASE(initialize_negative_check)
{
    BOOST_CHECK_THROW((PlaychainUser { "alice", PlaychainUserId { 12 }, "" }), std::exception);
    BOOST_CHECK_THROW((PlaychainUser { "alice", PlaychainUserId { 12 }, "5Kf31111" }), std::exception);
    BOOST_CHECK_THROW((PlaychainUser { "alice", PlaychainUserId { 12 }, "11f3Z8fUUdrMVqbozbrUVB6mAb2FFXxmcZ4hL6FJFYheD3hSmHW" }), std::exception);
}

BOOST_AUTO_TEST_CASE(sign_digest_check)
{
    PlaychainUser user { "andrew", PlaychainUserId { 14 }, "5HtDVv4JevGEUjtChnYzqvZNepHqMQPanaR45LyspFQmSnyoVDe" };

    BOOST_CHECK_EQUAL(user.getSerializedPublicKey(), "PLC83E5joJjQNJfNfbYYwGhNNY5zkDkkqCNLZVQU47EyJwJwUWzDr");

    const std::string digest = "c14a0494ac87dccc09da5c7d25a551eab4e45777cedac2ea1978ff56947ed0d4";

    auto&& hex_sign = user.signDigest(digest);

    BOOST_CHECK(check_signature(convert_signature(hex_sign), convert_digest(digest), user.getPublicKey()));
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace playchain_user_tests
