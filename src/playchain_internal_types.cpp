#include "playchain_internal_types.h"

#include <playchain/playchain_helper.h>

#include "pack_helper.h"

#include "convert_helper.h"

namespace playchain {

using namespace tp;

std::string public_key::serialize_to_string() const
{
    return public_key_to_string(_data);
}

template <typename Stream>
void pack_raw_object(const authority& obj, Stream& s)
{
    pack(s, obj.weight_threshold);
    pack(s, obj.account_auths);
    pack(s, obj.key_auths);
    pack(s, obj.obsolete);
}
void authority::pack_object(raw_stream& s) const
{
    pack_raw_object(*this, s);
}
void authority::pack_object(datastream<size_t>& s) const
{
    pack_raw_object(*this, s);
}
void authority::pack_object(json_stream& s) const
{
    s.StartObject();
    pack_field(s, "weight_threshold", weight_threshold);
    pack_field(s, "account_auths", account_auths);
    pack_field(s, "key_auths", key_auths);
    s.EndObject();
}
} // namespace playchain
