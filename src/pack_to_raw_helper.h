#pragma once

#include "playchain_internal_types.h"

#include <cassert>
#include <vector>
#include <type_traits>
#include <map>
#include <array>
#include <set>

namespace playchain {

template <typename Stream, typename T>
inline void pack_int(Stream& s, const T& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    assert(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value);
    s.write((const char*)&value, sizeof(value));
}

template <typename Stream>
inline void pack(Stream& s, const uint64_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const int64_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const uint32_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const int32_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const uint16_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const int16_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream>
inline void pack(Stream& s, const uint8_t& v, uint32_t depth = PACK_MAX_DEPTH)
{
    pack_int(s, v, depth);
}

template <typename Stream, typename T>
inline void pack(Stream& s, const safe<T>& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, v.value, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const unsigned_int& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    uint64_t val = v.value;
    do
    {
        uint8_t b = uint8_t(val) & 0x7f;
        val >>= 7;
        b |= ((val > 0) << 7);
        s.write((char*)&b, 1);
    } while (val);
}

template <typename Stream>
inline void pack(Stream& s, const std::string& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int((uint32_t)v.size()), depth - 1);
    if (v.size())
        s.write(v.c_str(), (uint32_t)v.size());
}

template <typename Stream, typename T, size_t N>
inline void pack(Stream& s, const std::array<T, N>& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    s.write((const char*)v.data(), N * sizeof(T));
}

template <typename Stream>
inline void pack(Stream& s, const public_key& pk, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, pk.serialize(), depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, bool v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, uint8_t(v), depth - 1);
}

template <typename Stream, typename T>
inline void pack(Stream& s, const std::vector<T>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    pack(s, unsigned_int { (uint32_t)value.size() }, depth);
    if (!std::is_fundamental<T>::value)
    {
        auto itr = value.begin();
        auto end = value.end();
        while (itr != end)
        {
            pack(s, *itr, depth);
            ++itr;
        }
    }
    else
    {
        s.write((const char*)value.data(), value.size());
    }
}

template <typename Stream, typename K, typename V>
inline void pack(Stream& s, const std::pair<K, V>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    pack(s, value.first, depth);
    pack(s, value.second, depth);
}

template <typename Stream, typename K, typename... V>
inline void pack(Stream& s, const std::map<K, V...>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    pack(s, unsigned_int((uint32_t)value.size()), depth);
    auto itr = value.begin();
    auto end = value.end();
    while (itr != end)
    {
        pack(s, *itr, depth);
        ++itr;
    }
}

template <typename Stream, typename T>
inline void pack(Stream& s, const std::set<T>& value, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    --depth;
    pack(s, unsigned_int((uint32_t)value.size()), depth);
    auto itr = value.begin();
    auto end = value.end();
    while (itr != end)
    {
        pack(s, *itr, depth);
        ++itr;
    }
}

template <typename Stream>
inline void pack(Stream& s, const account_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const witness_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const asset_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const room_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const table_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const vesting_balance_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const pending_buy_in_id_type& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, unsigned_int { v.instance }, depth - 1);
}

template <typename Stream>
inline void pack(Stream& s, const asset& v, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    pack(s, v.amount, depth - 1);
    pack(s, v.asset_id, depth - 1);
}

template <typename Stream>
inline void pack(Stream& ds, const sha256& ep, uint32_t depth = PACK_MAX_DEPTH)
{
    assert(depth > 0);
    ds << ep;
}
} // namespace playchain
