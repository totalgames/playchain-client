#include "playchain_tests_common.h"

#include <playchain/playchain_user.h>
#include <playchain/playchain_helper.h>

namespace request_builder_tests {
using namespace tp;

namespace utility = playchain::test;

BOOST_FIXTURE_TEST_SUITE(request_builder_tests, utility::builder_fixture)

BOOST_AUTO_TEST_CASE(makeLegacyLoginRequest_check)
{
    PlaychainUser user { "alice", PlaychainUserId { 12 }, "5Kf3Z8fUUdrMVqbozbrUVB6mAb2FFXxmcZ4hL6FJFYheD3hSmHW" };

    BlockchainRequest result = builder().makeLegacyLoginRequest("alice", user.getSerializedPublicKey());

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                        0,
                        "login_with_pubkey",
                        [
                            "alice",
                            "02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f"
                        ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));

    BOOST_CHECK_EQUAL(builder().makeLegacyLoginRequest("alice", user.getSerializedPublicKey()).str(), result.str());
    BOOST_CHECK_EQUAL(builder().makeLegacyLoginRequest("alice", "02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f").str(), result.str());
    BOOST_CHECK_EQUAL(builder().makeLegacyLoginRequest("alice", "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY").str(), result.str());
}

BOOST_AUTO_TEST_CASE(makeLegacyGetAccountBalanceRequest_check)
{
    BlockchainRequest result = builder().makeLegacyGetAccountBalanceRequest("alice");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       0,
                       "list_account_balances",
                       [
                           "alice"
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeLegacyCreateAccountWithPubkeyRequest_check)
{
    BlockchainRequest result = builder().makeLegacyCreateAccountWithPubkeyRequest("totalpoker", "bob", "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       0,
                       "create_account_with_pubkey",
                       [
                           "02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f",
                           "bob",
                           "totalpoker",
                           "totalpoker",
                           true,
                           false
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeLegacyGetAccountIdByNameRequest_check)
{
    BlockchainRequest result = builder().makeLegacyGetAccountIdByNameRequest("totalpoker", "bob", "alice");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "database",
                       "lookup_account_names",
                       [
                           [
                               "alice",
                               "bob",
                               "totalpoker"
                           ]
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeLoginRequest_check)
{
    BlockchainRequest result = builder().makeLoginRequest("alice");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                        "playchain",
                        "get_player_account_by_name",
                        [
                            "alice"
                        ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeGetChainIdRequest_check)
{
    BlockchainRequest result = builder().makeGetChainIdRequest();

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "database",
                       "get_chain_id",
                       []
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeGetTablesInfoRequest_check)
{
    BlockchainRequest result = builder().makeGetTablesInfoRequest({ PlaychainTableId { 1 }, PlaychainTableId { 2 }, PlaychainTableId { 3 } });

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "get_tables_info_by_id",
                       [
                           [
                               "3.4.1",
                               "3.4.2",
                               "3.4.3"
                           ]
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeGetAccountIdByNameRequest_check)
{
    BlockchainRequest result = builder().makeGetAccountIdByNameRequest("alice");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "get_account_id_by_name",
                       [
                           [
                               "alice"
                           ]
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeGetLastIrreversibleBlockHeaderRequest_check)
{
    BlockchainRequest result = builder().makeGetLastIrreversibleBlockHeaderRequest();

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "get_last_irreversible_block_header",
                       []
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeListPlayerInvitationsRequest_check)
{
    BlockchainRequest result = builder().makeListPlayerInvitationsRequest(PlaychainUserId { 168 }, "", 10);

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "list_player_invitations",
                       [
                           "1.2.168",
                           "",
                           10
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeListInvitedPlayersRequest_check)
{
    BlockchainRequest result = builder().makeListInvitedPlayersRequest(PlaychainUserId { 168 }, "", 10);

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "list_invited_players",
                       [
                           "1.2.168",
                           "",
                           10
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeGetPlaychainBalanceRequest_check)
{
    BlockchainRequest result = builder().makeGetPlaychainBalanceRequest(PlaychainUserId { 168 });

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "get_playchain_balance_info",
                       [
                           "1.2.168"
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(getPlayerInvitationDigest_check)
{
    std::string result = builder().getPlayerInvitationDigest("alice", "19888");
    BOOST_REQUIRE_EQUAL(result, "96ecf2ae636801399b25eba47476b95a59be6147d12564be39ae3cd1336554cb");
}

BOOST_AUTO_TEST_CASE(makeCreatePlayerInvitationTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeCreatePlayerInvitationTransaction(PlaychainUserId { 168 }, "19888", 600,
                                                                                         R"j({"info":"It-is-mine"})j");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       56,
                                       {
                                           "fee": {
                                               "amount": 25000,
                                               "asset_id": "1.3.0"
                                           },
                                           "inviter": "1.2.168",
                                           "uid": "19888",
                                           "lifetime_in_sec": 600,
                                           "metadata": "{\"info\":\"It-is-mine\"}"
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "89ab768b233160d23c98d5661270d6008e94e3f6fc9fa99691872bbc33ad4d9b");
}

BOOST_AUTO_TEST_CASE(makeCancelPlayerInvitationTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeCancelPlayerInvitationTransaction(PlaychainUserId { 168 }, "19888");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       58,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "inviter": "1.2.168",
                                           "uid": "19888"
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "43346edb52838fb250906f6de2fb273a3636188f6f7a0a0a7442f5bf90223a47");
}

BOOST_AUTO_TEST_CASE(makeResolvePlayerInvitationTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeResolvePlayerInvitationTransaction(PlaychainUserId { 168 }, "19888",
                                                                                          "1f14561ec1c707fa8bed974c10c041b0c96acbb9398556ee2223d4af61fd9428b10eb5b5665e163528cf1f97723d708eed60d5a595ef950f88d318bb83ed747fa5",
                                                                                          "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY",
                                                                                          "player2");

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       57,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "inviter": "1.2.168",
                                           "uid": "19888",
                                           "mandat": "1f14561ec1c707fa8bed974c10c041b0c96acbb9398556ee2223d4af61fd9428b10eb5b5665e163528cf1f97723d708eed60d5a595ef950f88d318bb83ed747fa5",
                                           "name": "player2",
                                           "owner": {
                                               "weight_threshold": 1,
                                               "account_auths": [],
                                               "key_auths": [
                                                   [
                                                       "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY",
                                                       1
                                                   ]
                                               ]
                                           },
                                           "active": {
                                               "weight_threshold": 1,
                                               "account_auths": [],
                                               "key_auths": [
                                                   [
                                                       "PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY",
                                                       1
                                                   ]
                                               ]
                                           }
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "c6279ab4775415a5e9c8a3ef8b969d05ed3700fdeff2c2d7927ec6f14fc76806");
}

BOOST_AUTO_TEST_CASE(makeBuyinTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeBuyinTransaction(PlaychainUserId { 168 }, PlaychainUserId { 10 },
                                                                        PlaychainTableId { 1 }, 100000); //1 PLC

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       66,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "player": "1.2.168",
                                           "table": "3.4.1",
                                           "table_owner": "1.2.10",
                                           "amount": {
                                               "amount": 100000,
                                               "asset_id": "1.3.0"
                                           }
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "36511276508c536583c29b92715f12aa66f035149303041d3b754b8033238520");
}

BOOST_AUTO_TEST_CASE(makeBuyoutTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeBuyoutTransaction(PlaychainUserId { 168 }, PlaychainUserId { 10 },
                                                                         PlaychainTableId { 1 }, 200000, "Player-do-buy-out"); //2 PLC

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       67,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "player": "1.2.168",
                                           "table": "3.4.1",
                                           "table_owner": "1.2.10",
                                           "amount": {
                                               "amount": 200000,
                                               "asset_id": "1.3.0"
                                           },
                                           "reason": "Player-do-buy-out"
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "21db56a330c56dd6a942b33a4b9fbeb188aab125972e0208710053f24bf87e6e");
}

BOOST_AUTO_TEST_CASE(makeGameStartPlayingTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeVoteForStartGameTransaction(PlaychainUserId { 168 }, PlaychainUserId { 10 },
                                                                                   PlaychainTableId { 1 },
                                                                                   GameInitialData { { std::make_pair(PlaychainUserId { 168 }, 100000),
                                                                                                       std::make_pair(PlaychainUserId { 166 }, 50000),
                                                                                                       std::make_pair(PlaychainUserId { 167 }, 100000) },
                                                                                                     "Alice-is-diler" });

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       68,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "table": "3.4.1",
                                           "table_owner": "1.2.10",
                                           "voter": "1.2.168",
                                           "initial_data": {
                                               "cash": [
                                                   [
                                                       "1.2.166",
                                                       {
                                                           "amount": 50000,
                                                           "asset_id": "1.3.0"
                                                       }
                                                   ],
                                                   [
                                                       "1.2.167",
                                                       {
                                                           "amount": 100000,
                                                           "asset_id": "1.3.0"
                                                       }
                                                   ],
                                                   [
                                                       "1.2.168",
                                                       {
                                                           "amount": 100000,
                                                           "asset_id": "1.3.0"
                                                       }
                                                   ]
                                               ],
                                               "info": "Alice-is-diler"
                                           }
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "86757014eebeebf9b4425811e155746aba6794f4f2af326d6b627db7be7c2b5d");
}

BOOST_AUTO_TEST_CASE(makeGameResultPlayingTransaction_check)
{
    set_chain_info();

    using CR = GameResult::CashResult;
    BlockchainDigestTransaction result = builder().makeVoteForGameResultTransaction(PlaychainUserId { 168 }, PlaychainUserId { 10 },
                                                                                    PlaychainTableId { 1 },
                                                                                    GameResult { { std::make_pair(PlaychainUserId { 168 }, CR { 150000, 20000 }),
                                                                                                   std::make_pair(PlaychainUserId { 166 }, CR { 0, 0 }),
                                                                                                   std::make_pair(PlaychainUserId { 167 }, CR { 100000, 0 }) },
                                                                                                 "$p1:0:455;p2:1*:1000;p3:2*:1000;p4:3*:1000|D0:10|L1B2F3F0F1M2|W2:15;K0|0:455;1:995;2:1005;3:1000" });

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       69,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "table": "3.4.1",
                                           "table_owner": "1.2.10",
                                           "voter": "1.2.168",
                                           "result": {
                                               "cash": [
                                                   [
                                                       "1.2.166",
                                                       {
                                                           "cash": {
                                                               "amount": 0,
                                                               "asset_id": "1.3.0"
                                                           },
                                                           "rake": {
                                                               "amount": 0,
                                                               "asset_id": "1.3.0"
                                                           }
                                                       }
                                                   ],
                                                   [
                                                       "1.2.167",
                                                       {
                                                           "cash": {
                                                               "amount": 100000,
                                                               "asset_id": "1.3.0"
                                                           },
                                                           "rake": {
                                                               "amount": 0,
                                                               "asset_id": "1.3.0"
                                                           }
                                                       }
                                                   ],
                                                   [
                                                       "1.2.168",
                                                       {
                                                           "cash": {
                                                               "amount": 150000,
                                                               "asset_id": "1.3.0"
                                                           },
                                                           "rake": {
                                                               "amount": 20000,
                                                               "asset_id": "1.3.0"
                                                           }
                                                       }
                                                   ]
                                               ],
                                               "log": "$p1:0:455;p2:1*:1000;p3:2*:1000;p4:3*:1000|D0:10|L1B2F3F0F1M2|W2:15;K0|0:455;1:995;2:1005;3:1000"
                                           }
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "a0f7460a9c3386f18be3b1230ee524daa7bd20c4fe5e7c0176452e966f4fdf03");
}

BOOST_AUTO_TEST_CASE(makeWithdrawPlaychainVestingBalanceTransaction_check)
{
    set_chain_info();

    WithdrawableBalanceInfo info;

    info.referral_balance_id = PlaychainVestingBalanceId { 1 };
    info.referral_balance = 50000;

    BlockchainDigestTransaction result = builder().makeWithdrawPlaychainVestingBalanceTransaction(PlaychainUserId { 168 }, info);

    info = WithdrawableBalanceInfo {};

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                           {
                               "ref_block_num": 37852,
                               "ref_block_prefix": 2318613512,
                               "expiration": "2018-12-06T10:38:20",
                               "operations": [
                                   [
                                       33,
                                       {
                                           "fee": {
                                               "amount": 0,
                                               "asset_id": "1.3.0"
                                           },
                                           "vesting_balance": "1.13.1",
                                           "owner": "1.2.168",
                                           "amount": {
                                               "amount": 50000,
                                               "asset_id": "1.3.0"
                                           }
                                       }
                                   ]
                               ],
                               "extensions": []
                           }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "8f452dadceee34b93dd7e9b8e25ff53aa3d3edaf052233f6793fbcaa45dd5b85");

    info.referral_balance_id = PlaychainVestingBalanceId { 1 };
    info.referral_balance = 50000;
    info.rake_balance_id = PlaychainVestingBalanceId { 2 };
    info.rake_balance = 150000;

    result = builder().makeWithdrawPlaychainVestingBalanceTransaction(PlaychainUserId { 168 }, info);

    info = WithdrawableBalanceInfo {};

    DUMP_JSON(result);

    requied = R"j(
              [
                  "network_broadcast",
                  "broadcast_transaction",
                  [
                      {
                          "ref_block_num": 37852,
                          "ref_block_prefix": 2318613512,
                          "expiration": "2018-12-06T10:38:20",
                          "operations": [
                              [
                                  33,
                                  {
                                      "fee": {
                                          "amount": 0,
                                          "asset_id": "1.3.0"
                                      },
                                      "vesting_balance": "1.13.1",
                                      "owner": "1.2.168",
                                      "amount": {
                                          "amount": 50000,
                                          "asset_id": "1.3.0"
                                      }
                                  }
                              ],
                              [
                                  33,
                                  {
                                      "fee": {
                                          "amount": 0,
                                          "asset_id": "1.3.0"
                                      },
                                      "vesting_balance": "1.13.2",
                                      "owner": "1.2.168",
                                      "amount": {
                                          "amount": 150000,
                                          "asset_id": "1.3.0"
                                      }
                                  }
                              ]
                          ],
                          "extensions": []
                      }
                  ]
              ]
              )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "0a7906e5ba33b822c232d3caaac53c2fe294fbea9f91629df4b442df9ec5f7bf");

    info.referral_balance_id = PlaychainVestingBalanceId { 11 };
    info.referral_balance = 50000;
    info.rake_balance_id = PlaychainVestingBalanceId { 20 };
    info.rake_balance = 150000;
    info.witness_balance_id = PlaychainVestingBalanceId { 3 };
    info.witness_balance = 150000;

    result = builder().makeWithdrawPlaychainVestingBalanceTransaction(PlaychainUserId { 168 }, info);

    DUMP_JSON(result);

    requied = R"j(
              [
                  "network_broadcast",
                  "broadcast_transaction",
                  [
                      {
                          "ref_block_num": 37852,
                          "ref_block_prefix": 2318613512,
                          "expiration": "2018-12-06T10:38:20",
                          "operations": [
                              [
                                  33,
                                  {
                                      "fee": {
                                          "amount": 0,
                                          "asset_id": "1.3.0"
                                      },
                                      "vesting_balance": "1.13.11",
                                      "owner": "1.2.168",
                                      "amount": {
                                          "amount": 50000,
                                          "asset_id": "1.3.0"
                                      }
                                  }
                              ],
                              [
                                  33,
                                  {
                                      "fee": {
                                          "amount": 0,
                                          "asset_id": "1.3.0"
                                      },
                                      "vesting_balance": "1.13.20",
                                      "owner": "1.2.168",
                                      "amount": {
                                          "amount": 150000,
                                          "asset_id": "1.3.0"
                                      }
                                  }
                              ],
                              [
                                  33,
                                  {
                                      "fee": {
                                          "amount": 0,
                                          "asset_id": "1.3.0"
                                      },
                                      "vesting_balance": "1.13.3",
                                      "owner": "1.2.168",
                                      "amount": {
                                          "amount": 150000,
                                          "asset_id": "1.3.0"
                                      }
                                  }
                              ]
                          ],
                          "extensions": []
                      }
                  ]
              ]
              )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "6c77bad55a9c625560d9ffe43d74f9975770c4c681832363227c17c3218298d5");
}

BOOST_AUTO_TEST_CASE(makeBroadcastTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeVoteForStartGameTransaction(PlaychainUserId { 168 }, PlaychainUserId { 10 },
                                                                                   PlaychainTableId { 1 },
                                                                                   GameInitialData { { std::make_pair(PlaychainUserId { 168 }, 100000),
                                                                                                       std::make_pair(PlaychainUserId { 166 }, 50000),
                                                                                                       std::make_pair(PlaychainUserId { 167 }, 100000) },
                                                                                                     "Alice-is-diler" });

    BOOST_REQUIRE(result.valid());

    PlaychainUser alice { "alice", PlaychainUserId { 166 }, "5JTLFAS3YcDyhzm2acyLTsqeA2t2fNrpMPY4dGQCtdf9SUKJZ1U" };
    PlaychainUser bob { "bob", PlaychainUserId { 167 }, "5KeLCuMiCHt9gqAVKUr1imYVDLanFQbiHg95Gf84zzU8Ek2zjuH" };

    BlockchainRequest request = builder().makeBroadcastTransaction(result, alice.signDigest(result.digest()));

    DUMP_JSON(request);

    {
        auto&& signatires = get_signatires_from_params_json(request.params());

        BOOST_CHECK_EQUAL(signatires.size(), 1u);
        BOOST_CHECK(check_signature(convert_signature(signatires[0]), convert_digest(result.digest()), alice.getPublicKey()));
    }

    request = builder().makeBroadcastTransaction(request, bob.signDigest(result.digest()));

    DUMP_JSON(request);

    {
        auto&& signatires = get_signatires_from_params_json(request.params());

        BOOST_CHECK_EQUAL(signatires.size(), 2u);
        BOOST_CHECK(check_signature(convert_signature(signatires[0]), convert_digest(result.digest()), alice.getPublicKey()) || check_signature(convert_signature(signatires[0]), convert_digest(result.digest()), bob.getPublicKey()));
        BOOST_CHECK(check_signature(convert_signature(signatires[1]), convert_digest(result.digest()), alice.getPublicKey()) || check_signature(convert_signature(signatires[1]), convert_digest(result.digest()), bob.getPublicKey()));
    }

    request = builder().makeBroadcastTransaction(request, alice.signDigest(result.digest()));

    DUMP_JSON(request);

    {
        auto&& signatires = get_signatires_from_params_json(request.params());

        BOOST_CHECK_GE(signatires.size(), 2u);
        BOOST_CHECK(check_signature(convert_signature(signatires[0]), convert_digest(result.digest()), alice.getPublicKey()) || check_signature(convert_signature(signatires[0]), convert_digest(result.digest()), bob.getPublicKey()));
        BOOST_CHECK(check_signature(convert_signature(signatires[1]), convert_digest(result.digest()), alice.getPublicKey()) || check_signature(convert_signature(signatires[1]), convert_digest(result.digest()), bob.getPublicKey()));
    }
}

BOOST_AUTO_TEST_CASE(makeSubscribeChangeTableInfoNotificationRequest_check)
{
    auto&& result = builder().makeSubscribeChangeTableInfoNotificationRequest(
        { PlaychainTableId { 1 }, PlaychainTableId { 2 }, PlaychainTableId { 3 } }, 10);

    DUMP_JSON(result.first);

    auto requied = R"j(
                   [
                       "playchain",
                       "set_tables_subscribe_callback",
                       [
                           10,
                           [
                               "3.4.1",
                               "3.4.2",
                               "3.4.3"
                           ]
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.first.str(), utility::remove_formatting(requied));
    BOOST_REQUIRE_GT(result.second, 0);
}

BOOST_AUTO_TEST_CASE(makeCancelSubscriptionForChangeTableInfoNotificationRequest_check)
{
    BlockchainRequest result = builder().makeCancelSubscriptionForChangeTableInfoNotificationRequest(
        { PlaychainTableId { 1 }, PlaychainTableId { 2 } });

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "cancel_tables_subscribe_callback",
                       [
                           [
                               "3.4.1",
                               "3.4.2"
                           ]
                       ]
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeCancelSubscriptionForChangeTableInfoNotificationRequest2_check)
{
    BlockchainRequest result = builder().makeCancelSubscriptionForChangeTableInfoNotificationRequest();

    DUMP_JSON(result);

    auto requied = R"j(
                   [
                       "playchain",
                       "cancel_all_tables_subscribe_callback",
                       []
                   ]
                   )j";

    BOOST_REQUIRE_EQUAL(result.str(), utility::remove_formatting(requied));
}

BOOST_AUTO_TEST_CASE(makeUpdateWitnessTransaction_check)
{
    set_chain_info();

    BlockchainDigestTransaction result = builder().makeUpdateWitnessRequest(PlaychainWitnessId { 2 }, PlaychainUserId { 22 }, "123", "PLC8JybfaeRpmGAq95g5w6mZXRPEEVhjMKsiRZGTcLS46zD5XDTPa");

    DUMP_JSON(result);

    auto requied = R"j(
                     [
                       "network_broadcast",
                       "broadcast_transaction",
                       [
                         {
                           "ref_block_num": 37852,
                           "ref_block_prefix": 2318613512,
                           "expiration": "2018-12-06T10:38:20",
                           "operations": [
                             [
                               21,
                               {
                                 "fee": {
                                   "amount": 0,
                                   "asset_id": "1.3.0"
                                 },
                                 "witness": "1.6.2",
                                 "witness_account": "1.2.22",
                                 "new_url": "123",
                                 "new_signing_key": "PLC8JybfaeRpmGAq95g5w6mZXRPEEVhjMKsiRZGTcLS46zD5XDTPa"
                               }
                             ]
                           ],
                           "extensions": [
                           ]
                         }
                       ]
                   ]
                   )j";

    BOOST_CHECK_EQUAL(result.str(), utility::remove_formatting(requied));
    BOOST_CHECK_EQUAL(result.digest(), "937413e0fe71a97581ec73a67e718cdded13d8affeb1e61c3aa83b34c1a59f0b");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace request_builder_tests
