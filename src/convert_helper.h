#pragma once

#include "playchain_internal_types.h"

#include <cstdint>
#include <string>
#include <stdexcept>

namespace playchain {

bool is_app_little_endian();

inline bool is_playchain_little_endian()
{
    return true;
}

uint32_t endian_reverse_u32(uint32_t x);

std::string to_hex(const char* d, uint32_t s);
std::string to_hex(const uint8_t* d, uint32_t s);

template <typename T>
std::string to_hex(const T& data)
{
    if (!data.empty())
        return to_hex(data.data(), data.size());
    return {};
}

size_t from_hex(const std::string& hex_str, char* out_data, size_t out_data_len);
size_t from_hex(const std::string& hex_str, uint8_t* out_data, size_t out_data_len);

template <typename T>
void from_hex(const std::string& hex_str, T& data)
{
    PLAYCHAIN_ASSERT(!hex_str.empty() && data.size() == hex_str.size() / 2);

    size_t r = from_hex(hex_str, data.data(), data.size());
    PLAYCHAIN_ASSERT(r == hex_str.size() / 2);
}

time_t from_iso_string(const std::string& formatted);
std::string to_iso_string(const time_t);

uint64_t create_pseudo_random_from_time(const uint32_t offset = 1);

tp::PlaychainMoney calculate_data_fee(const size_t bytes, const tp::PlaychainMoney price_per_kbyte);

} // namespace playchain
