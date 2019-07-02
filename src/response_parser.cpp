#include <playchain/response_parser.h>
#include <playchain/playchain_helper.h>

#include "convert_helper.h"

#include "playchain_defines.h"
#include "playchain_operations.h"

#include <rapidjson/document.h>

#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <stdlib.h>

namespace tp {

using namespace playchain;

namespace {
    template <typename Id, typename TJsonObject>
    Id parse_id(TJsonObject&& json_id, const PlaychainSettings&)
    {
        PLAYCHAIN_ASSERT_JSON(json_id.IsString());

        Id ret = Id::from_string(json_id.GetString());
        PLAYCHAIN_ASSERT_JSON(ret.valid());

        return ret;
    }

    template <typename TJsonObject>
    PlaychainMoney parse_asset(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("amount"));
        PLAYCHAIN_ASSERT_JSON(js_object["amount"].IsInt64() || js_object["amount"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("asset_id"));

        PlaychainAssetId asset_id = parse_id<PlaychainAssetId>(js_object["asset_id"], settings);
        PLAYCHAIN_ASSERT_JSON(asset_id == settings.asset_id);

        if (js_object["amount"].IsInt64())
            return js_object["amount"].GetInt64();

#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
        return std::atoll(js_object["amount"].GetString());
#else //< !PLAYCHAIN_LIB_FOR_MOBILE
        char* pEnd;
        auto r = strtoull(js_object["amount"].GetString(), &pEnd, 10);
        PLAYCHAIN_ASSERT_JSON(r >= 0);
        return (uint64_t)r;
#endif //< PLAYCHAIN_LIB_FOR_MOBILE
    }

    template <typename TJsonObject>
    PlaychainMoney parse_votes(TJsonObject&& js_object, const PlaychainSettings&)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsInt64() || js_object.IsString());

        if (js_object.IsInt64())
            return js_object.IsInt64();

#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
        return std::atoll(js_object.GetString());
#else //< !PLAYCHAIN_LIB_FOR_MOBILE
        char* pEnd;
        auto r = strtoull(js_object.GetString(), &pEnd, 10);
        PLAYCHAIN_ASSERT_JSON(r >= 0);
        return (uint64_t)r;
#endif //< PLAYCHAIN_LIB_FOR_MOBILE
    }

    template <typename TJsonObject>
    PlaychainPendingBuyinInfo parse_pending_buyin_info(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("name"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("uid"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("amount"));

        PlaychainPendingBuyinInfo result;

        result.name = js_object["name"].GetString();
        result.id = parse_id<PlaychainPendingBuyinId>(js_object["id"], settings);
        result.uid = js_object["uid"].GetString();
        result.amount = parse_asset(js_object["amount"], settings);

        return result;
    }

    PlaychainTableInfo::State parse_table_state(const std::string& state)
    {
        using State = PlaychainTableInfo::State;

        State result = State::NOP;

        if (state == "free")
            result = State::NO_GAME;
        else if (state == "playing")
            result = State::GAME;
        else if (state == "voting_for_playing")
            result = State::VOTING_FOR_GAME;
        else if (state == "voting_for_results")
            result = State::VOTING_FOR_RESULTS;

        return result;
    }

    PlaychainPlayerTableInfo::State parse_player_table_state(const std::string& state)
    {
        using State = PlaychainPlayerTableInfo::State;

        State result = State::NOP;

        if (state == "pending")
            result = State::PENDING;
        else if (state == "attable")
            result = State::ATTABLE;
        else if (state == "playing")
            result = State::PLAYING;

        return result;
    }

    template <typename TableObject, typename TJsonObject>
    TableObject parse_table_impl(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("required_witnesses"));
        PLAYCHAIN_ASSERT_JSON(js_object["required_witnesses"].IsInt());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("min_accepted_proposal_asset"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("state"));
        PLAYCHAIN_ASSERT_JSON(js_object["state"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("owner"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("owner_name"));
        PLAYCHAIN_ASSERT_JSON(js_object["owner_name"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("server_url"));
        PLAYCHAIN_ASSERT_JSON(js_object["server_url"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("metadata"));
        PLAYCHAIN_ASSERT_JSON(js_object["metadata"].IsString());

        TableObject result;

        result.id = parse_id<PlaychainTableId>(js_object["id"], settings);
        result.owner = parse_id<PlaychainUserId>(js_object["owner"], settings);
        result.owner_name = js_object["owner_name"].GetString();
        result.required_witnesses = (uint16_t)js_object["required_witnesses"].GetInt();
        result.min_accepted_proposal_asset = parse_asset(js_object["min_accepted_proposal_asset"], settings);
        result.state = parse_table_state(js_object["state"].GetString());
        result.metadata = js_object["metadata"].GetString();
        PLAYCHAIN_ASSERT_JSON(!result.metadata.empty());
        result.server_url = js_object["server_url"].GetString();

        return result;
    }

    template <typename TJsonObject>
    PlaychainTableInfo parse_table(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        return parse_table_impl<PlaychainTableInfo>(js_object, settings);
    }

    template <typename TJsonObject>
    PlaychainPlayerTableInfo parse_player_table(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("state"));
        PLAYCHAIN_ASSERT_JSON(js_object["state"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("balance"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("buyouting_balance"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("table"));
        PLAYCHAIN_ASSERT_JSON(js_object["table"].IsObject());

        PlaychainPlayerTableInfo result;

        result.state = parse_player_table_state(js_object["state"].GetString());
        result.balance = parse_asset(js_object["balance"], settings);
        result.buyouting_balance = parse_asset(js_object["buyouting_balance"], settings);
        result.table = parse_table(js_object["table"], settings);

        return result;
    }

    template <typename TJsonObject>
    CashInfo parse_cash_info(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("name"));
        PLAYCHAIN_ASSERT_JSON(js_object["name"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("amount"));

        CashInfo result;

        result.name = js_object["name"].GetString();
        result.amount = parse_asset(js_object["amount"], settings);

        return result;
    }

    template <typename TJsonObject>
    PlaychainTableInfoExt parse_table_ext(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PlaychainTableInfoExt result = parse_table_impl<PlaychainTableInfoExt>(js_object, settings);

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("pending_proposals"));
        PLAYCHAIN_ASSERT_JSON(js_object["pending_proposals"].IsArray());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("cash"));
        PLAYCHAIN_ASSERT_JSON(js_object["cash"].IsArray());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("playing_cash"));
        PLAYCHAIN_ASSERT_JSON(js_object["playing_cash"].IsArray());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("missed_voters"));
        PLAYCHAIN_ASSERT_JSON(js_object["missed_voters"].IsArray());

        {
            for (const auto& item : js_object["pending_proposals"].GetArray())
            {
                PLAYCHAIN_ASSERT_JSON(item.IsArray());

                auto&& js_array = item.GetArray();

                PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);

                result.pending_proposals.emplace(std::make_pair(parse_id<PlaychainUserId>(js_array[0], settings), parse_pending_buyin_info(js_array[1], settings)));
            }
        }

        {
            for (const auto& item : js_object["cash"].GetArray())
            {
                PLAYCHAIN_ASSERT_JSON(item.IsArray());

                auto&& js_array = item.GetArray();

                PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);

                result.cash.emplace(std::make_pair(parse_id<PlaychainUserId>(js_array[0], settings), parse_cash_info(js_array[1], settings)));
            }
        }

        {
            for (const auto& item : js_object["playing_cash"].GetArray())
            {
                PLAYCHAIN_ASSERT_JSON(item.IsArray());

                auto&& js_array = item.GetArray();

                PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);

                result.playing_cash.emplace(std::make_pair(parse_id<PlaychainUserId>(js_array[0], settings), parse_cash_info(js_array[1], settings)));
            }
        }

        {
            for (const auto& item : js_object["missed_voters"].GetArray())
            {
                PLAYCHAIN_ASSERT_JSON(item.IsArray());

                auto&& js_array = item.GetArray();

                PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);

                PLAYCHAIN_ASSERT_JSON(js_array[1].IsString());

                result.missed_voters.emplace(std::make_pair(parse_id<PlaychainUserId>(js_array[0], settings), js_array[1].GetString()));
            }
        }

        return result;
    }

    template <typename RoomObject, typename TJsonObject>
    RoomObject parse_room_impl_base(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("owner"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("server_url"));
        PLAYCHAIN_ASSERT_JSON(js_object["server_url"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("metadata"));
        PLAYCHAIN_ASSERT_JSON(js_object["metadata"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("rating"));
        PLAYCHAIN_ASSERT_JSON(js_object["rating"].IsInt());

        RoomObject result;

        result.id = parse_id<PlaychainRoomId>(js_object["id"], settings);
        result.owner = parse_id<PlaychainUserId>(js_object["owner"], settings);
        result.metadata = js_object["metadata"].GetString();
        result.server_url = js_object["server_url"].GetString();
        result.rating = js_object["rating"].GetInt();

        return result;
    }

    template <typename RoomObject, typename TJsonObject>
    RoomObject parse_room_impl(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        RoomObject result = parse_room_impl_base<RoomObject>(js_object, settings);

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("owner_name"));
        PLAYCHAIN_ASSERT_JSON(js_object["owner_name"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("protocol_version"));
        PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"].IsString());

        result.owner_name = js_object["owner_name"].GetString();
        result.protocol_version = ProtocolVersion(js_object["protocol_version"].GetString());

        return result;
    }

    template <typename TJsonObject>
    PlaychainRoomInfo parse_room(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        return parse_room_impl<PlaychainRoomInfo>(js_object, settings);
    }

    template <typename TJsonObject>
    PlaychainRoomInfoExt parse_room_ext(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PlaychainRoomInfoExt result = parse_room_impl<PlaychainRoomInfoExt>(js_object, settings);

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("last_rating_update"));
        PLAYCHAIN_ASSERT_JSON(js_object["last_rating_update"].IsString());

        result.last_rating_update_utc = js_object["last_rating_update"].GetString();
        result.rake_balance = parse_asset(js_object["rake_balance"], settings);
        if (js_object.HasMember("rake_balance_id") && js_object["rake_balance_id"].IsString())
        {
            result.rake_balance_id = parse_id<PlaychainVestingBalanceId>(js_object["rake_balance_id"], settings);
        }

        return result;
    }

    template <typename TJsonObject, typename TFeeItem>
    void parse_fee(TJsonObject&& js_object, TFeeItem& item, const PlaychainSettings& settings, const uint32_t scale)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        if (item.first)
        {
            uint32_t fee = 0;
            if (js_object.HasMember("fee"))
            {
                PLAYCHAIN_ASSERT_JSON(js_object["fee"].IsInt());
                fee = js_object["fee"].GetInt();
            }
            else
            {
                if (js_object.HasMember("basic_fee"))
                {
                    PLAYCHAIN_ASSERT_JSON(js_object["basic_fee"].IsInt());
                    fee = std::max<uint32_t>(js_object["basic_fee"].GetInt(), fee);
                }
                if (js_object.HasMember("premium_fee"))
                {
                    PLAYCHAIN_ASSERT_JSON(js_object["premium_fee"].IsInt());
                    fee = std::max<uint32_t>(js_object["premium_fee"].GetInt(), fee);
                }
            }

            uint64_t scaled = fee;
            scaled *= scale;
            scaled /= settings.DEFAULT().GRAPHENE_100_PERCENT;

            (*item.first) = (uint32_t)scaled;
        }

        if (item.second && js_object.HasMember("price_per_kbyte"))
        {
            PLAYCHAIN_ASSERT_JSON(js_object["price_per_kbyte"].IsInt());

            uint64_t scaled = js_object["price_per_kbyte"].GetInt();
            scaled *= scale;
            scaled /= settings.DEFAULT().GRAPHENE_100_PERCENT;

            (*item.second) = (uint32_t)scaled;
        }
    }

    template <typename TJsonObject>
    void parse_blockchain_fees(TJsonObject&& js_object, PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("current_fees"));
        PLAYCHAIN_ASSERT_JSON(js_object["current_fees"].IsObject());
        PLAYCHAIN_ASSERT_JSON(js_object["current_fees"].HasMember("parameters"));
        PLAYCHAIN_ASSERT_JSON(js_object["current_fees"].HasMember("scale"));
        PLAYCHAIN_ASSERT_JSON(js_object["current_fees"]["parameters"].IsArray());
        PLAYCHAIN_ASSERT_JSON(js_object["current_fees"]["scale"].IsInt());

        uint32_t scale = js_object["current_fees"]["scale"].GetInt();
        auto&& js_fees = js_object["current_fees"]["parameters"].GetArray();

        std::map<uint32_t, std::pair<PlaychainMoney*, PlaychainMoney*>> fee_in_settings;

        fee_in_settings[player_invitation_create_operation {}.which] = std::make_pair(&settings.fee_create_player_invitation, &settings.fee_create_player_invitation_price_per_kbyte);
        fee_in_settings[player_invitation_cancel_operation {}.which] = std::make_pair(&settings.fee_cancel_player_invitation, &settings.fee_cancel_player_invitation_price_per_kbyte);
        fee_in_settings[buy_in_table_operation {}.which] = std::make_pair(&settings.fee_buyin, nullptr);
        fee_in_settings[buy_out_table_operation {}.which] = std::make_pair(&settings.fee_buyout, &settings.fee_buyout_price_per_kbyte);
        fee_in_settings[game_start_playing_check_operation {}.which] = std::make_pair(&settings.fee_game_start_playing, &settings.fee_game_start_playing_price_per_kbyte);
        fee_in_settings[game_result_check_operation {}.which] = std::make_pair(&settings.fee_game_result_playing, &settings.fee_game_result_playing_price_per_kbyte);
        fee_in_settings[vesting_balance_withdraw_operation {}.which] = std::make_pair(&settings.fee_withdraw_playchain_vesting_balance, nullptr);
        fee_in_settings[game_reset_operation {}.which] = std::make_pair(&settings.fee_game_reset, nullptr);
        fee_in_settings[buy_in_reserve_operation {}.which] = std::make_pair(&settings.fee_buy_in_reserve, &settings.fee_buy_in_reserve_price_per_kbyte);
        fee_in_settings[buy_in_reserving_cancel_operation {}.which] = std::make_pair(&settings.fee_buy_in_reserving_cancel, &settings.fee_buy_in_reserving_cancel_price_per_kbyte);
        fee_in_settings[buy_in_reserving_resolve_operation {}.which] = std::make_pair(&settings.fee_buy_in_reserving_resolve, nullptr);
        fee_in_settings[account_create_operation {}.which] = std::make_pair(&settings.fee_create_account_with_public_key, &settings.fee_create_account_with_public_key_price_per_kbyte);
        fee_in_settings[transfer_operation {}.which] = std::make_pair(&settings.fee_transfer, &settings.fee_transfer_price_per_kbyte);
        fee_in_settings[player_create_by_room_owner_operation {}.which] = std::make_pair(&settings.fee_player_create_by_room_owner, nullptr);
        fee_in_settings[room_create_operation {}.which] = std::make_pair(&settings.fee_create_room, &settings.fee_create_room_price_per_kbyte);
        fee_in_settings[room_update_operation {}.which] = std::make_pair(&settings.fee_update_room, &settings.fee_update_room_price_per_kbyte);
        fee_in_settings[table_create_operation {}.which] = std::make_pair(&settings.fee_create_table, &settings.fee_create_table_price_per_kbyte);
        fee_in_settings[table_update_operation {}.which] = std::make_pair(&settings.fee_update_table, &settings.fee_update_table_price_per_kbyte);
        fee_in_settings[buy_in_reserving_cancel_all_operation {}.which] = std::make_pair(&settings.fee_buy_in_reserving_cancel_all, nullptr);
        fee_in_settings[witness_update_operation {}.which] = std::make_pair(&settings.fee_witness_update, nullptr);
        fee_in_settings[table_alive_operation {}.which] = std::make_pair(&settings.fee_alive_table, nullptr);

        for (const auto& item : js_fees)
        {
            PLAYCHAIN_ASSERT_JSON(item.IsArray());

            auto&& js_array = item.GetArray();

            PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);

            PLAYCHAIN_ASSERT_JSON(js_array[0].IsInt());

            auto witch = (uint32_t)js_array[0].GetInt();
            if (!fee_in_settings.count(witch))
                continue;

            auto& fee_item = fee_in_settings.at(witch);

            parse_fee(js_array[1], fee_item, settings, scale);
        }
    }

    template <typename TJsonObject>
    void parse_blockchain_options(TJsonObject&& js_object, PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("maximum_time_until_expiration"));
        PLAYCHAIN_ASSERT_JSON(js_object["maximum_time_until_expiration"].IsInt());

        settings.transaction_expiration_sec = std::min<uint32_t>(js_object["maximum_time_until_expiration"].GetInt(), settings.DEFAULT().TRANSACTION_EXPIRATION_SEC);
        settings.transaction_expiration_offset_sec = std::min(settings.transaction_expiration_sec / 3u, settings.DEFAULT().TRANSACTION_EXPIRATION_OFFSET_SEC);

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("block_interval"));
        PLAYCHAIN_ASSERT_JSON(js_object["block_interval"].IsInt());

        settings.block_interval_sec = js_object["block_interval"].GetInt();
    }

    void parse_blockchain_settings(const BlockchainResponse& response, PlaychainSettings& settings)
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

        auto&& js_object = document["result"];

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("parameters"));

        parse_blockchain_fees(js_object["parameters"], settings);
        parse_blockchain_options(js_object["parameters"], settings);
    }

    void parse_playchain_settings(const BlockchainResponse& response, PlaychainSettings& settings)
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());
        PLAYCHAIN_ASSERT_JSON(document["result"].HasMember("parameters"));

        auto&& js_object = document["result"]["parameters"];

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("pending_buyin_proposal_lifetime_limit_in_seconds"));
        PLAYCHAIN_ASSERT_JSON(js_object["pending_buyin_proposal_lifetime_limit_in_seconds"].IsInt());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("voting_for_playing_expiration_seconds"));
        PLAYCHAIN_ASSERT_JSON(js_object["voting_for_playing_expiration_seconds"].IsInt());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("voting_for_results_expiration_seconds"));
        PLAYCHAIN_ASSERT_JSON(js_object["voting_for_results_expiration_seconds"].IsInt());

        settings.pending_buyin_proposal_lifetime_limit_sec = js_object["pending_buyin_proposal_lifetime_limit_in_seconds"].GetInt();
        settings.voting_for_playing_expiration_sec = js_object["voting_for_playing_expiration_seconds"].GetInt();
        settings.voting_for_results_expiration_sec = js_object["voting_for_results_expiration_seconds"].GetInt();
        if (js_object.HasMember("table_alive_expiration_seconds"))
        {
            PLAYCHAIN_ASSERT_JSON(js_object["table_alive_expiration_seconds"].IsInt());
            settings.table_alive_expiration_sec = js_object["table_alive_expiration_seconds"].GetInt();
        }
    }

    template <typename TJsonObject>
    BlockchainWitness parse_blockchain_witness(TJsonObject&& js_object, const PlaychainSettings& settings)
    {
        PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

        BlockchainWitness result;

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("witness_account"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("last_aslot") && js_object["last_aslot"].IsUint());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("signing_key"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("total_votes"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("url"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("total_missed") && js_object["total_missed"].IsUint());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("last_confirmed_block_num") && js_object["last_confirmed_block_num"].IsUint());

        result.id = parse_id<PlaychainWitnessId>(js_object["id"], settings);
        result.account = parse_id<PlaychainUserId>(js_object["witness_account"], settings);

        result.last_aslot = js_object["last_aslot"].GetUint();
        std::string pub_key_str { js_object["signing_key"].GetString() };
        result.signing_key = public_key_from_string(pub_key_str);

        if (js_object.HasMember("pay_vb"))
        {
            result.witness_balance_id = parse_id<PlaychainVestingBalanceId>(js_object["pay_vb"], settings);
        }

        result.total_votes = parse_votes(js_object["total_votes"], settings);
        result.url = js_object["url"].GetString();
        result.total_missed = js_object["total_missed"].GetUint();
        result.last_confirmed_block_num = js_object["last_confirmed_block_num"].GetUint();

        return result;
    }
} // namespace

PlaychainResponseParser::PlaychainResponseParser(const PlaychainSettings& settings)
    : m_settings(settings)
{
}

ParsedResponse<std::string> PlaychainResponseParser::parseGetChainIdResponse(const BlockchainResponse& response)
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());

        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        PLAYCHAIN_ASSERT_JSON(document["result"].IsString());

        std::string result = document["result"].GetString();
        PLAYCHAIN_ASSERT_JSON(!result.empty());

        return { std::move(result) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainTableInfoExt>> PlaychainResponseParser::parseGetTablesInfoResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());

        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<PlaychainTableInfoExt> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& info : infos)
        {
            PlaychainTableInfoExt table_object = parse_table_ext(info, m_settings);

            PLAYCHAIN_ASSERT_JSON(table_object.valid());

            data.emplace_back(table_object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainTableInfo> PlaychainResponseParser::parseCheckIfTableAllocatedForPendingBuyinResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());

        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        if (!document["result"].IsNull())
        {
            PlaychainTableInfo table_object = parse_table(document["result"], m_settings);

            PLAYCHAIN_ASSERT_JSON(table_object.valid());

            return { std::move(table_object) };
        }

        return { PlaychainTableInfo {} };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainPlayerTableInfo>> PlaychainResponseParser::parseListTablesWithPlayerRequest(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<PlaychainPlayerTableInfo> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& info : infos)
        {
            PlaychainPlayerTableInfo object = parse_player_table(info, m_settings);

            PLAYCHAIN_ASSERT_JSON(object.valid());

            data.emplace_back(object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }
    return {};
}

ParsedResponse<std::map<std::string, PlaychainUserId>> PlaychainResponseParser::parseGetAccountIdByNameResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::map<std::string, PlaychainUserId> result;

        auto&& js_object = document["result"].GetArray();

        for (const auto& item : js_object)
        {
            PLAYCHAIN_ASSERT_JSON(item.IsArray());

            auto&& item_p = item.GetArray();
            PLAYCHAIN_ASSERT_JSON(item_p.Size() == 2u);
            PLAYCHAIN_ASSERT_JSON(item_p[0].IsString());

            result.emplace(item_p[0].GetString(), parse_id<PlaychainUserId>(item_p[1], m_settings));
        }

        return { std::move(result) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainBlockHeaderInfo> PlaychainResponseParser::parseGetLastIrreversibleBlockHeaderResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

        auto&& js_object = document["result"];

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("previous"));
        PLAYCHAIN_ASSERT_JSON(js_object["previous"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("timestamp"));
        PLAYCHAIN_ASSERT_JSON(js_object["timestamp"].IsString());

        PlaychainBlockHeaderInfo info;

        std::string hex_id { js_object["previous"].GetString() };

        playchain::from_hex(hex_id, info.previous);

        info.timestamp_utc = from_iso_string(js_object["timestamp"].GetString());

        return { std::move(info) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlayerInvitationInfo>> PlaychainResponseParser::parseListPlayerInvitationsResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        auto&& infos_p = document["result"].GetArray();

        PLAYCHAIN_ASSERT_JSON(infos_p.Size() == 2);

        PLAYCHAIN_ASSERT(infos_p[0].IsArray());
        PLAYCHAIN_ASSERT(infos_p[1].IsString());

        auto&& infos = infos_p[0].GetArray();
        time_t now_time = from_iso_string(infos_p[1].GetString());

        std::vector<PlayerInvitationInfo> data;
        data.reserve(infos.Size());

        for (const auto& info : infos)
        {
            PLAYCHAIN_ASSERT_JSON(info.IsObject());

            auto&& js_object = info.GetObject();

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("inviter"));
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("uid"));
            PLAYCHAIN_ASSERT_JSON(js_object["uid"].IsString());
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("metadata"));
            PLAYCHAIN_ASSERT_JSON(js_object["metadata"].IsString());
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("created"));
            PLAYCHAIN_ASSERT_JSON(js_object["created"].IsString());
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("expiration"));
            PLAYCHAIN_ASSERT_JSON(js_object["expiration"].IsString());

            PlayerInvitationInfo invitation_object;

            invitation_object.inviter = parse_id<PlaychainUserId>(js_object["inviter"], m_settings);
            invitation_object.uid = js_object["uid"].GetString();
            invitation_object.metadata = js_object["metadata"].GetString();
            time_t created_time = from_iso_string(js_object["created"].GetString());
            time_t expiration_time = from_iso_string(js_object["expiration"].GetString());

            invitation_object.lifetime_in_sec = expiration_time - created_time;
            invitation_object.lifetime_in_sec_left = expiration_time - now_time;

            data.emplace_back(invitation_object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<InvitedPlayerInfo>> PlaychainResponseParser::parseListInvitedPlayersResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        auto&& infos = document["result"].GetArray();

        std::vector<InvitedPlayerInfo> data;
        data.reserve(infos.Size());

        for (const auto& info : infos)
        {
            PLAYCHAIN_ASSERT_JSON(info.IsArray());

            auto&& info_p = info.GetArray();

            PLAYCHAIN_ASSERT_JSON(info_p.Size() == 2);

            InvitedPlayerInfo info_object;

            PLAYCHAIN_ASSERT_JSON(info_p[0].IsObject());

            {
                auto&& js_object = info_p[0].GetObject();

                PLAYCHAIN_ASSERT_JSON(js_object.HasMember("account"));

                info_object.id = parse_id<PlaychainUserId>(js_object["account"], m_settings);
            }

            PLAYCHAIN_ASSERT_JSON(info_p[1].IsObject());

            {
                auto&& js_object = info_p[1].GetObject();

                PLAYCHAIN_ASSERT_JSON(js_object.HasMember("name"));
                PLAYCHAIN_ASSERT_JSON(js_object["name"].IsString());

                info_object.name = js_object["name"].GetString();
            }

            data.emplace_back(info_object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainUserBalanceInfo> PlaychainResponseParser::parseGetPlaychainBalanceResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

        auto&& js_object = document["result"];

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("account_balance"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("rake_balance"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("referral_balance"));
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("witness_balance"));

        PlaychainUserBalanceInfo info;

        info.account_balance = parse_asset(js_object["account_balance"], m_settings);

        info.referral_balance = parse_asset(js_object["referral_balance"], m_settings);
        if (js_object.HasMember("referral_balance_id") && js_object["referral_balance_id"].IsString())
        {
            info.referral_balance_id = parse_id<PlaychainVestingBalanceId>(js_object["referral_balance_id"], m_settings);
        }

        info.rake_balance = parse_asset(js_object["rake_balance"], m_settings);
        if (js_object.HasMember("rake_balance_id") && js_object["rake_balance_id"].IsString())
        {
            info.rake_balance_id = parse_id<PlaychainVestingBalanceId>(js_object["rake_balance_id"], m_settings);
        }

        info.witness_balance = parse_asset(js_object["witness_balance"], m_settings);
        if (js_object.HasMember("witness_balance_id") && js_object["witness_balance_id"].IsString())
        {
            info.witness_balance_id = parse_id<PlaychainVestingBalanceId>(js_object["witness_balance_id"], m_settings);
        }

        return { std::move(info) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::pair<PlaychainUserId, CompressedPublicKey>> PlaychainResponseParser::parseLoginResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        if (!document["result"].IsNull())
        {
            PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

            auto&& js_object = document["result"];

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("account"));
            PlaychainUserId result_id = parse_id<PlaychainUserId>(js_object["account"], m_settings);

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("login_key"));
            PLAYCHAIN_ASSERT_JSON(js_object["login_key"].IsString());
            std::string pub_key_str { js_object["login_key"].GetString() };
            CompressedPublicKey result_pk = public_key_from_string(pub_key_str);

            return { std::make_pair(result_id, result_pk) };
        }
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<bool> PlaychainResponseParser::parseTransactionResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());

        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        PLAYCHAIN_ASSERT_JSON(document["result"].IsNull());

        return { true };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<bool> PlaychainResponseParser::parseLegacyLoginResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsBool());

        return { document["result"].GetBool() };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainMoney> PlaychainResponseParser::parseLegacyGetAccountBalanceResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        auto&& js_balance_array = document["result"].GetArray();

        if (js_balance_array.Size() == rapidjson::SizeType { 0 })
            return { PlaychainMoney { 0 } };

        PLAYCHAIN_ASSERT_JSON(js_balance_array.Size() > 0u);

        return { parse_asset(js_balance_array[0], m_settings) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::map<std::string, PlaychainUserId>> PlaychainResponseParser::parseLegacyGetAccountIdByNameResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        auto&& js_accounts_array = document["result"].GetArray();

        std::map<std::string, PlaychainUserId> result;

        for (const auto& item : js_accounts_array)
        {
            PLAYCHAIN_ASSERT_JSON(item.IsObject());

            auto&& js_account_object = item.GetObject();

            PLAYCHAIN_ASSERT_JSON(js_account_object.HasMember("id"));
            PLAYCHAIN_ASSERT_JSON(js_account_object.HasMember("name"));
            PLAYCHAIN_ASSERT_JSON(js_account_object["name"].IsString());

            result.emplace(js_account_object["name"].GetString(), parse_id<PlaychainUserId>(js_account_object["id"], m_settings));
        }

        return { std::move(result) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainBlockHeaderInfo> PlaychainResponseParser::parseLegacyGetLastBlockHeaderResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

        auto&& js_object = document["result"];

        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("head_block_id"));
        PLAYCHAIN_ASSERT_JSON(js_object["head_block_id"].IsString());
        PLAYCHAIN_ASSERT_JSON(js_object.HasMember("time"));
        PLAYCHAIN_ASSERT_JSON(js_object["time"].IsString());

        PlaychainBlockHeaderInfo info;

        std::string hex_id { js_object["head_block_id"].GetString() };

        playchain::from_hex(hex_id, info.previous);

        info.timestamp_utc = from_iso_string(js_object["time"].GetString());

        return { std::move(info) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainSettings> PlaychainResponseParser::parsePlaychainSettingFromProperties(const BlockchainResponse& blockchain_response,
                                                                                               const BlockchainResponse& playchain_response) const
{
    try
    {
        PlaychainSettings settings;

        parse_blockchain_settings(blockchain_response, settings);
        parse_playchain_settings(playchain_response, settings);

        return { std::move(settings) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainTableInfoExt>>
PlaychainResponseParser::parseChangeTableInfoNotification(const BlockchainResponse& response, const int identifier) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("method"));
        PLAYCHAIN_ASSERT_JSON(document["method"].IsString());
        PLAYCHAIN_ASSERT_JSON(std::string(document["method"].GetString()) == "notice");
        PLAYCHAIN_ASSERT_JSON(document.HasMember("params"));
        PLAYCHAIN_ASSERT_JSON(document["params"].IsArray());

        auto&& js_array = document["params"].GetArray();

        PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);
        PLAYCHAIN_ASSERT_JSON(js_array[0].IsInt());
        PLAYCHAIN_ASSERT_JSON(js_array[0].GetInt() == identifier);
        PLAYCHAIN_ASSERT_JSON(js_array[1].IsArray());

        auto&& js_data = js_array[1].GetArray();

        PLAYCHAIN_ASSERT_JSON(js_data.Size() == 1u);
        PLAYCHAIN_ASSERT_JSON(js_data[0].IsArray());

        auto&& js_tables = js_data[0].GetArray();

        std::vector<PlaychainTableInfoExt> data;

        data.reserve(js_tables.Size());
        for (const auto& info : js_tables)
        {
            PlaychainTableInfoExt table_object = parse_table_ext(info, m_settings);

            PLAYCHAIN_ASSERT_JSON(table_object.valid());

            data.emplace_back(table_object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<int> PlaychainResponseParser::parseNotificationCookie(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        if (!document.HasMember("method"))
            return {};

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("method"));
        PLAYCHAIN_ASSERT_JSON(document["method"].IsString());
        PLAYCHAIN_ASSERT_JSON(std::string(document["method"].GetString()) == "notice");
        PLAYCHAIN_ASSERT_JSON(document.HasMember("params"));
        PLAYCHAIN_ASSERT_JSON(document["params"].IsArray());

        auto&& js_array = document["params"].GetArray();

        PLAYCHAIN_ASSERT_JSON(js_array.Size() == 2u);
        PLAYCHAIN_ASSERT_JSON(js_array[0].IsInt());

        return { std::move(js_array[0].GetInt()) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainPlayerId> PlaychainResponseParser::parseGetPlayerIdByAccountIdResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        if (!document["result"].IsNull())
        {
            PLAYCHAIN_ASSERT_JSON(document["result"].IsObject());

            auto&& js_object = document["result"];

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));

            PlaychainPlayerId id = parse_id<PlaychainPlayerId>(js_object["id"], m_settings);

            return { std::move(id) };
        }

        return { PlaychainPlayerId {} };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainRoomInfo>> PlaychainResponseParser::parseListRoomsResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<PlaychainRoomInfo> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (auto&& js_object : infos)
        {
            auto&& room = parse_room_impl_base<PlaychainRoomInfo>(js_object, m_settings);

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("protocol_version"));
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"].IsObject());

            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"].HasMember("metadata"));
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"]["metadata"].IsString());
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"].HasMember("base"));
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"]["base"].IsObject());
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"]["base"].HasMember("v_num"));
            PLAYCHAIN_ASSERT_JSON(js_object["protocol_version"]["base"]["v_num"].IsUint());

            room.protocol_version.v_num = js_object["protocol_version"]["base"]["v_num"].GetUint();
            room.protocol_version.metadata = js_object["protocol_version"]["metadata"].GetString();

            data.emplace_back(room);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<PlaychainRoomInfoExt> PlaychainResponseParser::parseGetRoomInfoResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        if (!document["result"].IsNull())
        {
            PlaychainRoomInfoExt result = parse_room_ext(document["result"], m_settings);

            return { std::move(result) };
        }

        return { PlaychainRoomInfoExt {} };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainTableInfo>> PlaychainResponseParser::parseGetTablesInfoByMetadataResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<PlaychainTableInfo> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& info : infos)
        {
            PlaychainTableInfo room_object = parse_table(info, m_settings);

            PLAYCHAIN_ASSERT_JSON(room_object.valid());

            data.emplace_back(room_object);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<PlaychainTableId>> PlaychainResponseParser::parseListTablesResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<PlaychainTableId> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& js_object : infos)
        {
            PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));

            data.emplace_back(parse_id<PlaychainTableId>(js_object["id"], m_settings));
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

PlaychainMoney PlaychainResponseParser::getFeeFromTransaction(const BlockchainDigestTransaction& trx) const
{
    if (!trx.valid())
        return 0u;

    const auto& json_params = trx.request().params();

    rapidjson::Document document;
    document.Parse(json_params.c_str());

    PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
    PLAYCHAIN_ASSERT_JSON(document.IsArray());
    PLAYCHAIN_ASSERT_JSON(document.GetArray().Size() == 1u);

    auto&& json_object = document.GetArray()[0];

    PLAYCHAIN_ASSERT_JSON(json_object.HasMember("operations"));
    PLAYCHAIN_ASSERT_JSON(json_object["operations"].IsArray());

    PlaychainMoney fee = 0u;

    for (const auto& json_item : json_object["operations"].GetArray())
    {
        PLAYCHAIN_ASSERT_JSON(json_item.IsArray());
        PLAYCHAIN_ASSERT_JSON(json_item.GetArray().Size() == 2u);
        auto&& js_operation_object = json_item.GetArray()[1];

        PLAYCHAIN_ASSERT_JSON(js_operation_object.IsObject());

        PLAYCHAIN_ASSERT_JSON(js_operation_object.HasMember("fee"));
        fee += parse_asset(js_operation_object["fee"], m_settings);
    }

    return fee;
}

ParsedResponse<BlockchainWitness> PlaychainResponseParser::parseGetBlockchainWitnessResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        if (!document.HasMember("result"))
            return {};
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));

        if (!document["result"].IsNull())
        {
            BlockchainWitness result = parse_blockchain_witness(document["result"], m_settings);

            return { std::move(result) };
        }

        return { BlockchainWitness {} };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<BlockchainGameWitness>> PlaychainResponseParser::parseGetBlockchainGameWitnessesResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<BlockchainGameWitness> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& js_object : infos)
        {
            PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("account"));

            BlockchainGameWitness result;

            result.id = parse_id<PlaychainGameWitnessId>(js_object["id"], m_settings);
            result.account = parse_id<PlaychainUserId>(js_object["account"], m_settings);

            data.emplace_back(result);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

ParsedResponse<std::vector<BlockchainAccount>> PlaychainResponseParser::parseGetBlockchainAccountsResponse(const BlockchainResponse& response) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(response.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("result"));
        PLAYCHAIN_ASSERT_JSON(document["result"].IsArray());

        std::vector<BlockchainAccount> data;

        auto&& infos = document["result"].GetArray();

        data.reserve(infos.Size());
        for (const auto& js_object : infos)
        {
            PLAYCHAIN_ASSERT_JSON(js_object.IsObject());

            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("id"));
            PLAYCHAIN_ASSERT_JSON(js_object.HasMember("name"));

            BlockchainAccount result;

            result.id = parse_id<PlaychainUserId>(js_object["id"], m_settings);
            result.name = js_object["name"].GetString();

            data.emplace_back(result);
        }

        return { std::move(data) };
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

} // namespace tp
