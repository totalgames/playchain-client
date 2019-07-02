#pragma once

#include "playchain_internal_types.h"

#include <array>

namespace playchain {

using namespace tp;

struct private_key_sec
{
    private_key_sec(const std::string& private_key_wif, const uint64_t noise);
    ~private_key_sec() = default;

    private_key decrypt() const;
    CompressedPublicKey get_public() const;
    std::string get_public_str() const;

    using crypted_key_type = std::array<uint8_t, 40>;

private:
    crypted_key_type _crypted;
};

} // namespace playchain
