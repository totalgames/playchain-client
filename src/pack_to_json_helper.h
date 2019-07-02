#pragma once

#include "playchain_internal_types.h"
#include "convert_helper.h"

#include <cassert>
#include <vector>
#include <type_traits>
#include <map>
#include <array>
#include <set>

namespace playchain {

template <typename T>
inline void pack_int(json_stream& s, const T& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    assert(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value);
    if (sizeof(T) > sizeof(int32_t))
        s.Uint64((uint64_t)value);
    else
        s.Uint((uint32_t)value);
}

inline void pack(json_stream& s, const uint64_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const int64_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const uint32_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const int32_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const uint16_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const int16_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

inline void pack(json_stream& s, const uint8_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename T>
inline void pack(json_stream& s, const safe<T>& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, v.value, depth - 1);
}

inline void pack(json_stream& s, const std::string& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    s.String(v.c_str(), v.size());
}

inline void pack(json_stream& s, const char* v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    s.String(v);
}

inline void pack(json_stream& s, const account_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const witness_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const asset_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const room_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const table_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const vesting_balance_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

inline void pack(json_stream& s, const pending_buy_in_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, (std::string)v, depth - 1);
}

template <typename T, size_t N>
inline void pack(json_stream& s, const std::array<T, N>& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    std::string encoded = to_hex(v);
    pack(s, encoded, depth - 1);
}

inline void pack(json_stream& s, const public_key& pk, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, pk.serialize_to_string(), depth - 1);
}

inline void pack(json_stream& s, const unsigned_int& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, v.value, depth - 1);
}

inline void pack(json_stream& s, bool v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    s.Bool(v);
}

template <typename T>
inline void pack(json_stream& s, const std::vector<T>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    s.StartArray();
    auto itr = value.begin();
    auto end = value.end();
    while (itr != end)
    {
        pack(s, *itr, depth);
        ++itr;
    }
    s.EndArray();
}

template <typename T>
inline void pack(json_stream& s, const std::set<T>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    s.StartArray();
    auto itr = value.begin();
    auto end = value.end();
    while (itr != end)
    {
        pack(s, *itr, depth);
        ++itr;
    }
    s.EndArray();
}

template <typename K, typename V>
inline void pack(json_stream& s, const std::pair<K, V>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    s.StartArray();
    pack(s, value.first, depth);
    pack(s, value.second, depth);
    s.EndArray();
}

template <typename K, typename... V>
inline void pack(json_stream& s, const std::map<K, V...>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    s.StartArray();
    auto itr = value.begin();
    auto end = value.end();
    while (itr != end)
    {
        pack(s, *itr, depth);
        ++itr;
    }
    s.EndArray();
}

template <typename T>
inline void pack_field(json_stream& s, const char* name, T v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    pack(s, name, depth);
    pack(s, v, depth);
}

inline void pack(json_stream& s, const asset& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    s.StartObject();
    pack_field(s, "amount", v.amount, depth);
    pack_field(s, "asset_id", v.asset_id, depth);
    s.EndObject();
}
} // namespace playchain
