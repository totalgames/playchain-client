#include <playchain/playchain_types.h>
#include <playchain/playchain_settings.h>

#include "convert_helper.h"
#include "playchain_defines.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <string>
#include <sstream>
#include <limits>
#include <cstdlib>

namespace tp {

using namespace playchain;

namespace {
    template <typename T>
    std::string id_to_str(const T& id)
    {
        std::stringstream ss;

        ss << id.space_id;
        ss << '.';
        ss << id.type_id;
        ss << '.';
        ss << id.instance;

        return ss.str();
    }

#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
    int to_int(const std::string& s)
    {
        try
        {
            auto r = std::stoull(s);
            decltype(r) l = std::numeric_limits<uint32_t>::max();
            PLAYCHAIN_ASSERT(r <= l, "32-bit integer is required");
            return (int)r;
        }
        catch (const std::exception& e)
        {
            PLAYCHAIN_ERROR(e.what());
        }
    }
#else //< !PLAYCHAIN_LIB_FOR_MOBILE
    int to_int(const std::string& s)
    {
        try
        {
            auto r = std::atol(s.c_str());
            PLAYCHAIN_ASSERT(r >= 0, "Unsigned integer is required");
            decltype(r) l = std::numeric_limits<int>::max();
            PLAYCHAIN_ASSERT(r <= l, "sizeof(int) integer is required");
            return (int)r;
        }
        catch (const std::exception& e)
        {
            PLAYCHAIN_ERROR(e.what());
        }
    }
#endif //< PLAYCHAIN_LIB_FOR_MOBILE

    template <typename T>
    bool id_from_str(const std::string& s, T& id)
    {
        try
        {
            auto first_dot = s.find('.');
            auto second_dot = s.find('.', first_dot + 1);
            if (first_dot == second_dot)
                return false;
            if (first_dot == 0 || first_dot == std::string::npos)
                return false;

            if (to_int(s.substr(0, first_dot)) != id.space_id || to_int(s.substr(first_dot + 1, second_dot - first_dot - 1)) != id.type_id)
                return false;
            id.instance = to_int(s.substr(second_dot + 1));
        }
        catch (std::logic_error&)
        {
            return false;
        }
        return true;
    }
} // namespace

#define IMPL_PLAYCHAIN_ID(OBJECT)                                                   \
    Playchain##OBJECT##Id::operator std::string() const                             \
    {                                                                               \
        return id_to_str(*this);                                                    \
    }                                                                               \
                                                                                    \
    Playchain##OBJECT##Id Playchain##OBJECT##Id::from_string(const std::string& id) \
    {                                                                               \
        Playchain##OBJECT##Id result;                                               \
                                                                                    \
        if (!id_from_str(id, result))                                               \
            return {};                                                              \
                                                                                    \
        return result;                                                              \
    }

IMPL_PLAYCHAIN_ID(User)
IMPL_PLAYCHAIN_ID(Asset)
IMPL_PLAYCHAIN_ID(VestingBalance)
IMPL_PLAYCHAIN_ID(Room)
IMPL_PLAYCHAIN_ID(Table)
IMPL_PLAYCHAIN_ID(PendingBuyin)
IMPL_PLAYCHAIN_ID(Player)
IMPL_PLAYCHAIN_ID(Witness)
IMPL_PLAYCHAIN_ID(GameWitness)

const char* PlaychainPLC::PLC()
{
    return "PLC";
}

PlaychainPLC::PlaychainPLC(const std::string& manual_plc_str)
{
    _amount = from_string(manual_plc_str)._amount;
}

PlaychainPLC PlaychainPLC::from_string(const std::string& manual_plc_str)
{
    static PlaychainDefaultSettings default_settings;

    PlaychainPLC result;

    auto pos = manual_plc_str.rfind(PLC());
    if (std::string::npos == pos)
        return {};

    auto number = manual_plc_str.substr(0, pos);

    auto it_space = number.rbegin();
    while (it_space != number.rend())
    {
        char ch = *it_space;
        if (ch != ' ' && ch != '\t' && ch != '\n')
            break;
        ++it_space;
    }

    PlaychainMoney& amount = result._amount;

    int order = 1;
    bool cents = false;
    for (auto it_number = number.begin(); it_number != it_space.base(); ++it_number)
    {
        char ch[2] = { *it_number, '\0' };
        if (ch[0] == '.' || ch[0] == ',')
        {
            if (cents)
                break;
            cents = true;
            order = 1;
            continue;
        }
        if (ch[0] < '0' || ch[0] > '9')
            break;
        PlaychainMoney a = (PlaychainMoney)atoi(ch);
        a *= default_settings.PLC_PRECISION;
        if (cents)
        {
            for (auto ci = 0; ci < order; ++ci)
            {
                a /= 10;
            }
        }
        else if (order > 1)
            amount *= 10;
        amount += a;
        ++order;
    }

    return result;
}

PlaychainPLC::operator std::string() const
{
    static PlaychainDefaultSettings default_settings;

    std::stringstream ss;

    ss << _amount / default_settings.PLC_PRECISION;

    auto fractional_part = _amount % default_settings.PLC_PRECISION;
    if (fractional_part > 0)
    {
        ss << '.';

        auto tmp = fractional_part;
        tmp *= 10;
        while (tmp < default_settings.PLC_PRECISION)
        {
            ss << '0';
            tmp *= 10;
        }
        tmp = fractional_part;
        while (tmp >= 10 && tmp % 10 == 0)
        {
            tmp /= 10;
        }
        ss << tmp;
    }

    ss << ' ' << PLC();
    return ss.str();
}

//////////////////////////////////////////////////////////////////////////
ProtocolVersion::ProtocolVersion(const std::string& input_str)
{
    auto plus_pos = input_str.find('+');
    if (std::string::npos != plus_pos)
    {
        metadata = input_str.substr(plus_pos+1);
    }

    uint32_t major = 0, hardfork = 0, revision = 0;
    char dot_a = 0, dot_b = 0;

    std::stringstream s(input_str.substr(0, plus_pos));
    s >> major >> dot_a >> hardfork >> dot_b >> revision;

    // We'll accept either m.h.v or m_h_v as canonical version strings
    PLAYCHAIN_ASSERT((dot_a == '.' || dot_a == '_') && dot_a == dot_b, "Input version does not contain proper dotted decimal format");
    PLAYCHAIN_ASSERT(major <= 0xFF, "Major version is out of range");
    PLAYCHAIN_ASSERT(hardfork <= 0xFF, "Minor/Hardfork version is out of range");
    PLAYCHAIN_ASSERT(revision <= 0xFFFF, "Patch/Revision version is out of range");
    PLAYCHAIN_ASSERT(s.eof(), "Extra information at end of version string");

    v_num = 0 | (major << 24) | (hardfork << 16) | revision;
}

ProtocolVersion::operator std::string() const
{
    std::stringstream s;
    s << (int)major() << '.' << (int)minor() << '.' << patch();
    if (!metadata.empty())
    {
        s << '+';
        s << metadata;
    }

    return s.str();
}
//////////////////////////////////////////////////////////////////////////

bool PlaychainTableInfoExt::free() const
{
    return state == State::NO_GAME && pending_proposals.empty() && cash.empty() && playing_cash.empty();
}

uint32_t PlaychainBlockHeaderInfo::block_num(const BlockIdType& id)
{
    uint32_t result = 0;

    const uint32_t* pdata = (const uint32_t*)id.data();
    if (is_app_little_endian() == is_playchain_little_endian())
    {
        result = endian_reverse_u32(*pdata);
    }
    else
    {
        result = (*pdata);
    }

    return result;
}

uint32_t PlaychainBlockHeaderInfo::block_num() const
{
    return block_num(previous) + 1;
}

BlockchainRequest BlockchainRequest::from_legacy_transaction_string(const std::string& js_transaction)
{
    BlockchainRequest result;

    try
    {
        rapidjson::Document document;
        document.Parse(js_transaction.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.IsObject());

        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

        writer.StartArray();
        document.Accept(writer);
        writer.EndArray();

        return { PlaychainAPI {}.WALLET,
                 "broadcast_transaction", buff.GetString() };
    }
    catch (std::exception&)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}
std::string BlockchainRequest::to_legacy_transaction_string(const BlockchainRequest& request)
{
    if (!request.valid())
        return {};

    if (request.api() != PlaychainAPI {}.WALLET && request.api() != PlaychainAPI {}.GRAPHENE_NETWORK)
        return {};

    if (request.method() != "broadcast_transaction")
        return {};

    try
    {
        rapidjson::Document document;
        document.Parse(request.params().c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.IsArray());

        auto&& js_array = document.GetArray();

        PLAYCHAIN_ASSERT_JSON(js_array.Size() == 1u);
        PLAYCHAIN_ASSERT_JSON(js_array[0].IsObject());

        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

        js_array[0].Accept(writer);

        return { buff.GetString() };
    }
    catch (std::exception&)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

BlockchainRequest BlockchainRequest::from_string(const std::string& serialized_request)
{
    BlockchainRequest result;

    try
    {
        rapidjson::Document document;
        document.Parse(serialized_request.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());
        PLAYCHAIN_ASSERT_JSON(document.IsArray());

        auto&& js_array = document.GetArray();

        PLAYCHAIN_ASSERT_JSON(js_array.Size() == 3u);
        PLAYCHAIN_ASSERT_JSON(js_array[0].IsString());
        PLAYCHAIN_ASSERT_JSON(js_array[1].IsString());
        PLAYCHAIN_ASSERT_JSON(js_array[2].IsArray());

        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

        js_array[2].Accept(writer);

        return { js_array[0].GetString(),
                 js_array[1].GetString(), buff.GetString() };
    }
    catch (std::exception&)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

std::string BlockchainRequest::to_string(const BlockchainRequest& request)
{
    PLAYCHAIN_ASSERT(request.valid());

    auto&& _api = request.api();
    auto&& _method = request.method();
    auto&& _params = request.params();

    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    writer.StartArray();
    if (_api == PlaychainAPI {}.WALLET)
    {
        writer.Int(0);
    }
    else
    {
        PLAYCHAIN_ASSERT(!_api.empty(), "API required");
        writer.String(_api.c_str());
    }
    writer.String(_method.c_str());
    if (!_params.empty())
    {
        writer.RawValue(_params.c_str(), _params.length(), rapidjson::kArrayType);
    }
    else
    {
        writer.StartArray();
        writer.EndArray();
    }
    writer.EndArray();

    return buff.GetString();
}

bool BlockchainRequest::valid() const
{
    if (_method.empty())
        return false;

    if (!_params.empty())
    {
        rapidjson::Document document;
        document.Parse(_params.c_str());

        return !document.HasParseError() && document.IsArray();
    }

    return true;
}

bool BlockchainDigestTransaction::valid() const
{
    return !_digest.empty() && _request.valid();
}

} // namespace tp
