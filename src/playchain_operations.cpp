#include "playchain_operations.h"

#include "pack_helper.h"

namespace playchain {

PACK_TEMPLATE_DECLARE(authority)
PACK_TEMPLATE_DECLARE(game_initial_data)
PACK_TEMPLATE_DECLARE(gamer_cash_result)
PACK_TEMPLATE_DECLARE(game_result)
PACK_TEMPLATE_DECLARE(account_options)

void player_invitation_create_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, inviter);
    pack(s, uid);
    pack(s, lifetime_in_sec);
    pack(s, metadata);
}
size_t player_invitation_create_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, inviter);
    pack(s, uid);
    pack(s, lifetime_in_sec);
    pack(s, metadata);
    return s.tellp();
}
void player_invitation_create_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "inviter", inviter);
    pack_field(s, "uid", uid);
    pack_field(s, "lifetime_in_sec", lifetime_in_sec);
    pack_field(s, "metadata", metadata);
    s.EndObject();
    s.EndArray();
}

void player_invitation_resolve_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, inviter);
    pack(s, uid);
    pack(s, mandat);
    pack(s, name);
    pack(s, owner);
    pack(s, active);
}
void player_invitation_resolve_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "inviter", inviter);
    pack_field(s, "uid", uid);
    pack_field(s, "mandat", mandat);
    pack_field(s, "name", name);
    pack_field(s, "owner", owner);
    pack_field(s, "active", active);
    s.EndObject();
    s.EndArray();
}

void player_invitation_cancel_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, inviter);
    pack(s, uid);
}
size_t player_invitation_cancel_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, inviter);
    pack(s, uid);
    return s.tellp();
}
void player_invitation_cancel_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "inviter", inviter);
    pack_field(s, "uid", uid);
    s.EndObject();
    s.EndArray();
}

void buy_in_table_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, player);
    pack(s, table);
    pack(s, table_owner);
    pack(s, amount);
}
void buy_in_table_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "player", player);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "amount", amount);
    s.EndObject();
    s.EndArray();
}

void buy_out_table_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, player);
    pack(s, table);
    pack(s, table_owner);
    pack(s, amount);
    pack(s, reason);
}
size_t buy_out_table_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, player);
    pack(s, table);
    pack(s, table_owner);
    pack(s, amount);
    pack(s, reason);
    return s.tellp();
}
void buy_out_table_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "player", player);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "amount", amount);
    pack_field(s, "reason", reason);
    s.EndObject();
    s.EndArray();
}

template <typename Stream>
void pack_raw_object(const game_initial_data& obj, Stream& s)
{
    pack(s, obj.cash);
    pack(s, obj.info);
}
void game_initial_data::pack_object(raw_stream& s) const
{
    pack_raw_object(*this, s);
}
void game_initial_data::pack_object(datastream<size_t>& s) const
{
    pack_raw_object(*this, s);
}
void game_initial_data::pack_object(json_stream& s) const
{
    s.StartObject();
    pack_field(s, "cash", cash);
    pack_field(s, "info", info);
    s.EndObject();
}

void game_start_playing_check_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, voter);
    pack(s, initial_data);
}
size_t game_start_playing_check_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, voter);
    pack(s, initial_data);
    return s.tellp();
}
void game_start_playing_check_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "voter", voter);
    pack_field(s, "initial_data", initial_data);
    s.EndObject();
    s.EndArray();
}

template <typename Stream>
void pack_raw_object(const gamer_cash_result& obj, Stream& s)
{
    pack(s, obj.cash);
    pack(s, obj.rake);
}
void gamer_cash_result::pack_object(raw_stream& s) const
{
    pack_raw_object(*this, s);
}
void gamer_cash_result::pack_object(datastream<size_t>& s) const
{
    pack_raw_object(*this, s);
}
void gamer_cash_result::pack_object(json_stream& s) const
{
    s.StartObject();
    pack_field(s, "cash", cash);
    pack_field(s, "rake", rake);
    s.EndObject();
}

template <typename Stream>
void pack_raw_object(const game_result& obj, Stream& s)
{
    pack(s, obj.cash);
    pack(s, obj.log);
}
void game_result::pack_object(raw_stream& s) const
{
    pack_raw_object(*this, s);
}
void game_result::pack_object(datastream<size_t>& s) const
{
    pack_raw_object(*this, s);
}
void game_result::pack_object(json_stream& s) const
{
    s.StartObject();
    pack_field(s, "cash", cash);
    pack_field(s, "log", log);
    s.EndObject();
}

void game_result_check_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, voter);
    pack(s, result);
}
size_t game_result_check_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, voter);
    pack(s, result);
    return s.tellp();
}
void game_result_check_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "voter", voter);
    pack_field(s, "result", result);
    s.EndObject();
    s.EndArray();
}

void vesting_balance_withdraw_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, vesting_balance);
    pack(s, owner);
    pack(s, amount);
}
void vesting_balance_withdraw_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "vesting_balance", vesting_balance);
    pack_field(s, "owner", owner);
    pack_field(s, "amount", amount);
    s.EndObject();
    s.EndArray();
}

void game_reset_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, rollback_table);
}
void game_reset_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "rollback_table", rollback_table);
    s.EndObject();
    s.EndArray();
}

void buy_in_reserve_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, player);
    pack(s, uid);
    pack(s, amount);
    pack(s, metadata);
    pack(s, protocol_version);
}
size_t buy_in_reserve_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, player);
    pack(s, uid);
    pack(s, amount);
    pack(s, metadata);
    pack(s, protocol_version);
    return s.tellp();
}
void buy_in_reserve_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "player", player);
    pack_field(s, "uid", uid);
    pack_field(s, "amount", amount);
    pack_field(s, "metadata", metadata);
    pack_field(s, "protocol_version", protocol_version);
    s.EndObject();
    s.EndArray();
}

void buy_in_reserving_cancel_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, player);
    pack(s, uid);
}
size_t buy_in_reserving_cancel_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, player);
    pack(s, uid);
    return s.tellp();
}
void buy_in_reserving_cancel_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "player", player);
    pack_field(s, "uid", uid);
    s.EndObject();
    s.EndArray();
}

void buy_in_reserving_resolve_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, table);
    pack(s, table_owner);
    pack(s, pending_buyin);
}
void buy_in_reserving_resolve_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "table", table);
    pack_field(s, "table_owner", table_owner);
    pack_field(s, "pending_buyin", pending_buyin);
    s.EndObject();
    s.EndArray();
}

template <typename Stream>
void pack_raw_object(const account_options& obj, Stream& s)
{
    pack(s, obj.memo_key);
    pack(s, obj.voting_account);
    pack(s, obj.num_witness);
    pack(s, obj.num_committee);
    pack(s, obj.votes);
    pack(s, obj.extensions);
}
void account_options::pack_object(raw_stream& s) const
{
    pack_raw_object(*this, s);
}
void account_options::pack_object(datastream<size_t>& s) const
{
    pack_raw_object(*this, s);
}
void account_options::pack_object(json_stream& s) const
{
    s.StartObject();
    pack_field(s, "memo_key", memo_key);
    pack_field(s, "voting_account", voting_account);
    pack_field(s, "num_witness", num_witness);
    pack_field(s, "num_committee", num_committee);
    pack_field(s, "votes", votes);
    pack_field(s, "extensions", extensions);
    s.EndObject();
}

void account_create_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, registrar);
    pack(s, referrer);
    pack(s, referrer_percent);
    pack(s, name);
    pack(s, owner);
    pack(s, active);
    pack(s, options);
    pack(s, extensions);
}
size_t account_create_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, registrar);
    pack(s, referrer);
    pack(s, referrer_percent);
    pack(s, name);
    pack(s, owner);
    pack(s, active);
    pack(s, options);
    pack(s, extensions);
    return s.tellp();
}
void account_create_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "registrar", registrar);
    pack_field(s, "referrer", referrer);
    pack_field(s, "referrer_percent", referrer_percent);
    pack_field(s, "name", name);
    pack_field(s, "owner", owner);
    pack_field(s, "active", active);
    pack_field(s, "options", options);
    s.EndObject();
    s.EndArray();
}

void transfer_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, from);
    pack(s, to);
    pack(s, amount);
    pack(s, memo);
    pack(s, extensions);
}
size_t transfer_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, from);
    pack(s, to);
    pack(s, amount);
    pack(s, memo);
    pack(s, extensions);
    return s.tellp();
}
void transfer_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "from", from);
    pack_field(s, "to", to);
    pack_field(s, "amount", amount);
    s.EndObject();
    s.EndArray();
}

void player_create_by_room_owner_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, account);
    pack(s, room_owner);
}
void player_create_by_room_owner_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "account", account);
    pack_field(s, "room_owner", room_owner);
    s.EndObject();
    s.EndArray();
}

void room_create_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, owner);
    pack(s, server_url);
    pack(s, metadata);
    pack(s, protocol_version);
}
size_t room_create_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, owner);
    pack(s, server_url);
    pack(s, metadata);
    pack(s, protocol_version);
    return s.tellp();
}
void room_create_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "owner", owner);
    pack_field(s, "server_url", server_url);
    pack_field(s, "metadata", metadata);
    pack_field(s, "protocol_version", protocol_version);
    s.EndObject();
    s.EndArray();
}

void room_update_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, owner);
    pack(s, room);
    pack(s, server_url);
    pack(s, metadata);
    pack(s, protocol_version);
}
size_t room_update_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, owner);
    pack(s, room);
    pack(s, server_url);
    pack(s, metadata);
    pack(s, protocol_version);
    return s.tellp();
}
void room_update_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "owner", owner);
    pack_field(s, "room", room);
    pack_field(s, "server_url", server_url);
    pack_field(s, "metadata", metadata);
    pack_field(s, "protocol_version", protocol_version);
    s.EndObject();
    s.EndArray();
}

void table_create_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, owner);
    pack(s, room);
    pack(s, metadata);
    pack(s, required_witnesses);
    pack(s, min_accepted_proposal_asset);
}
size_t table_create_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, owner);
    pack(s, room);
    pack(s, metadata);
    pack(s, required_witnesses);
    pack(s, min_accepted_proposal_asset);
    return s.tellp();
}
void table_create_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "owner", owner);
    pack_field(s, "room", room);
    pack_field(s, "metadata", metadata);
    pack_field(s, "required_witnesses", required_witnesses);
    pack_field(s, "min_accepted_proposal_asset", min_accepted_proposal_asset);
    s.EndObject();
    s.EndArray();
}

void table_update_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, owner);
    pack(s, table);
    pack(s, metadata);
    pack(s, required_witnesses);
    pack(s, min_accepted_proposal_asset);
}
size_t table_update_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, owner);
    pack(s, table);
    pack(s, metadata);
    pack(s, required_witnesses);
    pack(s, min_accepted_proposal_asset);
    return s.tellp();
}
void table_update_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "owner", owner);
    pack_field(s, "table", table);
    pack_field(s, "metadata", metadata);
    pack_field(s, "required_witnesses", required_witnesses);
    pack_field(s, "min_accepted_proposal_asset", min_accepted_proposal_asset);
    s.EndObject();
    s.EndArray();
}

void buy_in_reserving_cancel_all_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, player);
}
size_t buy_in_reserving_cancel_all_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, player);
    return s.tellp();
}
void buy_in_reserving_cancel_all_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "player", player);
    s.EndObject();
    s.EndArray();
}

void witness_update_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, witness);
    pack(s, witness_account);
    pack(s, true);
    pack(s, new_url);
    pack(s, true);
    pack(s, new_signing_key);
}
size_t witness_update_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, witness);
    pack(s, witness_account);
    pack(s, true);
    pack(s, new_url);
    pack(s, true);
    pack(s, new_signing_key);
    return s.tellp();
}
void witness_update_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "witness", witness);
    pack_field(s, "witness_account", witness_account);
    pack_field(s, "new_url", new_url);
    pack_field(s, "new_signing_key", new_signing_key);
    s.EndObject();
    s.EndArray();
}

void table_alive_operation::pack_object(raw_stream& s) const
{
    pack(s, which);
    pack(s, fee);
    pack(s, owner);
    pack(s, tables);
}
size_t table_alive_operation::pack_size() const
{
    datastream<size_t> s;
    pack(s, fee);
    pack(s, owner);
    pack(s, tables);
    return s.tellp();
}
void table_alive_operation::pack_object(json_stream& s) const
{
    s.StartArray();
    pack(s, which);
    s.StartObject();
    pack_field(s, "fee", fee);
    pack_field(s, "owner", owner);
    pack_field(s, "tables", tables);
    s.EndObject();
    s.EndArray();
}
} // namespace playchain
