#pragma once

#include <playchain/playchain_types.h>

#include <chrono>

namespace tp {

struct PlaychainAPI
{
    PlaychainAPI() = default;

    const char* const WALLET = ""; //0
    const char* const GRAPHENE_DATABASE = "database";
    const char* const GRAPHENE_NETWORK = "network_broadcast";
    const char* const PLAYCHAIN = "playchain";
};

struct PlaychainDefaultSettings
{
    PlaychainDefaultSettings() = default;

    const uint32_t GRAPHENE_100_PERCENT = 10000;
    const uint32_t PLC_PRECISION = 100000;

    const uint32_t PLAYCHAIN_MINIMAL_INVITATION_EXPIRATION_PERIOD_SEC = 60 * 5;
    const uint32_t PLAYCHAIN_MAXIMUM_INVITATION_EXPIRATION_PERIOD_SEC = 60 * 60 * 24 * 30;

    const uint32_t TRANSACTION_EXPIRATION_SEC = 600;
    const uint32_t TRANSACTION_EXPIRATION_OFFSET_SEC = 100;

    const uint32_t TABLE_ALIVE_EXPIRATION_SEC = 60 * 10;

    const uint32_t PLAYCHAIN_MAX_SIZE_FOR_TABLES_ALIVE_PER_OP = 100;
};

struct PlaychainSettings
{
    PlaychainSettings() = default;

    const PlaychainAPI& API() const
    {
        static PlaychainAPI r;
        return r;
    }
    const PlaychainDefaultSettings& DEFAULT() const
    {
        static PlaychainDefaultSettings r;
        return r;
    }

    uint32_t transaction_expiration_sec = DEFAULT().TRANSACTION_EXPIRATION_SEC;
    uint32_t pending_buyin_proposal_lifetime_limit_sec = 60;
    uint32_t block_interval_sec = 3;
    uint32_t voting_for_playing_expiration_sec = 9;
    uint32_t voting_for_results_expiration_sec = 9;
    uint32_t table_alive_expiration_sec = DEFAULT().TABLE_ALIVE_EXPIRATION_SEC;

    //transaction_expiration_offset_sec used for randomize transaction
    //  (ex. same buyout in one block)
    //REQUIRED:
    //transaction_expiration_offset_sec <= transaction_expiration_sec/3
    uint32_t transaction_expiration_offset_sec = DEFAULT().TRANSACTION_EXPIRATION_OFFSET_SEC;

    PlaychainAssetId asset_id = PlaychainAssetId { 0 };

    PlaychainMoney fee_create_player_invitation = PlaychainPLC { "0.25 PLC" };
    PlaychainMoney fee_create_player_invitation_price_per_kbyte = 0;
    PlaychainMoney fee_cancel_player_invitation = 0;
    PlaychainMoney fee_cancel_player_invitation_price_per_kbyte = 0;
    PlaychainMoney fee_buyin = 0;
    PlaychainMoney fee_buyout = 0;
    PlaychainMoney fee_buyout_price_per_kbyte = 0;
    PlaychainMoney fee_game_start_playing = 0;
    PlaychainMoney fee_game_start_playing_price_per_kbyte = 0;
    PlaychainMoney fee_game_result_playing = 0;
    PlaychainMoney fee_game_result_playing_price_per_kbyte = 0;
    PlaychainMoney fee_game_reset = 0;
    PlaychainMoney fee_withdraw_playchain_vesting_balance = 0;
    PlaychainMoney fee_buy_in_reserve = 0;
    PlaychainMoney fee_buy_in_reserve_price_per_kbyte = 0;
    PlaychainMoney fee_buy_in_reserving_cancel = 0;
    PlaychainMoney fee_buy_in_reserving_cancel_price_per_kbyte = 0;
    PlaychainMoney fee_buy_in_reserving_resolve = 0;
    PlaychainMoney fee_transfer = 0;
    PlaychainMoney fee_transfer_price_per_kbyte = 0;
    PlaychainMoney fee_create_account_with_public_key = 0;
    PlaychainMoney fee_create_account_with_public_key_price_per_kbyte = 0;
    PlaychainMoney fee_create_room = 0;
    PlaychainMoney fee_create_room_price_per_kbyte = 0;
    PlaychainMoney fee_update_room = 0;
    PlaychainMoney fee_update_room_price_per_kbyte = 0;
    PlaychainMoney fee_create_table = 0;
    PlaychainMoney fee_create_table_price_per_kbyte = 0;
    PlaychainMoney fee_update_table = 0;
    PlaychainMoney fee_update_table_price_per_kbyte = 0;
    PlaychainMoney fee_player_create_by_room_owner = 0;
    PlaychainMoney fee_buy_in_reserving_cancel_all = 0;
    PlaychainMoney fee_witness_update = 0;
    PlaychainMoney fee_alive_table = 0;

    //all request with 'legacy' prefix will intended for wallet API
    bool all_legacy_from_wallet_api = false;
    //randomize transaction to make uniq for same data
    bool make_same_transactions_uniq = true;
};

} // namespace tp
