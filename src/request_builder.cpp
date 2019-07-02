
#include <playchain/request_builder.h>
#include <playchain/playchain_helper.h>

#include "convert_helper.h"

#include "playchain_operations.h"
#include "pack_helper.h"

#include <rapidjson/document.h>

#include <cassert>
#include <openssl/sha.h>
#include <algorithm>
#include <mutex>
#include <limits>

namespace tp {
using namespace playchain;

struct PlaychainRequestBuilderContext
{
    PlaychainRequestBuilderContext(const std::string& chain_id)
        : chain_id(chain_id)
    {
    }
    PlaychainRequestBuilderContext(const PlaychainRequestBuilderContext& other)
        : chain_id(other.chain_id)
        , last_blockchain_time(other.last_blockchain_time)
        , last_ref_block_num(other.last_ref_block_num)
        , last_ref_block_prefix(other.last_ref_block_prefix)
    {
    }

    std::string chain_id;

    void update(const time_t last_blockchain_time, const uint16_t last_ref_block_num, const uint32_t last_ref_block_prefix)
    {
        //locked to prevent reading separate field (get_...) when updating required for all fields
        std::unique_lock<std::mutex> lck(update_lock);

        if (this->last_blockchain_time != last_blockchain_time)
        {
            next_id = 0;
        }
        this->last_blockchain_time = last_blockchain_time;
        this->last_ref_block_num = last_ref_block_num;
        this->last_ref_block_prefix = last_ref_block_prefix;
    }

    time_t get_last_blockchain_time() const
    {
        std::unique_lock<std::mutex> lck(update_lock);

        return last_blockchain_time;
    }
    uint16_t get_last_ref_block_num() const
    {
        std::unique_lock<std::mutex> lck(update_lock);

        return last_ref_block_num;
    }
    uint32_t get_last_ref_block_prefix() const
    {
        std::unique_lock<std::mutex> lck(update_lock);

        return last_ref_block_prefix;
    }

    uint32_t get_next_sequence_id() const
    {
        std::unique_lock<std::mutex> lck(update_lock);

        return ++next_id;
    }

private:
    time_t last_blockchain_time = 0;
    uint16_t last_ref_block_num = 0;
    uint32_t last_ref_block_prefix = 0;
    mutable uint32_t next_id = 0;

    mutable std::mutex update_lock;
};

using chain_id_type = playchain::sha256;
using digest_type = playchain::sha256;

namespace {

    using operations = std::vector<const operation*>;

    BlockchainDigestTransaction makeTransaction(
        const PlaychainSettings& settings,
        const PlaychainRequestBuilderContext& context,
        const operations& ops)
    {
        BlockchainDigestTransaction result;
        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> js_writer(buff);

        js_writer.StartArray();
        js_writer.StartObject();

        time_t expiration = context.get_last_blockchain_time() + settings.transaction_expiration_sec;

        if (settings.make_same_transactions_uniq)
        {
            expiration -= context.get_next_sequence_id() % settings.transaction_expiration_offset_sec;
        }

        //expiration = [t + settings.transaction_expiration_sec - settings.transaction_expiration_offset_sec, t + settings.transaction_expiration_sec]

        digest_type::encoder coder;

        pack(coder, chain_id_type { context.chain_id });
        pack(coder, context.get_last_ref_block_num());
        pack(coder, context.get_last_ref_block_prefix());
        pack(coder, (uint32_t)expiration);

        pack_field(js_writer, "ref_block_num", context.get_last_ref_block_num());
        pack_field(js_writer, "ref_block_prefix", context.get_last_ref_block_prefix());
        pack_field(js_writer, "expiration", to_iso_string(expiration));

        pack(coder, unsigned_int((uint32_t)ops.size()));

        pack(js_writer, "operations");
        js_writer.StartArray();
        for (auto& op : ops)
        {
            op->pack_object(coder);
            op->pack_object(js_writer);
        }
        js_writer.EndArray();

        std::set<bool> extensions {};
        pack(coder, extensions);
        pack_field(js_writer, "extensions", extensions);

        auto digest = coder.result().str();

        js_writer.EndObject();
        js_writer.EndArray();

        auto request = BlockchainRequest { settings.API().GRAPHENE_NETWORK,
                                           "broadcast_transaction", buff.GetString() };

        return { request, digest };
    }

    BlockchainDigestTransaction makeTransaction(const PlaychainSettings& settings,
                                                const PlaychainRequestBuilderContext& context,
                                                const operation* pop)
    {
        return makeTransaction(settings, context, operations { pop });
    }
} // namespace

PlaychainRequestBuilder::PlaychainRequestBuilder(const std::string& chain_id, const PlaychainSettings& settings)
    : m_settings(settings)
    , m_context(new PlaychainRequestBuilderContext(chain_id))
{
    PLAYCHAIN_ASSERT(settings.transaction_expiration_offset_sec <= settings.transaction_expiration_sec / 3);
}

PlaychainRequestBuilder::PlaychainRequestBuilder(const PlaychainRequestBuilder& other)
{
    m_context.reset(new PlaychainRequestBuilderContext(*other.m_context));
    m_settings = other.m_settings;
}

PlaychainRequestBuilder::~PlaychainRequestBuilder()
{
}

std::string PlaychainRequestBuilder::getChainId() const
{
    return m_context->chain_id;
}

time_t PlaychainRequestBuilder::getLastBlockchainTime() const
{
    return m_context->get_last_blockchain_time();
}

BlockchainRequest PlaychainRequestBuilder::makeGetChainIdRequest()
{
    static PlaychainSettings default_settings;

    return { default_settings.API().GRAPHENE_DATABASE, "get_chain_id" };
}

BlockchainRequest PlaychainRequestBuilder::makeLoginRequest(const std::string& player) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_player_account_by_name", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetTablesInfoRequest(const std::set<PlaychainTableId>& ids) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, ids);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_tables_info_by_id", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetAccountIdByNameRequest(const std::set<std::string>& names) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, names);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_account_id_by_name", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetLastIrreversibleBlockHeaderRequest() const
{
    return { m_settings.API().PLAYCHAIN, "get_last_irreversible_block_header" };
}

BlockchainRequest PlaychainRequestBuilder::makeListPlayerInvitationsRequest(const PlaychainUserId& player,
                                                                            const std::string& last_page_uid,
                                                                            const uint32_t limit) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    pack(writer, last_page_uid);
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_player_invitations", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeListInvitedPlayersRequest(const PlaychainUserId& player,
                                                                         const std::string& last_page_uid,
                                                                         const uint32_t limit) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    pack(writer, last_page_uid);
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_invited_players", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetPlaychainBalanceRequest(const PlaychainUserId& account) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, account);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_playchain_balance_info", buff.GetString() };
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreatePendingBuyinTransaction(
    const std::string& protocol_version,
    const PlaychainUserId& player,
    const std::string& uid,
    const PlaychainMoney amount,
    const std::string& metadata) const
{
    buy_in_reserve_operation op;
    op.fee = asset(m_settings.fee_buy_in_reserve, m_settings.asset_id);
    op.player = player;
    op.uid = uid;
    op.amount = asset(amount, m_settings.asset_id);
    op.metadata = metadata;
    op.protocol_version = protocol_version;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_buy_in_reserve_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCancelPendingBuyinTransaction(
    const PlaychainUserId& player,
    const std::string& pending_buyin_uid) const
{
    buy_in_reserving_cancel_operation op;
    op.fee = asset(m_settings.fee_buy_in_reserving_cancel,
                   m_settings.asset_id);
    op.player = player;
    op.uid = pending_buyin_uid;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_buy_in_reserving_cancel_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeResolvePendingBuyinTransaction(
    const PlaychainUserId& table_owner,
    const PlaychainTableId& table,
    const PlaychainPendingBuyinId& pending_buyin) const
{
    buy_in_reserving_resolve_operation op;
    op.fee = asset(m_settings.fee_buy_in_reserving_resolve, m_settings.asset_id);
    op.table = table;
    op.table_owner = table_owner;
    op.pending_buyin = pending_buyin;

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCancelAllPendingBuyinsTransaction(
    const PlaychainUserId& player) const
{
    buy_in_reserving_cancel_all_operation op;
    op.fee = asset(m_settings.fee_buy_in_reserving_cancel_all, m_settings.asset_id);
    op.player = player;

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainRequest PlaychainRequestBuilder::makeCheckIfTableAllocatedForPendingBuyinRequest(
    const PlaychainUserId& player,
    const std::string& pending_buyin_uid) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    pack(writer, pending_buyin_uid);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_table_info_for_pending_buy_in_proposal", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeListTablesWithPlayerRequest(
    const PlaychainUserId& player,
    const uint32_t limit) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_tables_with_player", buff.GetString() };
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreatePlayerInvitationTransaction(
    const PlaychainUserId& inviter,
    const std::string& uid,
    const uint32_t& lifetime_in_sec,
    const std::string& metadata) const
{
    player_invitation_create_operation op;
    op.fee = asset(m_settings.fee_create_player_invitation, m_settings.asset_id);
    op.inviter = inviter;
    op.uid = uid;
    op.lifetime_in_sec = lifetime_in_sec;
    op.metadata = metadata;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_create_player_invitation_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

std::string PlaychainRequestBuilder::getPlayerInvitationDigest(const std::string& inviter,
                                                               const std::string& uid) const
{
    digest_type::encoder enc;
    pack(enc, chain_id_type { m_context->chain_id });
    pack(enc, inviter);
    pack(enc, uid);
    return enc.result().str();
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCancelPlayerInvitationTransaction(
    const PlaychainUserId& inviter,
    const std::string& uid) const
{
    player_invitation_cancel_operation op;
    op.fee = asset(m_settings.fee_cancel_player_invitation, m_settings.asset_id);
    op.inviter = inviter;
    op.uid = uid;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_cancel_player_invitation_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeResolvePlayerInvitationTransaction(
    const PlaychainUserId& inviter,
    const std::string& uid,
    const std::string& mandat,
    const std::string& new_pub_key,
    const std::string& new_account_name) const
{
    try
    {
        player_invitation_resolve_operation op;
        op.fee = asset(0, m_settings.asset_id); //zero by protocol
        op.inviter = inviter;
        op.uid = uid;
        playchain::from_hex(mandat, op.mandat);
        op.name = new_account_name;

        public_key_data owner_pubkey = public_key_from_string(new_pub_key);
        public_key_data active_pubkey = owner_pubkey;

        op.owner = authority(1, owner_pubkey, 1);
        op.active = authority(1, active_pubkey, 1);

        return makeTransaction(m_settings, *m_context, &op);
    }
    catch (std::exception&)
    {
    }
    return {};
}

BlockchainRequest PlaychainRequestBuilder::makeLegacyLoginRequest(
    const std::string& player, const std::string& formatted_key) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    pack(writer, public_key_from_string(formatted_key));
    writer.EndArray();

    return { m_settings.API().WALLET, "login_with_pubkey", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeLegacyGetAccountBalanceRequest(const std::string& player) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, player);
    writer.EndArray();

    return { m_settings.API().WALLET, "list_account_balances", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeLegacyCreateAccountWithPubkeyRequest(
    const std::string& registrator,
    const std::string& player,
    const std::string& formatted_key) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, public_key_from_string(formatted_key));
    pack(writer, player);
    pack(writer, registrator);
    pack(writer, registrator);
    pack(writer, true);
    pack(writer, false);
    writer.EndArray();

    return { m_settings.API().WALLET, "create_account_with_pubkey", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeLegacyGetAccountIdByNameRequest(const std::set<std::string>& names) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, names);
    writer.EndArray();

    return { m_settings.API().GRAPHENE_DATABASE, "lookup_account_names", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeLegacyGetLastBlockHeaderRequest() const
{
    return { m_settings.API().GRAPHENE_DATABASE, "get_dynamic_global_properties" };
}

BlockchainRequest PlaychainRequestBuilder::makeGetBlockchainPropertiesRequest() const
{
    return { m_settings.API().GRAPHENE_DATABASE, "get_global_properties" };
}

BlockchainRequest PlaychainRequestBuilder::makeGetPlaychainPropertiesRequest() const
{
    return { m_settings.API().PLAYCHAIN, "get_playchain_properties" };
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeBuyinTransaction(
    const PlaychainUserId& player,
    const PlaychainUserId& table_owner,
    const PlaychainTableId& table,
    const PlaychainMoney amount) const
{
    buy_in_table_operation op;
    op.fee = asset(m_settings.fee_buyin, m_settings.asset_id);
    op.player = player;
    op.table_owner = table_owner;
    op.table = table;
    op.amount = asset(amount, m_settings.asset_id);

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeBuyoutTransaction(
    const PlaychainUserId& player,
    const PlaychainUserId& table_owner,
    const PlaychainTableId& table,
    const PlaychainMoney amount,
    const std::string& reason) const
{
    buy_out_table_operation op;
    op.fee = asset(m_settings.fee_buyout, m_settings.asset_id);
    op.player = player;
    op.table_owner = table_owner;
    op.table = table;
    op.amount = asset(amount, m_settings.asset_id);
    op.reason = reason;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_buyout_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeVoteForStartGameTransaction(
    const PlaychainUserId& voter,
    const PlaychainUserId& table_owner,
    const PlaychainTableId& table,
    const GameInitialData& state) const
{
    game_start_playing_check_operation op;
    op.fee = asset(m_settings.fee_game_start_playing, m_settings.asset_id);
    op.voter = voter;
    op.table_owner = table_owner;
    op.table = table;

    std::transform(begin(state.cash), end(state.cash), std::inserter(op.initial_data.cash, end(op.initial_data.cash)),
                   [this](const decltype(state.cash)::value_type& data) {
                       return std::make_pair(data.first, asset(data.second, m_settings.asset_id));
                   });

    op.initial_data.info = state.info;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_game_start_playing_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeVoteForGameResultTransaction(
    const PlaychainUserId& voter,
    const PlaychainUserId& table_owner,
    const PlaychainTableId& table,
    const GameResult& state) const
{
    game_result_check_operation op;
    op.fee = asset(m_settings.fee_game_result_playing, m_settings.asset_id);
    op.voter = voter;
    op.table_owner = table_owner;
    op.table = table;

    std::transform(begin(state.cash), end(state.cash), std::inserter(op.result.cash, end(op.result.cash)),
                   [this](const decltype(state.cash)::value_type& data) {
                       gamer_cash_result r { asset(data.second.cash, m_settings.asset_id),
                                             asset(data.second.rake, m_settings.asset_id) };
                       return std::make_pair(data.first, r);
                   });

    op.result.log = state.log;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_game_result_playing_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeGameResetTransaction(const PlaychainUserId& table_owner,
                                                                              const PlaychainTableId& table,
                                                                              const bool rollback_table) const
{
    game_reset_operation op;
    op.fee = asset(m_settings.fee_game_reset, m_settings.asset_id);
    op.table_owner = table_owner;
    op.table = table;
    op.rollback_table = rollback_table;

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeWithdrawPlaychainVestingBalanceTransaction(
    const PlaychainUserId& account,
    const WithdrawableBalanceInfo& to_withdraw) const
{
    vesting_balance_withdraw_operation op_for_referral_balance;
    vesting_balance_withdraw_operation op_for_rake_balance;
    vesting_balance_withdraw_operation op_for_witness_balance;

    operations ops;
    ops.reserve(3);

    if (to_withdraw.referral_balance_id.valid())
    {
        op_for_referral_balance.fee = asset(m_settings.fee_withdraw_playchain_vesting_balance, m_settings.asset_id);
        op_for_referral_balance.vesting_balance = to_withdraw.referral_balance_id;
        op_for_referral_balance.owner = account;
        op_for_referral_balance.amount = asset(to_withdraw.referral_balance, m_settings.asset_id);

        ops.emplace_back(&op_for_referral_balance);
    }

    if (to_withdraw.rake_balance_id.valid())
    {
        op_for_rake_balance.fee = asset(m_settings.fee_withdraw_playchain_vesting_balance, m_settings.asset_id);
        op_for_rake_balance.vesting_balance = to_withdraw.rake_balance_id;
        op_for_rake_balance.owner = account;
        op_for_rake_balance.amount = asset(to_withdraw.rake_balance, m_settings.asset_id);

        ops.emplace_back(&op_for_rake_balance);
    }

    if (to_withdraw.witness_balance_id.valid())
    {
        op_for_witness_balance.fee = asset(m_settings.fee_withdraw_playchain_vesting_balance, m_settings.asset_id);
        op_for_witness_balance.vesting_balance = to_withdraw.witness_balance_id;
        op_for_witness_balance.owner = account;
        op_for_witness_balance.amount = asset(to_withdraw.witness_balance, m_settings.asset_id);

        ops.emplace_back(&op_for_witness_balance);
    }

    return makeTransaction(m_settings, *m_context, ops);
}

BlockchainRequest PlaychainRequestBuilder::makeBroadcastTransaction(
    const BlockchainRequest& trx,
    const std::set<std::string>& signatures) const
{
    try
    {
        rapidjson::Document document;
        document.Parse(trx.params().c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.IsArray());
        PLAYCHAIN_ASSERT_JSON(document.GetArray().Size() == 1u);
        PLAYCHAIN_ASSERT_JSON(document.GetArray()[0].IsObject());

        auto&& json_object = document.GetArray()[0].GetObject();

        std::set<std::string> ext_signatures;
        if (json_object.HasMember("signatures"))
        {
            PLAYCHAIN_ASSERT_JSON(json_object["signatures"].IsArray());

            for (const auto& sig : json_object["signatures"].GetArray())
            {
                PLAYCHAIN_ASSERT_JSON(sig.IsString());

                ext_signatures.emplace(sig.GetString());
            }

            json_object.RemoveMember("signatures");
        }

        for (const auto& sig : signatures)
        {
            ext_signatures.emplace(sig);
        }

        auto&& allocator = document.GetAllocator();
        rapidjson::Value json_signatures(rapidjson::kArrayType);

        for (const std::string& sig : ext_signatures)
        {
            json_signatures.PushBack(rapidjson::Value::StringRefType { sig.c_str() }, allocator);
        }

        json_object.AddMember("signatures", json_signatures, allocator);

        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

        document.Accept(writer);

        std::string api = m_settings.API().GRAPHENE_NETWORK;
        if (m_settings.all_legacy_from_wallet_api)
            api = m_settings.API().WALLET;

        return { api, "broadcast_transaction", buff.GetString() };
    }
    catch (std::exception& e)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

std::pair<BlockchainRequest, int> PlaychainRequestBuilder::makeSubscribeChangeTableInfoNotificationRequest(const std::set<PlaychainTableId>& ids, const int identifier) const
{
    int _identifier = identifier;
    if (-1 == _identifier)
    {
        static uint32_t i = 0;
        auto rnd = create_pseudo_random_from_time(++i);
        _identifier = rnd % std::numeric_limits<int>::max();
    }

    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, _identifier);
    pack(writer, ids);
    writer.EndArray();

    return std::make_pair(BlockchainRequest { m_settings.API().PLAYCHAIN, "set_tables_subscribe_callback", buff.GetString() }, _identifier);
}

BlockchainRequest PlaychainRequestBuilder::makeCancelSubscriptionForChangeTableInfoNotificationRequest(const std::set<PlaychainTableId>& ids) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, ids);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "cancel_tables_subscribe_callback", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeCancelSubscriptionForChangeTableInfoNotificationRequest() const
{
    return { m_settings.API().PLAYCHAIN, "cancel_all_tables_subscribe_callback" };
}

BlockchainRequest PlaychainRequestBuilder::makeGetPlayerIdByAccountIdRequest(const PlaychainUserId& account) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, account);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_player", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeListRoomsRequest(const PlaychainUserId& owner,
                                                                const uint32_t limit,
                                                                const PlaychainRoomId& from) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, owner);
    if (from.valid())
    {
        pack(writer, (std::string)from);
    }
    else
    {
        pack(writer, std::string {});
    }
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_rooms", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetRoomInfoRequest(const PlaychainUserId& owner, const std::string& metadata) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, owner);
    pack(writer, metadata);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_room_info", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetTablesInfoByMetadataRequest(const PlaychainRoomId& room,
                                                                              const std::string& metadata,
                                                                              const uint32_t limit) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, room);
    pack(writer, metadata);
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "get_tables_info_by_metadata", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeListTablesRequest(const PlaychainRoomId& room, const uint32_t limit, const PlaychainTableId& from) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, room);
    if (from.valid())
    {
        pack(writer, (std::string)from);
    }
    else
    {
        pack(writer, std::string {});
    }
    pack(writer, limit);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_tables", buff.GetString() };
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeTransferRequest(
    const PlaychainUserId& from,
    const PlaychainUserId& to,
    const PlaychainMoney amount) const
{
    transfer_operation op;
    op.fee = asset(m_settings.fee_transfer, m_settings.asset_id);
    op.from = from;
    op.to = to;
    op.amount = asset(amount, m_settings.asset_id);

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_transfer_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreateAccountWithPubkeyRequest(
    const PlaychainUserId& registrator,
    const std::string& player,
    const std::string& new_pub_key) const
{
    account_create_operation op;
    op.fee = asset(m_settings.fee_create_account_with_public_key, m_settings.asset_id);
    op.registrar = registrator;
    op.referrer = registrator;
    op.name = player;

    public_key_data owner_pubkey = public_key_from_string(new_pub_key);
    public_key_data active_pubkey = owner_pubkey;

    op.owner = authority(1, owner_pubkey, 1);
    op.active = authority(1, active_pubkey, 1);
    op.options.memo_key = active_pubkey;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_create_account_with_public_key_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreatePlayerByRoomOwnerRequest(
    const PlaychainUserId& room_owner,
    const PlaychainUserId& account) const
{
    player_create_by_room_owner_operation op;
    op.fee = asset(m_settings.fee_player_create_by_room_owner, m_settings.asset_id);
    op.account = account;
    op.room_owner = room_owner;

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreateRoomRequest(
    const std::string& protocol_version,
    const PlaychainUserId& room_owner,
    const std::string& server_url,
    const std::string& metadata) const
{
    room_create_operation op;
    op.fee = asset(m_settings.fee_create_room, m_settings.asset_id);
    op.owner = room_owner;
    op.server_url = server_url;
    op.metadata = metadata;
    op.protocol_version = protocol_version;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_create_room_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeCreateTableRequest(
    const PlaychainUserId& room_owner,
    const PlaychainRoomId& room,
    const std::string& metadata,
    const uint16_t required_witnesses,
    const PlaychainMoney min_accepted_proposal_asset) const
{
    table_create_operation op;
    op.fee = asset(m_settings.fee_create_table, m_settings.asset_id);
    op.owner = room_owner;
    op.room = room;
    op.metadata = metadata;
    op.required_witnesses = required_witnesses;
    op.min_accepted_proposal_asset = asset(min_accepted_proposal_asset, m_settings.asset_id);

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_create_table_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeUpdateRoomRequest(
    const std::string& protocol_version,
    const PlaychainRoomId& room,
    const PlaychainUserId& room_owner,
    const std::string& server_url,
    const std::string& metadata) const
{
    room_update_operation op;
    op.fee = asset(m_settings.fee_update_room, m_settings.asset_id);
    op.room = room;
    op.owner = room_owner;
    op.server_url = server_url;
    op.metadata = metadata;
    op.protocol_version = protocol_version;

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_update_room_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeUpdateTableRequest(
    const PlaychainTableId& table,
    const PlaychainUserId& room_owner,
    const std::string& metadata,
    const uint16_t required_witnesses,
    const PlaychainMoney min_accepted_proposal_asset) const
{
    table_update_operation op;
    op.fee = asset(m_settings.fee_update_table, m_settings.asset_id);
    op.owner = room_owner;
    op.table = table;
    op.metadata = metadata;
    op.required_witnesses = required_witnesses;
    op.min_accepted_proposal_asset = asset(min_accepted_proposal_asset, m_settings.asset_id);

    op.fee += asset(calculate_data_fee(op.pack_size(), m_settings.fee_update_table_price_per_kbyte),
                    m_settings.asset_id);
    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeAliveTableRequest(
    const std::set<PlaychainTableId>& tables,
    const PlaychainUserId& room_owner) const
{
    table_alive_operation op;
    op.fee = asset(m_settings.fee_alive_table, m_settings.asset_id);
    op.owner = room_owner;
    op.tables = tables;

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainDigestTransaction PlaychainRequestBuilder::makeUpdateWitnessRequest(
    const PlaychainWitnessId& witness,
    const PlaychainUserId& witness_account,
    const std::string& new_url,
    const std::string& new_signing_key) const
{
    witness_update_operation op;
    op.fee = asset(m_settings.fee_witness_update, m_settings.asset_id);
    op.witness = witness;
    op.witness_account = witness_account;
    op.new_url = new_url;
    op.new_signing_key = public_key_from_string(new_signing_key);

    return makeTransaction(m_settings, *m_context, &op);
}

BlockchainRequest PlaychainRequestBuilder::makeGetBlockchainWitnessRequest(const PlaychainUserId& witness_account) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, witness_account);
    writer.EndArray();

    return { m_settings.API().GRAPHENE_DATABASE, "get_witness_by_account", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetBlockchainAccountsRequest(const std::vector<PlaychainUserId>& accounts) const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, accounts);
    writer.EndArray();

    return { m_settings.API().GRAPHENE_DATABASE, "get_accounts", buff.GetString() };
}

BlockchainRequest PlaychainRequestBuilder::makeGetBlockchainGameWitnessesRequest() const
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    pack(writer, "");
    pack(writer, 100);
    writer.EndArray();

    return { m_settings.API().PLAYCHAIN, "list_all_game_witnesses", buff.GetString() };
}

void PlaychainRequestBuilder::setChainInfo(const PlaychainBlockHeaderInfo& info)
{
    auto last_blockchain_time = info.timestamp_utc;

    auto last_ref_block_num = PlaychainBlockHeaderInfo::block_num(info.previous);

    //blockchain uses system byte order to check ref_block_prefix
    assert(is_app_little_endian() == is_playchain_little_endian());

    uint32_t* pdata = (uint32_t*)info.previous.data();
    pdata++;
    auto last_ref_block_prefix = (*pdata);

    m_context->update(last_blockchain_time, last_ref_block_num, last_ref_block_prefix);
}
} // namespace tp
