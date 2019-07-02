#include "convert_helper.h"
#include "playchain_defines.h"

#if defined(PLAYCHAIN_LIB_FOR_MOBILE)
#include <cstdlib>
#include <ctime>
#include <time.h> //POSIX strptime
#else //< PLAYCHAIN_LIB_FOR_MOBILE
#include <ctime>
#include <iomanip> // std::put_time, std::get_time
#include <sstream>
#endif //< !PLAYCHAIN_LIB_FOR_MOBILE
#include <chrono>

namespace playchain {

bool is_app_little_endian()
{
    int test = 1;
    return (*(char*)&test == 1);
}

uint32_t endian_reverse_u32(uint32_t x)
{
    return (((x >> 0x18) & 0xFF))
        | (((x >> 0x10) & 0xFF) << 0x08)
        | (((x >> 0x08) & 0xFF) << 0x10)
        | (((x)&0xFF) << 0x18);
}

uint8_t from_hex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    PLAYCHAIN_ERROR("Invalid hex character");
    return 0;
}

std::string to_hex(const uint8_t* d, uint32_t s)
{
    std::string r;
    const char* to_hex = "0123456789abcdef";
    for (uint32_t i = 0; i < s; ++i)
        (r += to_hex[(d[i] >> 4)]) += to_hex[(d[i] & 0x0f)];
    return r;
}

std::string to_hex(const char* d, uint32_t s)
{
    return to_hex((const uint8_t*)d, s);
}

size_t from_hex(const std::string& hex_str, uint8_t* out_data, size_t out_data_len)
{
    std::string::const_iterator i = hex_str.begin();
    uint8_t* out_pos = out_data;
    uint8_t* out_end = out_pos + out_data_len;
    while (i != hex_str.end() && out_end != out_pos)
    {
        *out_pos = from_hex(*i) << 4;
        ++i;
        if (i != hex_str.end())
        {
            *out_pos |= from_hex(*i);
            ++i;
        }
        ++out_pos;
    }
    return out_pos - out_data;
}

size_t from_hex(const std::string& hex_str, char* out_data, size_t out_data_len)
{
    return from_hex(hex_str, (uint8_t*)out_data, out_data_len);
}

static const char* PLAYCHAIN_TIME_FORMAT = "%Y-%m-%dT%H:%M:%S";

#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
#if !defined(PLAYCHAIN_LIB_FOR_WINDOWS)
time_t from_iso_string(const std::string& formatted)
{
    std::stringstream ss;

    ss << formatted;

    std::tm tp {};

    ss >> std::get_time(&tp, PLAYCHAIN_TIME_FORMAT);
    if (!ss.fail())
    {
        time_t r = std::mktime(&tp);
        return r + tp.tm_gmtoff;
    }
    return {};
}
#else //< !PLAYCHAIN_LIB_FOR_WINDOWS
time_t from_iso_string(const std::string& formatted)
{
    std::stringstream ss;

    ss << formatted;

    std::tm tp {};

    ss >> std::get_time(&tp, PLAYCHAIN_TIME_FORMAT);
    if (!ss.fail())
    {
        return _mkgmtime(&tp);
    }
    return {};
}
#endif //< PLAYCHAIN_LIB_FOR_WINDOWS

std::string to_iso_string(const time_t t)
{
    std::stringstream ss;

    ss << std::put_time(std::gmtime(&t), PLAYCHAIN_TIME_FORMAT);

    return ss.str();
}
#else //< !PLAYCHAIN_LIB_FOR_MOBILE
time_t from_iso_string(const std::string& formatted)
{
    std::tm tp {};

    auto call_r = strptime(formatted.c_str(), PLAYCHAIN_TIME_FORMAT, &tp);

    PLAYCHAIN_ASSERT(call_r != NULL, "Can't parse time");

    time_t r = std::mktime(&tp);
    return r + tp.tm_gmtoff;
}

std::string to_iso_string(const time_t t)
{
    char buff[100];

    auto call_r = std::strftime(buff, sizeof(buff), PLAYCHAIN_TIME_FORMAT, std::gmtime(&t));

    PLAYCHAIN_ASSERT(call_r > 0, "Can't format time");

    return { buff };
}
#endif //< PLAYCHAIN_LIB_FOR_MOBILE

uint64_t create_pseudo_random_from_time(const uint32_t offset)
{
    using clock_type = std::chrono::high_resolution_clock;
    auto now = clock_type::now().time_since_epoch().count();

    /// High performance random generator
    /// http://xorshift.di.unimi.it/
    uint64_t r = (uint64_t)now + uint64_t(offset) * 2685821657736338717ULL;
    r ^= (r >> 12);
    r ^= (r << 25);
    r ^= (r >> 27);
    r *= 2685821657736338717ULL;
    return r;
}

tp::PlaychainMoney calculate_data_fee(const size_t bytes, const tp::PlaychainMoney price_per_kbyte)
{
    if (!price_per_kbyte)
        return {};

    auto result = (bytes * price_per_kbyte) / 1024;
    PLAYCHAIN_ASSERT(result < tp::PlaychainPLC("1000 PLC"));
    return tp::PlaychainMoney { result };
}

} // namespace playchain
