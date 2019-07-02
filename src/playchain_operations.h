#pragma once

#include "playchain_internal_types.h"

#include <string>
#include <vector>
#include <map>

namespace playchain {
struct operation
{
    virtual ~operation() = default;

    virtual void pack_object(raw_stream& s) const = 0;
    virtual size_t pack_size() const { return 0; }
    virtual void pack_object(json_stream& s) const = 0;
};

struct player_invitation_create_operation : public operation
{
    const unsigned_int which = 56;

    asset fee;
    account_id_type inviter;
    std::string uid;
    uint32_t lifetime_in_sec;
    std::string metadata;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct player_invitation_resolve_operation : public operation
{
    const unsigned_int which = 57;

    asset fee;
    account_id_type inviter;
    std::string uid;
    signature_type mandat;
    std::string name;
    authority owner;
    authority active;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct player_invitation_cancel_operation : public operation
{
    const unsigned_int which = 58;

    asset fee;
    account_id_type inviter;
    std::string uid;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct buy_in_table_operation : public operation
{
    const unsigned_int which = 66;

    asset fee;
    account_id_type player;
    table_id_type table;
    account_id_type table_owner;
    asset amount;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct buy_out_table_operation : public operation
{
    const unsigned_int which = 67;

    asset fee;
    account_id_type player;
    table_id_type table;
    account_id_type table_owner;
    asset amount;

    std::string reason;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct game_initial_data
{
    std::map<account_id_type, asset> cash;

    std::string info;

    void pack_object(raw_stream& s) const;
    void pack_object(datastream<size_t>& s) const;
    void pack_object(json_stream& s) const;
};

struct game_start_playing_check_operation : public operation
{
    const unsigned_int which = 68;

    asset fee;
    table_id_type table;
    account_id_type table_owner;
    account_id_type voter;

    game_initial_data initial_data;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct gamer_cash_result
{
    asset cash;
    asset rake;

    void pack_object(raw_stream& s) const;
    void pack_object(datastream<size_t>& s) const;
    void pack_object(json_stream& s) const;
};

struct game_result
{
    std::map<account_id_type, gamer_cash_result> cash;
    std::string log;

    void pack_object(raw_stream& s) const;
    void pack_object(datastream<size_t>& s) const;
    void pack_object(json_stream& s) const;
};

struct game_result_check_operation : public operation
{
    const unsigned_int which = 69;

    asset fee;
    table_id_type table;
    account_id_type table_owner;
    account_id_type voter;

    game_result result;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct vesting_balance_withdraw_operation : public operation
{
    const unsigned_int which = 33;

    asset fee;
    vesting_balance_id_type vesting_balance;
    account_id_type owner;
    asset amount;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct game_reset_operation : public operation
{
    const unsigned_int which = 72;

    asset fee;
    table_id_type table;
    account_id_type table_owner;

    bool rollback_table;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct buy_in_reserve_operation : public operation
{
    const unsigned_int which = 73;

    asset fee;
    account_id_type player;
    std::string uid;
    asset amount;
    std::string metadata;
    std::string protocol_version;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct buy_in_reserving_cancel_operation : public operation
{
    const unsigned_int which = 74;

    asset fee;
    account_id_type player;
    std::string uid;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct buy_in_reserving_resolve_operation : public operation
{
    const unsigned_int which = 75;

    asset fee;
    table_id_type table;
    account_id_type table_owner;
    pending_buy_in_id_type pending_buyin;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct account_options
{
    public_key memo_key;
    account_id_type voting_account = account_id_type { 5 };
    uint16_t num_witness = 0;
    uint16_t num_committee = 0;
    std::set<unsigned_int> votes;
    std::set<bool> extensions = {};

    void pack_object(raw_stream& s) const;
    void pack_object(datastream<size_t>& s) const;
    void pack_object(json_stream& s) const;
};

struct account_create_operation : public operation
{
    const unsigned_int which = 5;

    asset fee;
    /// This account pays the fee. Must be a lifetime member.
    account_id_type registrar;

    /// This account receives a portion of the fee split between registrar and referrer. Must be a member.
    account_id_type referrer;
    /// Of the fee split between registrar and referrer, this percentage goes to the referrer. The rest goes to the
    /// registrar.
    uint16_t referrer_percent = 0;

    std::string name;
    authority owner;
    authority active;

    account_options options;
    unsigned_int extensions = 0u;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct transfer_operation : public operation
{
    const unsigned_int which = 0;

    asset fee;

    account_id_type from;
    account_id_type to;

    asset amount;

    bool memo = false;
    std::set<bool> extensions = {};

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct player_create_by_room_owner_operation : public operation
{
    const unsigned_int which = 71;

    asset fee;

    account_id_type account;
    account_id_type room_owner;

    void pack_object(raw_stream& s) const;
    void pack_object(json_stream& s) const;
};

struct room_create_operation : public operation
{
    const unsigned_int which = 62;

    asset fee;

    account_id_type owner;
    std::string server_url;
    std::string metadata;
    std::string protocol_version;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct room_update_operation : public operation
{
    const unsigned_int which = 63;

    asset fee;

    account_id_type owner;
    room_id_type room;
    std::string server_url;
    std::string metadata;
    std::string protocol_version;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct table_create_operation : public operation
{
    const unsigned_int which = 64;

    asset fee;

    account_id_type owner;
    room_id_type room;
    std::string metadata;
    amount_type required_witnesses = 0u;
    asset min_accepted_proposal_asset;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct table_update_operation : public operation
{
    const unsigned_int which = 65;

    asset fee;

    account_id_type owner;
    table_id_type table;
    std::string metadata;
    amount_type required_witnesses = 0u;
    asset min_accepted_proposal_asset;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct buy_in_reserving_cancel_all_operation : public operation
{
    const unsigned_int which = 79;

    asset fee;

    account_id_type player;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct witness_update_operation: public operation
{
    const unsigned_int which = 21;

    asset             fee;
    /// The witness object to update.
    witness_id_type   witness;
    /// The account which owns the witness. This account pays the fee for this operation.
    account_id_type   witness_account;

    /// The new URL.
    std::string       new_url;
    /// The new block signing key.
    public_key        new_signing_key;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

struct table_alive_operation: public operation
{
    const unsigned_int which = 85;

    asset fee;

    account_id_type owner;
    std::set<table_id_type> tables;

    void pack_object(raw_stream& s) const;
    size_t pack_size() const;
    void pack_object(json_stream& s) const;
};

} // namespace playchain
