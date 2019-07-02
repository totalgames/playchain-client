#include "ripemd160.h"
#include "sha256.h"
#include "sha512.h"

#include "convert_helper.h"
#include "playchain_defines.h"
#include "hash_helper.h"

#include <string.h>
#include <cmath>

namespace playchain {

ripemd160::ripemd160() { std::memset(_hash, 0, sizeof(_hash)); }
ripemd160::ripemd160(const std::string& hex_str)
{
    from_hex(hex_str, (char*)_hash, sizeof(_hash));
}

std::string ripemd160::str() const
{
    return to_hex((char*)_hash, sizeof(_hash));
}
ripemd160::operator std::string() const { return str(); }

char* ripemd160::data() const { return (char*)&_hash[0]; }

ripemd160::encoder::~encoder() {}
ripemd160::encoder::encoder()
{
    reset();
}

ripemd160 ripemd160::hash(const sha512& h)
{
    return hash((const char*)&h, sizeof(h));
}
ripemd160 ripemd160::hash(const sha256& h)
{
    return hash((const char*)&h, sizeof(h));
}
ripemd160 ripemd160::hash(const char* d, uint32_t dlen)
{
    encoder e;
    e.write(d, dlen);
    return e.result();
}
ripemd160 ripemd160::hash(const std::string& s)
{
    return hash(s.c_str(), s.size());
}

void ripemd160::encoder::write(const char* d, uint32_t dlen)
{
    RIPEMD160_Update(&_context, d, dlen);
}
ripemd160 ripemd160::encoder::result()
{
    ripemd160 h;
    RIPEMD160_Final((uint8_t*)h.data(), &_context);
    return h;
}
void ripemd160::encoder::reset()
{
    RIPEMD160_Init(&_context);
}

ripemd160 operator<<(const ripemd160& h1, uint32_t i)
{
    ripemd160 result;
    shift_l(h1.data(), result.data(), result.data_size(), i);
    return result;
}
ripemd160 operator^(const ripemd160& h1, const ripemd160& h2)
{
    ripemd160 result;
    result._hash[0] = h1._hash[0] ^ h2._hash[0];
    result._hash[1] = h1._hash[1] ^ h2._hash[1];
    result._hash[2] = h1._hash[2] ^ h2._hash[2];
    result._hash[3] = h1._hash[3] ^ h2._hash[3];
    result._hash[4] = h1._hash[4] ^ h2._hash[4];
    return result;
}
bool operator>=(const ripemd160& h1, const ripemd160& h2)
{
    return memcmp(h1._hash, h2._hash, sizeof(h1._hash)) >= 0;
}
bool operator>(const ripemd160& h1, const ripemd160& h2)
{
    return memcmp(h1._hash, h2._hash, sizeof(h1._hash)) > 0;
}
bool operator<(const ripemd160& h1, const ripemd160& h2)
{
    return memcmp(h1._hash, h2._hash, sizeof(h1._hash)) < 0;
}
bool operator!=(const ripemd160& h1, const ripemd160& h2)
{
    return memcmp(h1._hash, h2._hash, sizeof(h1._hash)) != 0;
}
bool operator==(const ripemd160& h1, const ripemd160& h2)
{
    return memcmp(h1._hash, h2._hash, sizeof(h1._hash)) == 0;
}

} // namespace playchain
