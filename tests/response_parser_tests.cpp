#include "playchain_tests_common.h"
#include <playchain/playchain_helper.h>
#include <playchain/playchain_totalpoker_helper.h>

namespace response_parser_tests {
using namespace tp;

namespace utility = playchain::test;

BOOST_FIXTURE_TEST_SUITE(response_parser_tests, utility::parser_fixture)

BOOST_AUTO_TEST_CASE(parseGetChainIdResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": "50b3e77b9b27a0a032c2c3ef5d99a0af25d29e99b655710c597f6d297bc6aa28"
                    }
                   )j";

    auto&& result = parser.parseGetChainIdResponse(response);

    BOOST_REQUIRE(result.valid());

    std::string chain_id = result;

    BOOST_CHECK_EQUAL(chain_id, "50b3e77b9b27a0a032c2c3ef5d99a0af25d29e99b655710c597f6d297bc6aa28");
}

BOOST_AUTO_TEST_CASE(parseGetTablesInfoResponse_check)
{
    auto response = R"j(
                    {
                        "id": 111,
                        "jsonrpc": "2.0",
                        "result": [
                            {
                                "id": "3.4.1",
                                "metadata": "{\"bb_price\":100000,\"rake\":400, \"game\":\"TP\",\"info\":\"master\",\"min_bb\":40,\"max_bb\":100,\"chips_bb\":10}",
                                "required_witnesses": 0,
                                "owner": "1.2.10",
                                "owner_name": "andrew",
                                "state": "playing",
                                "server_url": "stage.totalpoker.io:8092",
                                "min_accepted_proposal_asset":
                                {
                                    "amount": 0,
                                    "asset_id": "1.3.0"
                                },
                                "pending_proposals":[],
                                "cash":[
                                        ["1.2.11",
                                        {
                                            "name": "player8",
                                            "amount": {
                                                "amount": 100000,
                                                "asset_id": "1.3.0"
                                            }
                                        }],
                                        ["1.2.12",
                                        {
                                            "name": "player9",
                                            "amount": {
                                                "amount": 60000,
                                                "asset_id": "1.3.0"
                                            }
                                        }]
                                ],
                                "playing_cash":[],
                                "missed_voters": []
                            },
                            {
                                "id": "3.4.2",
                                "metadata": "{\"bb_price\":1000,\"rake\":400,\"game\":\"TP\",\"min_bb\":40,\"max_bb\":100,\"chips_bb\":10}",
                                "required_witnesses": 0,
                                "owner": "1.2.10",
                                "owner_name": "andrew",
                                "state": "free",
                                "server_url": "stage.totalpoker.io:8092",
                                "min_accepted_proposal_asset":
                                {
                                    "amount": 20000,
                                    "asset_id": "1.3.0"
                                },
                                "pending_proposals":[
                                            ["1.2.11",
                                            {
                                                "name": "player8",
                                                "id": "3.7.1",
                                                "uid": "5e52fee47e6b070565f74372468cdc699de89107",
                                                "amount": {
                                                    "amount": 100000,
                                                    "asset_id": "1.3.0"
                                                }
                                            }],
                                            ["1.2.12",
                                            {
                                                "name": "player9",
                                                "id": "3.7.2",
                                                "uid": "80b85ebf641abccdd26e327c5782353137a0a0af",
                                                "amount": {
                                                    "amount": 60000,
                                                    "asset_id": "1.3.0"
                                                }
                                            }]
                                ],
                                "cash":[],
                                "playing_cash":[],
                                "missed_voters": [["1.2.21", "bot1"],["1.2.22", "bot2"]]
                            }
                        ]
                    }
                   )j";

    auto&& result = parser.parseGetTablesInfoResponse(response);

    BOOST_REQUIRE(result.valid());

    std::vector<PlaychainTableInfoExt> tables = result;

    BOOST_CHECK_EQUAL(tables.size(), 2u);

    PlaychainTotalpokerTableInfo tp_table0 = PlaychainTotalpokerTableInfo::create(tables[0], "TP");

    BOOST_CHECK_EQUAL(tp_table0.big_blind_price, 100000);
    BOOST_CHECK_EQUAL((std::string)tables[0].id, (std::string)PlaychainTableId { 1 });
    BOOST_CHECK_EQUAL(tables[0].required_witnesses, 0);
    BOOST_CHECK_EQUAL(tp_table0.info, "master");
    BOOST_CHECK_EQUAL((int)tables[0].state, (int)PlaychainTableInfo::State::GAME);
    BOOST_CHECK_EQUAL((std::string)tables[0].owner, (std::string)PlaychainUserId { 10 });

    PlaychainTotalpokerTableInfo tp_table1 = PlaychainTotalpokerTableInfo::create(tables[1], "TP");

    BOOST_CHECK_EQUAL(tp_table1.big_blind_price, 1000);
    BOOST_CHECK_EQUAL((std::string)tables[1].id, (std::string)PlaychainTableId { 2 });
    BOOST_CHECK_EQUAL(tables[1].required_witnesses, 0);
    BOOST_CHECK_EQUAL(tp_table1.info, "");
    BOOST_CHECK_EQUAL((int)tables[1].state, (int)PlaychainTableInfo::State::NO_GAME);
    BOOST_CHECK_EQUAL((std::string)tables[1].owner, (std::string)PlaychainUserId { 10 });
}

BOOST_AUTO_TEST_CASE(parseCheckIfTableAllocatedForPendingBuyinResponse_check)
{
    auto response = R"j(
                    {
                        "id": 111,
                        "jsonrpc": "2.0",
                        "result":
                        {
                            "id": "3.4.1",
                            "metadata": "{\"bb_price\":100000,\"rake\":450,\"game\":\"TP\",\"info\":\"master\",\"min_bb\":40,\"max_bb\":100,\"chips_bb\":10}",
                            "required_witnesses": 2,
                            "min_accepted_proposal_asset":
                            {
                                "amount": 20000,
                                "asset_id": "1.3.0"
                            },
                            "owner": "1.2.10",
                            "owner_name": "andrew",
                            "state": "free",
                            "server_url": "stage.totalpoker.io:8092"
                        }
                    }
                   )j";

    auto&& result = parser.parseCheckIfTableAllocatedForPendingBuyinResponse(response);

    BOOST_REQUIRE(result.valid());

    PlaychainTableInfo table = result;

    BOOST_CHECK_EQUAL((std::string)table.id, (std::string)PlaychainTableId { 1 });
    BOOST_CHECK_EQUAL(table.required_witnesses, 2);
    BOOST_CHECK_EQUAL((int)table.state, (int)PlaychainTableInfo::State::NO_GAME);
    BOOST_CHECK_EQUAL((std::string)table.owner, (std::string)PlaychainUserId { 10 });
    BOOST_CHECK_EQUAL(table.server_url, "stage.totalpoker.io:8092");
    BOOST_CHECK_EQUAL(PlaychainPLC { table.min_accepted_proposal_asset }, PlaychainPLC::from_string("0.2 PLC"));

    PlaychainTotalpokerTableInfo tp_table = PlaychainTotalpokerTableInfo::create(table, "TP");

    BOOST_CHECK_EQUAL(tp_table.big_blind_price, 100000);
    BOOST_CHECK_EQUAL(tp_table.info, "master");
}

BOOST_AUTO_TEST_CASE(parseGetAccountIdByNameResponse_check)
{
    auto response = R"j(
                    {
                        "id": 1,
                        "jsonrpc": "2.0",
                        "result": [["andrew", "1.2.168"], ["alice", "1.2.169"]]
                    }
                   )j";

    auto&& result = parser.parseGetAccountIdByNameResponse(response);

    BOOST_REQUIRE(result.valid());

    std::map<std::string, PlaychainUserId> accounts = result;

    BOOST_CHECK_EQUAL(accounts.size(), 2u);
    BOOST_CHECK_EQUAL((std::string)accounts["andrew"], (std::string)PlaychainUserId { 168 });
    BOOST_CHECK_EQUAL((std::string)accounts["alice"], (std::string)PlaychainUserId { 169 });
}

BOOST_AUTO_TEST_CASE(parseGetLastIrreversibleBlockHeaderResponse_check)
{
    auto response = R"j(
                    {
                        "id": 1,
                        "jsonrpc": "2.0",
                        "result": {
                            "extensions": [],
                            "previous": "0000cfc6e758632b9bedc535cb00e0c4ec5e82ef",
                            "timestamp": "2018-12-07T14:52:30",
                            "transaction_merkle_root": "0000000000000000000000000000000000000000",
                            "witness": "1.6.1"
                        }
                    }
                   )j";

    auto&& result = parser.parseGetLastIrreversibleBlockHeaderResponse(response);

    BOOST_REQUIRE(result.valid());

    PlaychainBlockHeaderInfo info = result;

    BOOST_CHECK_EQUAL(info.block_num(), 53191);
    BOOST_CHECK_EQUAL(info.timestamp_utc, 1544194350);
}

BOOST_AUTO_TEST_CASE(parseListPlayerInvitationsResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": [
                            [
                                {
                                    "created": "2018-12-10T12:25:40",
                                    "expiration": "2018-12-10T13:25:40",
                                    "id": "3.0.10",
                                    "inviter": "1.2.168",
                                    "metadata": "for-andrew-child1",
                                    "uid": "1"
                                },
                                {
                                    "created": "2018-12-10T12:25:55",
                                    "expiration": "2018-12-10T13:25:55",
                                    "id": "3.0.11",
                                    "inviter": "1.2.168",
                                    "metadata": "for-andrew-child2",
                                    "uid": "2"
                                }
                            ],
                            "2018-12-10T12:27:00"
                        ]
                    }
                   )j";

    auto&& result = parser.parseListPlayerInvitationsResponse(response);

    BOOST_REQUIRE(result.valid());

    std::vector<PlayerInvitationInfo> invitations = result;

    BOOST_CHECK_EQUAL(invitations.size(), 2u);

    BOOST_CHECK_EQUAL((std::string)invitations[0].inviter, (std::string)PlaychainUserId { 168 });
    BOOST_CHECK_EQUAL(invitations[0].uid, "1");
    BOOST_CHECK_EQUAL(invitations[0].metadata, "for-andrew-child1");
    BOOST_CHECK_EQUAL(invitations[0].lifetime_in_sec, 3600u);
    BOOST_CHECK_EQUAL(invitations[0].lifetime_in_sec_left, 3520u);

    BOOST_CHECK_EQUAL((std::string)invitations[1].inviter, (std::string)PlaychainUserId { 168 });
    BOOST_CHECK_EQUAL(invitations[1].uid, "2");
    BOOST_CHECK_EQUAL(invitations[1].metadata, "for-andrew-child2");
    BOOST_CHECK_EQUAL(invitations[1].lifetime_in_sec, 3600u);
    BOOST_CHECK_EQUAL(invitations[1].lifetime_in_sec_left, 3535u);
}

BOOST_AUTO_TEST_CASE(parseListInvitedPlayersResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": [
                            [
                                {
                                    "account": "1.2.169",
                                    "id": "3.1.156",
                                    "inviter": "3.1.155",
                                    "pending_fees": [],
                                    "pending_parent_invitation_fees": []
                                },
                                {
                                    "active": {
                                        "account_auths": [],
                                        "address_auths": [],
                                        "key_auths": [
                                            [
                                                "PLC6aFeQaK2ZGwH7753PYzCJQcZBKYunBvgwe2nD4KpEtLPs3kbTS",
                                                1
                                            ]
                                        ],
                                        "weight_threshold": 1
                                    },
                                    "active_special_authority": [
                                        0,
                                        {}
                                    ],
                                    "blacklisted_accounts": [],
                                    "blacklisting_accounts": [],
                                    "id": "1.2.169",
                                    "lifetime_referrer": "1.2.4",
                                    "lifetime_referrer_fee_percentage": 3000,
                                    "membership_expiration_date": "1970-01-01T00:00:00",
                                    "name": "andrew-child1",
                                    "network_fee_percentage": 2000,
                                    "options": {
                                        "extensions": [],
                                        "memo_key": "PLC6aFeQaK2ZGwH7753PYzCJQcZBKYunBvgwe2nD4KpEtLPs3kbTS",
                                        "num_committee": 0,
                                        "num_witness": 0,
                                        "votes": [],
                                        "voting_account": "1.2.5"
                                    },
                                    "owner": {
                                        "account_auths": [],
                                        "address_auths": [],
                                        "key_auths": [
                                            [
                                                "PLC6aFeQaK2ZGwH7753PYzCJQcZBKYunBvgwe2nD4KpEtLPs3kbTS",
                                                1
                                            ]
                                        ],
                                        "weight_threshold": 1
                                    },
                                    "owner_special_authority": [
                                        0,
                                        {}
                                    ],
                                    "referrer": "1.2.4",
                                    "referrer_rewards_percentage": 0,
                                    "registrar": "1.2.168",
                                    "statistics": "2.6.169",
                                    "top_n_control_flags": 0,
                                    "whitelisted_accounts": [],
                                    "whitelisting_accounts": []
                                }
                            ],
                            [
                                {
                                    "account": "1.2.170",
                                    "id": "3.1.157",
                                    "inviter": "3.1.155",
                                    "pending_fees": [],
                                    "pending_parent_invitation_fees": []
                                },
                                {
                                    "active": {
                                        "account_auths": [],
                                        "address_auths": [],
                                        "key_auths": [
                                            [
                                                "PLC7j7RvG2zGdFe2i21SbbcuJHbznYNRQCXtS7tS7APyY8UkzZqfG",
                                                1
                                            ]
                                        ],
                                        "weight_threshold": 1
                                    },
                                    "active_special_authority": [
                                        0,
                                        {}
                                    ],
                                    "blacklisted_accounts": [],
                                    "blacklisting_accounts": [],
                                    "id": "1.2.170",
                                    "lifetime_referrer": "1.2.4",
                                    "lifetime_referrer_fee_percentage": 3000,
                                    "membership_expiration_date": "1970-01-01T00:00:00",
                                    "name": "andrew-child2",
                                    "network_fee_percentage": 2000,
                                    "options": {
                                        "extensions": [],
                                        "memo_key": "PLC7j7RvG2zGdFe2i21SbbcuJHbznYNRQCXtS7tS7APyY8UkzZqfG",
                                        "num_committee": 0,
                                        "num_witness": 0,
                                        "votes": [],
                                        "voting_account": "1.2.5"
                                    },
                                    "owner": {
                                        "account_auths": [],
                                        "address_auths": [],
                                        "key_auths": [
                                            [
                                                "PLC7j7RvG2zGdFe2i21SbbcuJHbznYNRQCXtS7tS7APyY8UkzZqfG",
                                                1
                                            ]
                                        ],
                                        "weight_threshold": 1
                                    },
                                    "owner_special_authority": [
                                        0,
                                        {}
                                    ],
                                    "referrer": "1.2.4",
                                    "referrer_rewards_percentage": 0,
                                    "registrar": "1.2.168",
                                    "statistics": "2.6.170",
                                    "top_n_control_flags": 0,
                                    "whitelisted_accounts": [],
                                    "whitelisting_accounts": []
                                }
                            ]
                        ]
                    }
                   )j";

    auto&& result = parser.parseListInvitedPlayersResponse(response);

    BOOST_REQUIRE(result.valid());

    std::vector<InvitedPlayerInfo> invitations = result;

    BOOST_CHECK_EQUAL(invitations.size(), 2u);

    BOOST_CHECK_EQUAL((std::string)invitations[0].id, (std::string)PlaychainUserId { 169 });
    BOOST_CHECK_EQUAL(invitations[0].name, "andrew-child1");

    BOOST_CHECK_EQUAL((std::string)invitations[1].id, (std::string)PlaychainUserId { 170 });
    BOOST_CHECK_EQUAL(invitations[1].name, "andrew-child2");
}

BOOST_AUTO_TEST_CASE(parseListPlayerInvitationsResponse_negative_check)
{
    auto invalid_json_response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "resu2-10T12:27:00"
                        ]
                   )j";

    BOOST_CHECK(!parser.parseListPlayerInvitationsResponse(invalid_json_response).valid());

    auto invalid_logic_response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": [
                            {
                                "created": "2018-12-10T12:25:40",
                                "expiration": "2018-12-10T13:25:40",
                                "id": "3.0.10",
                                "inviter": "1.2.168",
                                "metadata": "for-andrew-child1",
                                "uid": "1"
                            },
                            {
                                "created": "2018-12-10T12:25:55",
                                "expiration": "2018-12-10T13:25:55",
                                "id": "3.0.11",
                                "inviter": "1.2.168",
                                "metadata": "for-andrew-child2",
                                "uid": "2"
                            }
                        ]
                    }
                   )j";

    BOOST_CHECK(!parser.parseListPlayerInvitationsResponse(invalid_logic_response).valid());
}

BOOST_AUTO_TEST_CASE(parseGetPlaychainBalanceResponse_check)
{
    auto response = R"j(
                    {
                        "id": 2,
                        "jsonrpc": "2.0",
                        "result": {
                            "account_balance": {
                                "amount": 91850000,
                                "asset_id": "1.3.0"
                            },
                            "rake_balance": {
                                "amount": 0,
                                "asset_id": "1.3.0"
                            },
                            "referral_balance": {
                                "amount": 765,
                                "asset_id": "1.3.0"
                            },
                            "referral_balance_id": "1.13.4",
                            "witness_balance": {
                                "amount": 0,
                                "asset_id": "1.3.0"
                            }
                        }
                    }
                   )j";

    {
        auto&& result = parser.parseGetPlaychainBalanceResponse(response);

        BOOST_REQUIRE(result.valid());

        PlaychainUserBalanceInfo info = result;

        BOOST_CHECK_EQUAL(info.referral_balance, 765);
        BOOST_CHECK_EQUAL(info.rake_balance, 0);
        BOOST_CHECK_EQUAL(info.witness_balance, 0);

        BOOST_CHECK_EQUAL((std::string)info.referral_balance_id, (std::string)PlaychainVestingBalanceId { 4 });
    }

    response = R"j(
               {
                   "id": 2,
                   "jsonrpc": "2.0",
                   "result": {
                       "account_balance": {
                           "amount": "9898765714",
                           "asset_id": "1.3.0"
                       },
                       "rake_balance": {
                           "amount": 6934,
                           "asset_id": "1.3.0"
                       },
                       "rake_balance_id": "1.13.5",
                       "referral_balance": {
                           "amount": 0,
                           "asset_id": "1.3.0"
                       },
                       "witness_balance": {
                           "amount": 0,
                           "asset_id": "1.3.0"
                       }
                   }
               }
               )j";

    {
        auto&& result = parser.parseGetPlaychainBalanceResponse(response);

        BOOST_REQUIRE(result.valid());

        PlaychainUserBalanceInfo info = result;

        BOOST_CHECK_EQUAL(info.referral_balance, 0);
        BOOST_CHECK_EQUAL(info.rake_balance, 6934);
        BOOST_CHECK_EQUAL(info.witness_balance, 0);

        BOOST_CHECK_EQUAL((std::string)info.rake_balance_id, (std::string)PlaychainVestingBalanceId { 5 });
    }
}

BOOST_AUTO_TEST_CASE(parseLoginResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": {
                            "account": "1.2.167",
                            "player": "3.1.18",
                            "login_key": "PLC5LV8gTvQZt754JiWiA8tH9Epr5A9NyNjDFTaV4zn9ZTMbAHM1m"
                        }
                    }
                   )j";

    auto&& result = parser.parseLoginResponse(response);

    BOOST_REQUIRE(result.valid());

    std::pair<PlaychainUserId, CompressedPublicKey> login_data = result;

    BOOST_CHECK_EQUAL((std::string)login_data.first, (std::string)PlaychainUserId(167));
    BOOST_CHECK_EQUAL(public_key_to_string(login_data.second), "PLC5LV8gTvQZt754JiWiA8tH9Epr5A9NyNjDFTaV4zn9ZTMbAHM1m");
}

BOOST_AUTO_TEST_CASE(parseLegacyLoginResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": false
                    }
                   )j";

    auto&& result = parser.parseLegacyLoginResponse(response);

    BOOST_REQUIRE(result.valid());

    bool registered = result;

    BOOST_CHECK(!registered);
}

BOOST_AUTO_TEST_CASE(parseLegacyGetAccountBalanceResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": [
                            {
                                "amount": 150000,
                                "asset_id": "1.3.0"
                            }
                        ]
                    }
                   )j";

    {
        auto&& result = parser.parseLegacyGetAccountBalanceResponse(response);

        BOOST_REQUIRE(result.valid());

        PlaychainMoney balance = result;

        BOOST_CHECK_EQUAL(balance, 150000);
    }

    response = R"j(
                        {
                            "id": 0,
                            "jsonrpc": "2.0",
                            "result": []
                        }
                       )j";

    {
        auto&& result = parser.parseLegacyGetAccountBalanceResponse(response);

        BOOST_REQUIRE(result.valid());

        PlaychainMoney balance = result;

        BOOST_CHECK_EQUAL(balance, 0);
    }

    response = R"j(
               {
                   "id": 0,
                   "jsonrpc": "2.0",
                   "result": [
                       {
                           "amount": 150000,
                           "asset_id": "1.3.1"
                       }
                   ]
               }
               )j";

    //invalid asset Id
    BOOST_REQUIRE(!parser.parseLegacyGetAccountBalanceResponse(response).valid());
}

BOOST_AUTO_TEST_CASE(parseLegacyGetAccountIdByNameResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": [
                            {
                                "active": {
                                    "account_auths": [],
                                    "address_auths": [],
                                    "key_auths": [
                                        [
                                            "PLC723RwR7tyqxfyoA3oRfDxBNWypJYZohS8aNwbo5CWNGfqDkjrX",
                                            1
                                        ]
                                    ],
                                    "weight_threshold": 1
                                },
                                "active_special_authority": [
                                    0,
                                    {}
                                ],
                                "blacklisted_accounts": [],
                                "blacklisting_accounts": [],
                                "cashback_vb": "1.13.0",
                                "id": "1.2.10",
                                "lifetime_referrer": "1.2.10",
                                "lifetime_referrer_fee_percentage": 8000,
                                "membership_expiration_date": "1969-12-31T23:59:59",
                                "name": "totalpoker",
                                "network_fee_percentage": 2000,
                                "options": {
                                    "extensions": [],
                                    "memo_key": "PLC723RwR7tyqxfyoA3oRfDxBNWypJYZohS8aNwbo5CWNGfqDkjrX",
                                    "num_committee": 0,
                                    "num_witness": 0,
                                    "votes": [],
                                    "voting_account": "1.2.5"
                                },
                                "owner": {
                                    "account_auths": [],
                                    "address_auths": [],
                                    "key_auths": [
                                        [
                                            "PLC723RwR7tyqxfyoA3oRfDxBNWypJYZohS8aNwbo5CWNGfqDkjrX",
                                            1
                                        ]
                                    ],
                                    "weight_threshold": 1
                                },
                                "owner_special_authority": [
                                    0,
                                    {}
                                ],
                                "referrer": "1.2.10",
                                "referrer_rewards_percentage": 0,
                                "registrar": "1.2.10",
                                "statistics": "2.6.10",
                                "top_n_control_flags": 0,
                                "whitelisted_accounts": [],
                                "whitelisting_accounts": []
                            },
                            {
                                "active": {
                                    "account_auths": [],
                                    "address_auths": [],
                                    "key_auths": [
                                        [
                                            "PLC5LV8gTvQZt754JiWiA8tH9Epr5A9NyNjDFTaV4zn9ZTMbAHM1m",
                                            1
                                        ]
                                    ],
                                    "weight_threshold": 1
                                },
                                "active_special_authority": [
                                    0,
                                    {}
                                ],
                                "blacklisted_accounts": [],
                                "blacklisting_accounts": [],
                                "id": "1.2.167",
                                "lifetime_referrer": "1.2.10",
                                "lifetime_referrer_fee_percentage": 3000,
                                "membership_expiration_date": "1970-01-01T00:00:00",
                                "name": "alice",
                                "network_fee_percentage": 2000,
                                "options": {
                                    "extensions": [],
                                    "memo_key": "PLC5LV8gTvQZt754JiWiA8tH9Epr5A9NyNjDFTaV4zn9ZTMbAHM1m",
                                    "num_committee": 0,
                                    "num_witness": 0,
                                    "votes": [],
                                    "voting_account": "1.2.5"
                                },
                                "owner": {
                                    "account_auths": [],
                                    "address_auths": [],
                                    "key_auths": [
                                        [
                                            "PLC5LV8gTvQZt754JiWiA8tH9Epr5A9NyNjDFTaV4zn9ZTMbAHM1m",
                                            1
                                        ]
                                    ],
                                    "weight_threshold": 1
                                },
                                "owner_special_authority": [
                                    0,
                                    {}
                                ],
                                "referrer": "1.2.10",
                                "referrer_rewards_percentage": 0,
                                "registrar": "1.2.10",
                                "statistics": "2.6.167",
                                "top_n_control_flags": 0,
                                "whitelisted_accounts": [],
                                "whitelisting_accounts": []
                            }
                        ]
                    }
                   )j";

    auto&& result = parser.parseLegacyGetAccountIdByNameResponse(response);

    BOOST_REQUIRE(result.valid());

    std::map<std::string, PlaychainUserId> accounts = result;

    BOOST_CHECK_EQUAL(accounts.size(), 2u);
    BOOST_CHECK_EQUAL((std::string)accounts["totalpoker"], (std::string)PlaychainUserId { 10 });
    BOOST_CHECK_EQUAL((std::string)accounts["alice"], (std::string)PlaychainUserId { 167 });
}

BOOST_AUTO_TEST_CASE(parseLegacyGetLastBlockHeaderResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": {
                            "accounts_registered_this_interval": 0,
                            "current_aslot": 133430,
                            "current_witness": "1.6.1",
                            "dynamic_flags": 0,
                            "head_block_id": "0001fe6381f7a962275b89e9f4b63430edaef6c9",
                            "head_block_number": 130659,
                            "id": "2.1.0",
                            "last_budget_time": "2018-12-13T14:21:00",
                            "last_irreversible_block_num": 130659,
                            "next_maintenance_time": "2018-12-13T14:22:00",
                            "recent_slots_filled": "340282366920938463463374607431768211455",
                            "recently_missed_count": 0,
                            "time": "2018-12-13T14:21:40",
                            "witness_budget": 0
                        }
                    }

                   )j";

    auto&& result = parser.parseLegacyGetLastBlockHeaderResponse(response);

    BOOST_REQUIRE(result.valid());

    PlaychainBlockHeaderInfo info = result;

    BOOST_CHECK_EQUAL(info.block_num(), 130660);
    BOOST_CHECK_EQUAL(info.timestamp_utc, 1544710900);
}

BOOST_AUTO_TEST_CASE(parseTransactionResponse_check)
{
    auto response = R"j(
                    {
                        "id": 0,
                        "jsonrpc": "2.0",
                        "result": null
                    }
                   )j";

    auto&& result = parser.parseTransactionResponse(response);

    BOOST_REQUIRE(result.valid());
    BOOST_REQUIRE((bool)result);
}

BOOST_AUTO_TEST_CASE(parseChangeTableInfoNotification_check)
{
    auto response = R"j(
                    {
                        "method": "notice",
                        "params": [
                            10, [[
                            {
                                "id": "3.4.1",
                                "metadata": "{\"bb_price\":100000,\"rake\":450,\"game\":\"TP\",\"info\":\"master\",\"min_bb\":40,\"max_bb\":100,\"chips_bb\":10}",
                                "required_witnesses": 0,
                                "state": "free",
                                "owner": "1.2.10",
                                "owner_name": "andrew",
                                "server_url": "stage.totalpoker.io:8092",
                                "min_accepted_proposal_asset":
                                {
                                    "amount": 10000,
                                    "asset_id": "1.3.0"
                                },
                                "pending_proposals":[],
                                "cash":[],
                                "playing_cash":[],
                                "missed_voters":[]
                            },
                            {
                                "id": "3.4.2",
                                "metadata": "{\"bb_price\":1000,\"rake\":450,\"game\":\"TP\",\"min_bb\":40,\"max_bb\":100,\"chips_bb\":10}",
                                "required_witnesses": 0,
                                "state": "free",
                                "owner": "1.2.11",
                                "owner_name": "andrew",
                                "server_url": "stage.totalpoker.io:8092",
                                "min_accepted_proposal_asset":
                                {
                                    "amount": 20000,
                                    "asset_id": "1.3.0"
                                },
                                "pending_proposals":[],
                                "cash":[],
                                "playing_cash":[],
                                "missed_voters":[]
                            }]
                        ]]
                    }
                   )j";

    auto&& result = parser.parseChangeTableInfoNotification(response, 10);

    BOOST_REQUIRE(result.valid());

    std::vector<PlaychainTableInfoExt> tables = result;

    BOOST_CHECK_EQUAL(tables.size(), 2u);

    PlaychainTotalpokerTableInfo tp_table0 = PlaychainTotalpokerTableInfo::create(tables[0], "TP");

    BOOST_CHECK_EQUAL(tp_table0.big_blind_price, 100000);
    BOOST_CHECK_EQUAL((std::string)tables[0].id, (std::string)PlaychainTableId { 1 });
    BOOST_CHECK_EQUAL(tables[0].required_witnesses, 0);
    BOOST_CHECK_EQUAL(tp_table0.info, "master");
    BOOST_CHECK_EQUAL((int)tables[0].state, (int)PlaychainTableInfo::State::NO_GAME);

    PlaychainTotalpokerTableInfo tp_table1 = PlaychainTotalpokerTableInfo::create(tables[1], "TP");

    BOOST_CHECK_EQUAL(tp_table1.big_blind_price, 1000);
    BOOST_CHECK_EQUAL((std::string)tables[1].id, (std::string)PlaychainTableId { 2 });
    BOOST_CHECK_EQUAL(tables[1].required_witnesses, 0);
    BOOST_CHECK_EQUAL(tp_table1.info, "");
    BOOST_CHECK_EQUAL((int)tables[1].state, (int)PlaychainTableInfo::State::NO_GAME);
}

BOOST_AUTO_TEST_CASE(parsePlaychainSettingFromProperties_check)
{
    auto blockchain_response = R"j(
                               {
                                   "id": 0,
                                   "jsonrpc": "2.0",
                                   "result": {
                                       "id": "2.0.0",
                                       "parameters": {
                                           "current_fees": {
                                               "parameters": [
                                                   [
                                                       0,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       1,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       2,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       3,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       4,
                                                       {}
                                                   ],
                                                   [
                                                       5,
                                                       {
                                                           "basic_fee": 10000,
                                                           "premium_fee": 10000,
                                                           "price_per_kbyte": 100
                                                       }
                                                   ],
                                                   [
                                                       6,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       7,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       8,
                                                       {
                                                           "membership_annual_fee": 10000,
                                                           "membership_lifetime_fee": 20000
                                                       }
                                                   ],
                                                   [
                                                       9,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       10,
                                                       {
                                                           "symbol3": 0,
                                                           "symbol4": 0,
                                                           "long_symbol": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       11,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       12,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       13,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       14,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       15,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       16,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       17,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       18,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       19,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       20,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       21,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       22,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       23,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       24,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       25,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       26,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       27,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       28,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       29,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       30,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       31,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       32,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       33,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       34,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       35,
                                                       {
                                                           "fee": 999,
                                                           "price_per_kbyte": 9
                                                       }
                                                   ],
                                                   [
                                                       36,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       37,
                                                       {}
                                                   ],
                                                   [
                                                       38,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       39,
                                                       {
                                                           "fee": 0,
                                                           "price_per_output": 0
                                                       }
                                                   ],
                                                   [
                                                       40,
                                                       {
                                                           "fee": 0,
                                                           "price_per_output": 0
                                                       }
                                                   ],
                                                   [
                                                       41,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       42,
                                                       {}
                                                   ],
                                                   [
                                                       43,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       44,
                                                       {}
                                                   ],
                                                   [
                                                       45,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       46,
                                                       {}
                                                   ],
                                                   [
                                                       47,
                                                       {}
                                                   ],
                                                   [
                                                       48,
                                                       {}
                                                   ],
                                                   [
                                                       49,
                                                       {}
                                                   ],
                                                   [
                                                       50,
                                                       {}
                                                   ],
                                                   [
                                                       51,
                                                       {}
                                                   ],
                                                   [
                                                       52,
                                                       {}
                                                   ],
                                                   [
                                                       53,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       54,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       55,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       56,
                                                       {
                                                           "fee": 25000,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       57,
                                                       {}
                                                   ],
                                                   [
                                                       58,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       59,
                                                       {}
                                                   ],
                                                   [
                                                       60,
                                                       {}
                                                   ],
                                                   [
                                                       61,
                                                       {}
                                                   ],
                                                   [
                                                       62,
                                                       {
                                                           "fee": 10000,
                                                           "price_per_kbyte": 100
                                                       }
                                                   ],
                                                   [
                                                       63,
                                                       {
                                                           "fee": 5000,
                                                           "price_per_kbyte": 200
                                                       }
                                                   ],
                                                   [
                                                       64,
                                                       {
                                                           "fee": 10000,
                                                           "price_per_kbyte": 100
                                                       }
                                                   ],
                                                   [
                                                       65,
                                                       {
                                                           "fee": 5000,
                                                           "price_per_kbyte": 100
                                                       }
                                                   ],
                                                   [
                                                       66,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       67,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       68,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       69,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       70,
                                                       {}
                                                   ],
                                                   [
                                                       71,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       72,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       73,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       74,
                                                       {
                                                           "fee": 0,
                                                           "price_per_kbyte": 0
                                                       }
                                                   ],
                                                   [
                                                       75,
                                                       {
                                                           "fee": 0
                                                       }
                                                   ],
                                                   [
                                                       76,
                                                       {}
                                                   ],
                                                   [
                                                       77,
                                                       {}
                                                   ],
                                                   [
                                                       78,
                                                       {
                                                           "fee": 25000
                                                       }
                                                   ]
                                               ],
                                               "scale": 10000
                                           },
                                           "block_interval": 3,
                                           "maintenance_interval": 60,
                                           "maintenance_skip_slots": 2,
                                           "committee_proposal_review_period": 3600,
                                           "maximum_transaction_size": 98304,
                                           "maximum_block_size": 2097152,
                                           "maximum_time_until_expiration": 86400,
                                           "maximum_proposal_lifetime": 2419200,
                                           "maximum_asset_whitelist_authorities": 10,
                                           "maximum_asset_feed_publishers": 10,
                                           "maximum_witness_count": 1001,
                                           "maximum_committee_count": 1001,
                                           "maximum_authority_membership": 10,
                                           "reserve_percent_of_fee": 0,
                                           "network_percent_of_fee": 10000,
                                           "lifetime_referrer_percent_of_fee": 0,
                                           "cashback_vesting_period_seconds": 600,
                                           "cashback_vesting_threshold": 1000000,
                                           "count_non_member_votes": true,
                                           "allow_non_member_whitelists": false,
                                           "witness_pay_per_block": 150000,
                                           "worker_budget_per_day": "50000000000",
                                           "max_predicate_opcode": 1,
                                           "fee_liquidation_threshold": 0,
                                           "accounts_per_fee_scale": 1000,
                                           "account_fee_scale_bitshifts": 4,
                                           "max_authority_depth": 2,
                                           "extensions": []
                                       },
                                       "next_available_vote_id": 2,
                                       "active_committee_members": [
                                           "1.5.0"
                                       ],
                                       "active_witnesses": [
                                           "1.6.1"
                                       ]
                                   }
                               }
                   )j";

    auto playchain_response = R"j(
                              {
                                  "id": 0,
                                  "jsonrpc": "2.0",
                                  "result": {
                                      "id": "3.8.0",
                                      "parameters": {
                                          "player_referrer_percent_of_fee": 2500,
                                          "player_referrer_parent_percent_of_fee": 5000,
                                          "player_referrer_balance_min_threshold": 1000000,
                                          "player_referrer_balance_max_threshold": 100000000,
                                          "game_witness_percent_of_fee": 2500,
                                          "take_into_account_graphene_balances": false,
                                          "voting_for_playing_expiration_seconds": 60,
                                          "voting_for_results_expiration_seconds": 60,
                                          "voting_for_playing_requied_percent": 6000,
                                          "voting_for_results_requied_percent": 6000,
                                          "game_lifetime_limit_in_seconds": 86400,
                                          "pending_buyin_proposal_lifetime_limit_in_seconds": 100,
                                          "amount_reserving_places_per_user": 1,
                                          "pending_buy_in_allocate_per_block": 300,
                                          "rooms_rating_recalculations_per_maintenance": 1000,
                                          "tables_weight_recalculations_per_maintenance": 1000,
                                          "max_allowed_table_weight_to_be_allocated": 0,
                                          "percentage_of_voter_witness_substitution_while_voting_for_playing": 0,
                                          "percentage_of_voter_witness_substitution_while_voting_for_results": 5000,
                                          "allowed_missed_players": 2,
                                          "extensions": []
                                      },
                                      "active_games_committee_members": []
                                  }
                              }
                               )j";

    auto&& result = parser.parsePlaychainSettingFromProperties(blockchain_response, playchain_response);

    BOOST_REQUIRE(result.valid());

    PlaychainSettings settings = result;

    BOOST_CHECK_EQUAL(PlaychainPLC { settings.fee_create_account_with_public_key }, PlaychainPLC::from_string("0.1 PLC"));
    BOOST_CHECK_EQUAL(PlaychainPLC { settings.fee_create_account_with_public_key_price_per_kbyte }, PlaychainPLC::from_string("0.001 PLC"));
    BOOST_CHECK_EQUAL(PlaychainPLC { settings.fee_update_room }, PlaychainPLC::from_string("0.05 PLC"));
    BOOST_CHECK_EQUAL(PlaychainPLC { settings.fee_update_room_price_per_kbyte }, PlaychainPLC::from_string("0.002 PLC"));

    BOOST_CHECK_EQUAL(settings.pending_buyin_proposal_lifetime_limit_sec, 100u);
    BOOST_CHECK_EQUAL(settings.block_interval_sec, 3u);
}


BOOST_AUTO_TEST_SUITE_END()
} // namespace response_parser_tests
