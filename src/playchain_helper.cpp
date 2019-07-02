#include <playchain/playchain_helper.h>
#include <playchain/playchain_settings.h>

#include "convert_helper.h"

#ifdef SECP256K1
#include <secp256k1.h>
#endif
#include "sha256.h"
#include "datastream.h"
#include "pack_helper.h"
#include "base58.h"
#include "ripemd160.h"
#include "sha512.h"

#include <rapidjson/document.h>

namespace tp {
using namespace playchain;

namespace {
    template <typename Stream>
    inline void unpack(Stream& ds, sha256& ep)
    {
        ds >> ep;
    }

    struct binary_key
    {
        uint32_t check = 0;
        std::array<char, 33> data;
    };

    template <typename Stream>
    inline void pack(Stream& s, const binary_key& v)
    {
        s.write((const char*)v.data.data(), v.data.size());
        s.write((const char*)&v.check, sizeof(v.check));
    }

    std::vector<char> pack(const binary_key& v)
    {
        datastream<size_t> ps;
        pack(ps, v);
        std::vector<char> vec(ps.tellp());

        if (vec.size())
        {
            datastream<char*> ds(vec.data(), size_t(vec.size()));
            pack(ds, v);
        }
        return vec;
    }

    template <typename Stream>
    inline void unpack(Stream& s, binary_key& v)
    {
        s.read((char*)v.data.data(), v.data.size());
        s.read((char*)&v.check, sizeof(v.check));
    }

    template <typename T>
    inline T unpack(const std::vector<char>& s)
    {
        T tmp;
        if (s.size())
        {
            datastream<const char*> ds(s.data(), size_t(s.size()));
            unpack(ds, tmp);
        }
        return tmp;
    }

    int extended_nonce_function(unsigned char* nonce32,
                                const unsigned char* msg32,
                                const unsigned char* key32,
                                const unsigned char* algo16,
                                void* data,
                                unsigned int attempt)
    {
        unsigned int* extra = static_cast<unsigned int*>(data);
        (*extra)++;
        return secp256k1_nonce_function_default(nonce32, msg32, key32, algo16, data, attempt);
    }

    bool is_canonical(const CompactSignature& c)
    {
        return !(c[1] & 0x80) && !(c[1] == 0 && !(c[2] & 0x80)) && !(c[33] & 0x80) && !(c[33] == 0 && !(c[34] & 0x80));
    }
} // namespace

PrivateKey priv_key_from_brain_key(const std::string& brain_key)
{
    PLAYCHAIN_ASSERT(!brain_key.empty(), "Invalid brain key");

    PrivateKey result;
    sha256 h = sha256::hash(brain_key.c_str(), brain_key.size());
    assert(h.data_size() == result.size());

    datastream<uint8_t*> ds(result.data(), size_t(result.size()));
    pack(ds, h);
    return result;
}

std::string priv_key_to_wif(const PrivateKey& key)
{
    sha256 secret;
    datastream<const uint8_t*> ds(key.data(), size_t(key.size()));
    unpack(ds, secret);

    const size_t size_of_data_to_hash = sizeof(secret) + 1;
    const size_t size_of_hash_bytes = 4;
    char data[size_of_data_to_hash + size_of_hash_bytes];
    data[0] = (char)0x80;
    memcpy(&data[1], (char*)&secret, sizeof(secret));
    sha256 digest = sha256::hash(data, size_of_data_to_hash);
    digest = sha256::hash(digest);
    memcpy(data + size_of_data_to_hash, (char*)&digest, size_of_hash_bytes);
    return to_base58(data, sizeof(data));
}

PrivateKey priv_key_from_wif(const std::string& wif_key)
{
    PrivateKey result;
    std::vector<char> wif_bytes;

    wif_bytes = from_base58(wif_key);

    PLAYCHAIN_ASSERT(wif_bytes.size() >= 5, "Invalid WIF format");

    std::vector<char> key_bytes(wif_bytes.begin() + 1, wif_bytes.end() - 4);
    sha256 bi;
    memset(&bi, char(0), sizeof(bi));
    if (key_bytes.size())
        memcpy(&bi, key_bytes.data(), std::min(key_bytes.size(), sizeof(bi)));

    assert(result.size() == bi.data_size());

    std::memcpy(result.data(), key_bytes.data(), result.size());

    sha256 check = sha256::hash(wif_bytes.data(), wif_bytes.size() - 4);
    sha256 check2 = sha256::hash(check);

    PLAYCHAIN_ASSERT(memcmp((char*)&check, wif_bytes.data() + wif_bytes.size() - 4, 4) == 0 || memcmp((char*)&check2, wif_bytes.data() + wif_bytes.size() - 4, 4) == 0);

    return result;
}

#ifdef SECP256K1
CompressedPublicKey public_key_from_key(const PrivateKey& key)
{
    secp256k1_context* _context = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN);

    PLAYCHAIN_ASSERT(_context);

    try
    {
        CompressedPublicKey result;

        secp256k1_pubkey pub_key_xy;

        PLAYCHAIN_ASSERT(1 == secp256k1_ec_pubkey_create(_context, &pub_key_xy, (unsigned char*)key.data()));

        size_t pk_len = result.size();

        PLAYCHAIN_ASSERT(1 == secp256k1_ec_pubkey_serialize(_context, (unsigned char*)result.data(), &pk_len, &pub_key_xy, SECP256K1_EC_COMPRESSED));

        PLAYCHAIN_ASSERT(pk_len == result.size());

        secp256k1_context_destroy(_context);

        return result;
    }
    catch (const std::logic_error&)
    {
        secp256k1_context_destroy(_context);

        throw;
    }

    return {};
}
#else //SECP256K1
CompressedPublicKey public_key_from_key(const PrivateKey& key)
{
    PLAYCHAIN_ERROR("Required SECP256K1 lib");
    return {};
}
#endif //!SECP256K1

CompressedPublicKey public_key_from_string(const std::string& formatted_key)
{
    public_key_data result;

    std::string prefix(PLAYCHAIN_ADDRESS_PREFIX);
    if (formatted_key.substr(0, prefix.size()) == prefix)
    {
        const size_t prefix_len = prefix.size();
        PLAYCHAIN_ASSERT(formatted_key.size() > prefix_len);
        PLAYCHAIN_ASSERT(formatted_key.substr(0, prefix_len) == prefix);
        auto bin = from_base58(formatted_key.substr(prefix_len));
        auto bin_key = unpack<binary_key>(bin);

        assert(result.size() == bin_key.data.size());

        std::memcpy(result.data(), (const uint8_t*)bin_key.data.data(), result.size());
        PLAYCHAIN_ASSERT(ripemd160::hash((const char*)result.data(), result.size())._hash[0] == bin_key.check);

        return result;
    }

    from_hex(formatted_key, result.data(), result.size());

    return result;
}

std::string public_key_to_string(const CompressedPublicKey& pub_key, bool wellformatted)
{
    binary_key k;
    std::memcpy(k.data.data(), (const char*)pub_key.data(), pub_key.size());
    if (wellformatted)
    {
        k.check = ripemd160::hash((const char*)k.data.data(), k.data.size())._hash[0];
        auto data = pack(k);
        return PLAYCHAIN_ADDRESS_PREFIX + to_base58(data.data(), data.size());
    }

    return to_hex(k.data.data(), k.data.size());
}

std::string address_from_public_key(const CompressedPublicKey& pub_key, bool wellformatted)
{
    ripemd160 addr;
    addr = ripemd160::hash(sha512::hash((const char*)pub_key.data(), pub_key.size()));
    std::array<char, 24> bin_addr;
    memcpy((char*)&bin_addr, (char*)&addr, sizeof(addr));
    auto checksum = ripemd160::hash((char*)&addr, sizeof(addr));
    memcpy(((char*)&bin_addr) + 20, (char*)&checksum._hash[0], 4);
    std::string result;
    if (wellformatted)
        result = PLAYCHAIN_ADDRESS_PREFIX;
    result.append(to_base58(bin_addr.data(), bin_addr.size()));
    return result;
}

Digest convert_digest(const std::string& hex_digest)
{
    sha256 sha_digest { hex_digest };

    Digest digest;
    auto* pdigest = digest.data();
    for (size_t ci = 0; ci < sizeof(sha_digest._hash) / sizeof(uint64_t); ++ci)
    {
        memcpy(pdigest, &sha_digest._hash[ci], sizeof(uint64_t));
        pdigest += sizeof(uint64_t);
    }

    return digest;
}

CompactSignature convert_signature(const std::string& hex_sign)
{
    CompactSignature sign;
#if !defined(NDEBUG)
    memset(sign.data(), 0, sign.size());
#endif
    from_hex(hex_sign, sign.data(), sign.size());

    return sign;
}

#ifdef SECP256K1
CompactSignature sign_digest(const Digest& digest, const PrivateKey& key, bool check_canonical)
{
    secp256k1_context* _context = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN);

    PLAYCHAIN_ASSERT(_context);

    try
    {
        CompactSignature sign;

        int recid = 0;

        secp256k1_ecdsa_signature _signature;

#if defined(SECP256K1_EXT)
        uint8_t counter[32];

        std::memset(&counter, 0, sizeof(counter));
#endif

        do
        {
#if defined(SECP256K1_EXT)
            PLAYCHAIN_ASSERT(1 == secp256k1_ecdsa_sign(_context, &_signature, (unsigned char*)digest.data(), key.data(), extended_nonce_function, &counter, &recid));
#else
            PLAYCHAIN_ASSERT(1 == secp256k1_ecdsa_sign(_context, &_signature, (unsigned char*)digest.data(), key.data(), extended_nonce_function, &recid));
#endif
            PLAYCHAIN_ASSERT(1 == secp256k1_ecdsa_signature_serialize_compact(_context, &sign[1], &_signature));
        } while (check_canonical && !is_canonical(sign));

        sign[0] = static_cast<uint8_t>(27 + 4 + recid);

        secp256k1_context_destroy(_context);

        return sign;
    }
    catch (const std::logic_error&)
    {
        secp256k1_context_destroy(_context);

        throw;
    }

    return {};
}
#else //SECP256K1
CompactSignature sign_digest(const Digest& digest, const PrivateKey& key, bool check_canonical)
{
    PLAYCHAIN_ERROR("Required SECP256K1 lib");
    return {};
}
#endif //!SECP256K1

#ifdef SECP256K1
bool check_signature(const CompactSignature& sign, const Digest& digest, const CompressedPublicKey& key, bool check_canonical)
{
    secp256k1_context* _context = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN);

    PLAYCHAIN_ASSERT(_context);

    try
    {
        if (check_canonical)
        {
            PLAYCHAIN_ASSERT(is_canonical(sign));
        }

        secp256k1_ecdsa_signature signature;

        PLAYCHAIN_ASSERT(sizeof(signature.data) / sizeof(unsigned char) == sign.size() - 1);

        PLAYCHAIN_ASSERT(1 == secp256k1_ecdsa_signature_parse_compact(_context, &signature, &sign[1]));

        secp256k1_pubkey public_key;

        PLAYCHAIN_ASSERT(1 == secp256k1_ec_pubkey_parse(_context, &public_key, (unsigned char*)key.data(), key.size()));

        int result = secp256k1_ecdsa_verify(_context, &signature, (unsigned char*)digest.data(), &public_key);

        secp256k1_context_destroy(_context);

        return 1 == result;
    }
    catch (const std::logic_error&)
    {
        secp256k1_context_destroy(_context);

        throw;
    }

    return false;
}
#else //SECP256K1
bool check_signature(const CompactSignature& sign, const Digest& digest, const CompressedPublicKey& key, bool check_canonical)
{
    PLAYCHAIN_ERROR("Required SECP256K1 lib");
    return false;
}
#endif //!SECP256K1

std::string to_hex(const char* d, uint32_t s)
{
    return playchain::to_hex(d, s);
}
std::string to_hex(const uint8_t* d, uint32_t s)
{
    return playchain::to_hex(d, s);
}
size_t from_hex(const std::string& hex_str, char* out_data, size_t out_data_len)
{
    return playchain::from_hex(hex_str, out_data, out_data_len);
}
size_t from_hex(const std::string& hex_str, uint8_t* out_data, size_t out_data_len)
{
    return playchain::from_hex(hex_str, out_data, out_data_len);
}

uint32_t create_pseudo_random_from_time(const uint32_t offset)
{
    auto r = playchain::create_pseudo_random_from_time(offset + 1) % std::numeric_limits<uint32_t>::max();
    return (uint32_t)r;
}

std::string create_pseudo_random_string_from_time(const uint32_t offset)
{
    auto r = playchain::create_pseudo_random_from_time(offset + 1);
    ripemd160::encoder enc;
    enc.write((const char*)&r, sizeof(r));
    return enc.result().str();
}

PlaychainPercent get_percent(const float& val)
{
    static PlaychainDefaultSettings default_settings;
    auto percent = default_settings.GRAPHENE_100_PERCENT / 100;

    return (uint16_t)(val * percent);
}

} // namespace tp
