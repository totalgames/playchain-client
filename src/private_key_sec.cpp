#include <playchain/playchain_helper.h>

#include "private_key_sec.h"

#include "convert_helper.h"

#include "pack_helper.h"

#include <vector>
#include <sstream>
#include <cstring>

#ifdef DEBUG_KEYS
#include <iostream>
#endif

namespace playchain {
namespace {

    private_key_sec::crypted_key_type encode(const private_key& key, const uint64_t noise)
    {
        private_key_sec::crypted_key_type result;

        auto&& pcrypted = result.data();

        const size_t cipher_size = sizeof(noise);

        std::memcpy(pcrypted, (const uint8_t*)&noise, cipher_size);

        size_t data_offset = cipher_size;
        size_t cipher_offset = 0;
        for (size_t ci = data_offset; ci < result.size(); ++ci)
        {
            if (cipher_offset >= cipher_size)
                cipher_offset = 0;
            pcrypted[ci] = key[ci - data_offset] ^ pcrypted[cipher_offset++];
        }

#ifdef DEBUG_KEYS
        std::cout << to_hex(result.data(), result.size()) << std::endl;
#endif

        return result;
    }

    private_key decode(const private_key_sec::crypted_key_type& crypted)
    {
        private_key result;

        auto&& pcrypted = crypted.data();

        const size_t cipher_size = sizeof(uint64_t);

        size_t data_offset = cipher_size;
        size_t cipher_offset = 0;
        for (size_t ci = 0; ci < result.size(); ++ci)
        {
            if (cipher_offset >= cipher_size)
                cipher_offset = 0;
            result[ci] = pcrypted[ci + data_offset] ^ pcrypted[cipher_offset++];
        }

#ifdef DEBUG_KEYS
        std::cout << to_hex(result.data(), result.size()) << std::endl;
#endif

        return result;
    }
} // namespace

private_key_sec::private_key_sec(const std::string& private_key_wif, const uint64_t noise)
{
    private_key key = priv_key_from_wif(private_key_wif);

#ifdef DEBUG_KEYS
    std::cout << to_hex(key.data(), key.size()) << std::endl;
#endif

    _crypted = encode(key, noise);
}

private_key private_key_sec::decrypt() const
{
    return decode(_crypted);
}

CompressedPublicKey private_key_sec::get_public() const
{
    return public_key_from_key(decrypt());
}

std::string private_key_sec::get_public_str() const
{
    return public_key_to_string(public_key_from_key(decrypt()));
}

} // namespace playchain
