#include "playchain_tests_common.h"

#include "../src/convert_helper.h"
#include <playchain/playchain_helper.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace playchain {
namespace test {
    void builder_fixture::set_chain_info()
    {
        BlockIdType block_id;
        playchain::from_hex("000093dc083c338a09879291cd40268a2460f23c", block_id);
        PlaychainBlockHeaderInfo block { block_id, playchain::from_iso_string("2018-12-06T10:28:20") };

        BOOST_REQUIRE_EQUAL(block.timestamp_utc, 1544092100);
        BOOST_REQUIRE_EQUAL(block.block_num(), 37853u);

        BOOST_REQUIRE(_builder);
        BOOST_REQUIRE_NO_THROW(_builder->setChainInfo(block));
    }

    std::vector<std::string> builder_fixture::get_signatires_from_params_json(const std::string& json)
    {
        std::vector<std::string> result;

        rapidjson::Document document;
        document.Parse(json.c_str());

        BOOST_REQUIRE(!document.HasParseError());
        BOOST_REQUIRE(document.IsArray());
        BOOST_REQUIRE(document.GetArray().Size() > 0);
        BOOST_REQUIRE(document.GetArray()[0].IsObject());

        auto&& json_object = document.GetArray()[0].GetObject();

        BOOST_REQUIRE(json_object.HasMember("signatures"));
        BOOST_REQUIRE(json_object["signatures"].IsArray());

        for (const auto& sig : json_object["signatures"].GetArray())
        {
            PLAYCHAIN_ASSERT_JSON(sig.IsString());

            result.emplace_back(sig.GetString());
        }

        return result;
    }

    std::string format_json(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());

        BOOST_REQUIRE(!document.HasParseError());

        rapidjson::StringBuffer buff;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);

        document.Accept(writer);

        return buff.GetString();
    }

    std::string remove_formatting(const std::string& json)
    {
        std::string ret;
        ret.reserve(json.size());

        for (const auto& ch : json)
        {
            switch (ch)
            {
            case ' ':
            case '\t':
            case '\n':
                continue;
            default:
                break;
            }
            ret.push_back(ch);
        }

        return ret;
    }

    BOOST_AUTO_TEST_SUITE(playchain_tests_common)

    BOOST_AUTO_TEST_CASE(time_conversions_check)
    {
        time_t timestamp_utc = playchain::from_iso_string("2018-12-06T10:28:20");

        BOOST_CHECK_EQUAL(timestamp_utc, 1544092100);
        BOOST_CHECK_EQUAL(playchain::to_iso_string(timestamp_utc), "2018-12-06T10:28:20");
    }

    BOOST_AUTO_TEST_CASE(get_signatires_from_params_json_check)
    {
        builder_fixture fx;

        auto json = R"j(
                       [{
                         "ref_block_num": 57,
                         "ref_block_prefix": 3056275158,
                         "expiration": "2018-11-29T15:27:35",
                         "operations": [[
                             52,{
                               "fee": {
                                 "amount": 25000,
                                 "asset_id": "1.3.0"
                               },
                               "inviter": "1.2.7",
                               "uid": "1",
                               "lifetime_in_sec": 600,
                               "metadata": ""
                             }
                           ]
                         ],
                         "extensions": [],
                         "signatures": [
                           "1f2971e500e4450a7564c7b27ebcd0640de055ce1a827dd7e6cffe4042925cf53b416da46eb631826d2208980e8f37cde3b225954c07ede15bb0f58756df5e4101"
                         ]
                       }]
                       )j";

        {
            auto&& signatires = fx.get_signatires_from_params_json(json);
            BOOST_REQUIRE_EQUAL(signatires.size(), 1u);
            BOOST_CHECK_EQUAL(signatires[0], "1f2971e500e4450a7564c7b27ebcd0640de055ce1a827dd7e6cffe4042925cf53b416da46eb631826d2208980e8f37cde3b225954c07ede15bb0f58756df5e4101");
        }

        json = R"j(
                       [{
                         "ref_block_num": 57,
                         "ref_block_prefix": 3056275158,
                         "expiration": "2018-11-29T15:27:35",
                         "operations": [[
                             52,{
                               "fee": {
                                 "amount": 25000,
                                 "asset_id": "1.3.0"
                               },
                               "inviter": "1.2.7",
                               "uid": "1",
                               "lifetime_in_sec": 600,
                               "metadata": ""
                             }
                           ]
                         ],
                         "extensions": [],
                         "signatures": [
                           "1f2971e500e4450a7564c7b27ebcd0640de055ce1a827dd7e6cffe4042925cf53b416da46eb631826d2208980e8f37cde3b225954c07ede15bb0f58756df5e4101",
                           "204fc844ce1871ed02e1edd1407cd61645c6759fe0ceb4cc8810566d9ab84fe8f06c041aa8e68599159c82c7012d4fa09b17e8124686dfef2c17229a1cb225bbfb"
                         ]
                       }]
                       )j";

        {
            auto&& signatires = fx.get_signatires_from_params_json(json);
            BOOST_REQUIRE_EQUAL(signatires.size(), 2u);
            BOOST_CHECK_EQUAL(signatires[0], "1f2971e500e4450a7564c7b27ebcd0640de055ce1a827dd7e6cffe4042925cf53b416da46eb631826d2208980e8f37cde3b225954c07ede15bb0f58756df5e4101");
            BOOST_CHECK_EQUAL(signatires[1], "204fc844ce1871ed02e1edd1407cd61645c6759fe0ceb4cc8810566d9ab84fe8f06c041aa8e68599159c82c7012d4fa09b17e8124686dfef2c17229a1cb225bbfb");
        }

        json = R"j(
                       [{
                         "ref_block_num": 57,
                         "ref_block_prefix": 3056275158,
                         "expiration": "2018-11-29T15:27:35",
                         "operations": [[
                             52,{
                               "fee": {
                                 "amount": 25000,
                                 "asset_id": "1.3.0"
                               },
                               "inviter": "1.2.7",
                               "uid": "1",
                               "lifetime_in_sec": 600,
                               "metadata": ""
                             }
                           ]
                         ],
                         "extensions": [],
                         "signatures": [
                         ]
                       }]
                       )j";

        {
            auto&& signatires = fx.get_signatires_from_params_json(json);
            BOOST_REQUIRE_EQUAL(signatires.size(), 0u);
        }
    }

    BOOST_AUTO_TEST_CASE(playchain_plc_check)
    {
        BOOST_CHECK_EQUAL(PlaychainPLC { 125000 }.amount(), 125000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("1.25 PLC").amount(), 125000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("23 PLC").amount(), 2300000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10 PLC").amount(), 1000000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("670.01 PLC").amount(), 67001000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("0.00001 PLC").amount(), 1);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("0.0000000000001 PLC").amount(), 0);

        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("1 PLC").str(), "1 PLC");
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("67000 PLC").str(), "67000 PLC");
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("670.01 PLC").str(), "670.01 PLC");
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("2.15 PLC").str(), "2.15 PLC");
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("0.00015 PLC").str(), "0.00015 PLC");

        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10         PLC").amount(), 1000000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10\tPLC").amount(), 1000000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10   \t PLC").amount(), 1000000);
        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10   \n PLC").amount(), 1000000);

        BOOST_CHECK_EQUAL(PlaychainPLC::from_string("10,50 PLC").amount(), 1050000);

        BOOST_CHECK_NE(PlaychainPLC::from_string("a10 PLC").amount(), 1000000);
    }

    BOOST_AUTO_TEST_CASE(key_conversions_check)
    {
        auto&& priv_key = priv_key_from_brain_key("ACHER APHASIC STAP SEMBLE WAVICLE HAGWEED JUNIPER DARAT FOSSOR TOOL KOLKHOS CONNATE GILLING NOVA UNBORN HIERON");

        auto&& with_priv_key = priv_key_to_wif(priv_key);

        BOOST_CHECK_EQUAL(with_priv_key, "5KLE3YefQDVK4rJjPmyU7YZRbvqkofSs1vqZwgSkU7vN4SGJBkH");

        BOOST_CHECK(priv_key_from_wif(with_priv_key) == priv_key);

        auto&& pub_key = public_key_from_key(priv_key);

        auto&& wellformatted_pub_key = public_key_to_string(pub_key);

        BOOST_CHECK_EQUAL(wellformatted_pub_key, "PLC7SijQHeekG9yYLK8cxwM9GRCiqbyUSubRsk4uMj2tifUdt3nYZ");

        BOOST_CHECK(public_key_from_string(wellformatted_pub_key) == pub_key);

        auto&& hex_pub_key = public_key_to_string(pub_key, false);

        BOOST_CHECK_EQUAL(hex_pub_key, "0350936a99e896ac4d6f365ca24a60703a30548c266bdab063fed15deab34167c2");

        BOOST_CHECK(public_key_from_string(hex_pub_key) == pub_key);
    }

    BOOST_AUTO_TEST_CASE(sign_digest_with_helper_and_check)
    {
        auto&& brain_key = priv_key_from_brain_key("ACHER APHASIC STAP SEMBLE WAVICLE HAGWEED JUNIPER DARAT FOSSOR TOOL KOLKHOS CONNATE GILLING NOVA UNBORN HIERON");

        auto&& with_priv_key = priv_key_to_wif(brain_key);

        auto&& priv_key = priv_key_from_wif(with_priv_key);

        auto&& pub_key = public_key_from_key(priv_key);

        const std::string& test_digest = "c14a0494ac87dccc09da5c7d25a551eab4e45777cedac2ea1978ff56947ed0d4";

        Digest digest = convert_digest(test_digest);

        CompactSignature sign = sign_digest(digest, priv_key);

        BOOST_REQUIRE(check_signature(sign, digest, pub_key));
    }

    BOOST_AUTO_TEST_SUITE_END()
} // namespace test
} // namespace playchain
