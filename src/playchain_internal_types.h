#pragma once

#include <playchain/playchain_types.h>

#include "playchain_defines.h"
#include "sha256.h"
#include "datastream.h"

#include <rapidjson/writer.h>

#include <cstdint>
#include <array>
#include <map>

namespace playchain {

struct unsigned_int
{
    unsigned_int(uint32_t v = 0)
        : value(v)
    {
    }

    template <typename T>
    unsigned_int(T v)
        : value(v)
    {
    }

    template <typename T>
    operator T() const { return static_cast<T>(value); }

    uint32_t value = 0;
};

inline bool operator<(const unsigned_int& a, const unsigned_int& b)
{
    return a.value < b.value;
}

template <typename T>
struct safe
{
    T value = 0;

    template <typename O>
    safe(O o)
        : value(o)
    {
    }
    safe() {}
    safe(const safe& o)
        : value(o.value)
    {
    }

    safe& operator+=(const safe& o)
    {
        value += o.value;
        return *this;
    }
};

using share_type = safe<int64_t>;

using asset_id_type = tp::PlaychainAssetId;

struct asset
{
    asset() = default;

    template <typename T>
    asset(T v, const asset_id_type& id)
        : amount(v)
        , asset_id(id)
    {
    }

    share_type amount;
    asset_id_type asset_id;

    asset& operator+=(const asset& o)
    {
        PLAYCHAIN_ASSERT(asset_id == o.asset_id);
        amount += o.amount;
        return *this;
    }
};

using signature_type = std::array<unsigned char, 65>;

using public_key_data = tp::CompressedPublicKey;

struct public_key
{
    public_key() = default;

    public_key(const public_key_data& data)
        : _data(data)
    {
    }
    public_key(public_key_data&& data)
        : _data(data)
    {
    }

    const public_key_data& serialize() const
    {
        return _data;
    }

    std::string serialize_to_string() const;

    inline friend bool operator<(const public_key& a, const public_key& b)
    {
        return a.serialize() < b.serialize();
    }

private:
    public_key_data _data;
};

using weight_type = uint16_t;

using account_id_type = tp::PlaychainUserId;

using witness_id_type = tp::PlaychainWitnessId;

using raw_stream = sha256::encoder;
using json_stream = rapidjson::Writer<rapidjson::StringBuffer>;

struct authority
{
    authority() {}
    template <class... Args>
    authority(uint32_t threshhold, Args... auths)
        : weight_threshold(threshhold)
    {
        add_authorities(auths...);
    }

    void add_authority(const public_key& k, weight_type w)
    {
        key_auths[k] = w;
    }

    template <typename AuthType>
    void add_authorities(AuthType k, weight_type w)
    {
        add_authority(k, w);
    }
    template <typename AuthType, class... Args>
    void add_authorities(AuthType k, weight_type w, Args... auths)
    {
        add_authority(k, w);
        add_authorities(auths...);
    }

    void pack_object(raw_stream& s) const;
    void pack_object(datastream<size_t>& s) const;
    void pack_object(json_stream& s) const;

    uint32_t weight_threshold = 0;
    std::map<account_id_type, weight_type> account_auths;
    std::map<public_key, weight_type> key_auths;
    /** needed for backward compatibility only */
    unsigned_int obsolete = 0;
};

using room_id_type = tp::PlaychainRoomId;
using table_id_type = tp::PlaychainTableId;
using vesting_balance_id_type = tp::PlaychainVestingBalanceId;
using pending_buy_in_id_type = tp::PlaychainPendingBuyinId;
using player_id_type = tp::PlaychainPlayerId;

using private_key = tp::PrivateKey;

const uint32_t PACK_MAX_DEPTH = 10;

using amount_type = uint16_t;

} // namespace playchain
