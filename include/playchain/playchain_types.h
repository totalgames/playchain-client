#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>
#include <set>
#include <vector>

namespace tp {

#define DECLARE_PLAYCHAIN_ID(OBJECT, SPACE_ID, TYPE_ID, FROM_INSTANCE_ID) \
    struct Playchain##OBJECT##Id                                          \
    {                                                                     \
        Playchain##OBJECT##Id() = default;                                \
        Playchain##OBJECT##Id(int instance)                               \
            : instance(instance)                                          \
        {                                                                 \
        }                                                                 \
                                                                          \
        static constexpr int space_id = SPACE_ID;                         \
        static constexpr int type_id = TYPE_ID;                           \
        int instance = FROM_INSTANCE_ID;                                  \
                                                                          \
        operator int() const                                              \
        {                                                                 \
            return instance;                                              \
        }                                                                 \
                                                                          \
        bool valid() const                                                \
        {                                                                 \
            return instance > FROM_INSTANCE_ID;                           \
        }                                                                 \
                                                                          \
        operator std::string() const;                                     \
                                                                          \
        static Playchain##OBJECT##Id from_string(const std::string& id);  \
    }

DECLARE_PLAYCHAIN_ID(User, 1, 2, 0);
DECLARE_PLAYCHAIN_ID(Asset, 1, 3, -1);
DECLARE_PLAYCHAIN_ID(VestingBalance, 1, 13, -1);
DECLARE_PLAYCHAIN_ID(Room, 3, 2, 0);
DECLARE_PLAYCHAIN_ID(Table, 3, 4, 0);
DECLARE_PLAYCHAIN_ID(PendingBuyin, 3, 7, 0);
DECLARE_PLAYCHAIN_ID(Player, 3, 1, 0);
DECLARE_PLAYCHAIN_ID(Witness, 1, 6, 0);
DECLARE_PLAYCHAIN_ID(GameWitness, 3, 3, 0);

using PlaychainMoney = uint64_t;
using PlaychainPercent = uint16_t;

struct PlaychainPLC
{
    PlaychainPLC() = default;

    PlaychainPLC(PlaychainMoney amount)
        : _amount(amount)
    {
    }

    //1.25, 2, 0.015 PLC
    PlaychainPLC(const std::string& manual_plc_str);

    static const char* PLC();

    static PlaychainPLC from_string(const std::string& manual_plc_str);

    operator PlaychainMoney() const
    {
        return _amount;
    }
    PlaychainMoney amount() const
    {
        return _amount;
    }

    operator std::string() const;
    std::string str() const
    {
        return (std::string)(*this);
    }

private:
    PlaychainMoney _amount = 0;
};

struct ProtocolVersion
{
    ProtocolVersion() {}
    ProtocolVersion(const std::string& version);
    ProtocolVersion(uint8_t m, uint8_t h, uint16_t r)
    {
        v_num = (0 | m) << 8;
        v_num = (v_num | h) << 16;
        v_num = v_num | r;
    }

    bool operator==(const ProtocolVersion& o) const
    {
        return major() == o.major() && minor() == o.minor();
    }
    bool operator!=(const ProtocolVersion& o) const
    {
        return !(*this == o);
    }
    bool operator<(const ProtocolVersion& o) const
    {
        return v_num < o.v_num;
    }
    bool operator<=(const ProtocolVersion& o) const
    {
        return v_num <= o.v_num;
    }
    bool operator>(const ProtocolVersion& o) const
    {
        return v_num > o.v_num;
    }
    bool operator>=(const ProtocolVersion& o) const
    {
        return v_num >= o.v_num;
    }

    uint8_t major() const { return ((v_num >> 24) & 0x000000FF); }
    uint8_t minor() const { return ((v_num >> 16) & 0x000000FF); }
    uint16_t patch() const { return ((v_num & 0x0000FFFF)); }

    operator std::string() const;

    uint32_t v_num = 0;
    std::string metadata;
};

struct PlaychainRoomInfo
{
    PlaychainRoomInfo() = default;

    PlaychainRoomId id;
    PlaychainUserId owner;
    std::string owner_name;
    std::string server_url;
    std::string metadata;
    int32_t rating = 0;
    ProtocolVersion protocol_version;

    bool valid() const
    {
        return id.valid();
    }
};

struct PlaychainRoomInfoExt : public PlaychainRoomInfo
{
    PlaychainRoomInfoExt() = default;

    std::string last_rating_update_utc;
    PlaychainMoney rake_balance;
    PlaychainVestingBalanceId rake_balance_id;
};

struct PlaychainTableInfo
{
    PlaychainTableInfo() = default;

    enum class State : int
    {
        NOP = 0,
        NO_GAME,
        VOTING_FOR_GAME,
        GAME,
        VOTING_FOR_RESULTS,
    };

    PlaychainTableId id;
    PlaychainUserId owner;
    std::string owner_name;
    std::string metadata;
    std::string server_url;
    uint16_t required_witnesses = 0;
    PlaychainMoney min_accepted_proposal_asset = 0;
    State state = State::NOP;

    bool valid() const
    {
        return id.valid() && owner.valid();
    }
};

struct PlaychainPlayerTableInfo
{
    PlaychainPlayerTableInfo() = default;

    enum class State : int
    {
        NOP = 0,
        PENDING,
        ATTABLE,
        PLAYING,
    };

    State state = State::NOP;
    PlaychainMoney balance = 0;
    PlaychainMoney buyouting_balance = 0;
    PlaychainTableInfo table;

    bool valid() const
    {
        return table.valid() && state != State::NOP;
    }
};


struct PlaychainPendingBuyinInfo
{
    std::string name;
    PlaychainPendingBuyinId id;
    std::string uid;
    PlaychainMoney amount = 0;
};

struct CashInfo
{
    CashInfo() = default;
    CashInfo(const std::string& name, const PlaychainMoney amount)
        : name(name)
        , amount(amount)
    {
    }

    std::string name;
    PlaychainMoney amount = 0;
};

struct PlaychainTableInfoExt : public PlaychainTableInfo
{
    PlaychainTableInfoExt() = default;

    using PendingBuyinsType = std::map<PlaychainUserId, PlaychainPendingBuyinInfo>;
    using CashDatasType = std::map<PlaychainUserId, CashInfo>;
    using AccountsType = std::map<PlaychainUserId, std::string>;

    PendingBuyinsType pending_proposals;
    CashDatasType cash;
    CashDatasType playing_cash;
    AccountsType missed_voters;

    bool free() const;
};

struct PlayerInvitationInfo
{
    PlaychainUserId inviter;
    std::string uid;
    uint32_t lifetime_in_sec = 0;
    uint32_t lifetime_in_sec_left = 0;
    std::string metadata;
};

struct InvitedPlayerInfo
{
    PlaychainUserId id;
    std::string name;
};

struct WithdrawableBalanceInfo
{
    PlaychainMoney referral_balance = 0;
    PlaychainMoney rake_balance = 0;
    PlaychainMoney witness_balance = 0;

    PlaychainVestingBalanceId referral_balance_id;
    PlaychainVestingBalanceId rake_balance_id;
    PlaychainVestingBalanceId witness_balance_id;
};

struct PlaychainUserBalanceInfo : public WithdrawableBalanceInfo
{
    PlaychainMoney account_balance = 0;
};

struct GameInitialData
{
    std::map<PlaychainUserId, PlaychainMoney> cash;

    std::string info;
};

struct GameResult
{
    struct CashResult
    {
        CashResult() = default;
        CashResult(PlaychainMoney cash, PlaychainMoney rake)
            : cash(cash)
            , rake(rake)
        {
        }

        PlaychainMoney cash;
        PlaychainMoney rake;
    };

    std::map<PlaychainUserId, CashResult> cash;

    std::string log;
};

using BlockIdType = std::array<char, 20>;

using CompressedPublicKey = std::array<uint8_t, 33>;
using PrivateKey = std::array<uint8_t, 32>;

using CompactSignature = std::array<uint8_t, 65>;
using Digest = std::array<uint8_t, 32>;

struct PlaychainBlockHeaderInfo
{
    BlockIdType previous;
    time_t timestamp_utc;

    uint32_t block_num() const;

    static uint32_t block_num(const BlockIdType&);
};

struct BlockchainWitness
{
    PlaychainWitnessId id;
    PlaychainUserId account;
    uint32_t last_aslot = 0;
    CompressedPublicKey signing_key;
    PlaychainVestingBalanceId witness_balance_id;
    PlaychainMoney total_votes = 0;
    std::string url;
    uint32_t total_missed = 0;
    uint32_t last_confirmed_block_num = 0;
};

struct BlockchainGameWitness
{
    PlaychainGameWitnessId id;
    PlaychainUserId account;
    std::string name;
    std::vector<PlaychainRoomInfo> avaliable_rooms;
};

struct BlockchainAccount
{
    PlaychainUserId id;
    std::string name;
};

struct BlockchainRequest
{
    BlockchainRequest() = default;
    BlockchainRequest(const std::string& api, const std::string& method, const std::string& params = {})
        : _api(api)
        , _method(method)
        , _params(params)
    {
    }

    //NOTE: FOR COMPATIBILITY WITH OLD POKER_ROOM-WALLET INTERFACE ONLY
    //Old poker room interface transfer only transaction json ({transaction})
    //but this library operates  with extended format [api, method, [params]] ->
    //[api, method, [{transaction}]]
    static BlockchainRequest from_legacy_transaction_string(const std::string& js_request);
    static std::string to_legacy_transaction_string(const BlockchainRequest& request);

    //
    static BlockchainRequest from_string(const std::string& serialized_request);
    static std::string to_string(const BlockchainRequest& request);

    bool valid() const;

    operator std::string() const
    {
        return to_string(*this);
    }
    std::string str() const
    {
        return (std::string)(*this);
    }

    std::string api() const
    {
        return _api;
    }
    std::string method() const
    {
        return _method;
    }
    std::string params() const
    {
        return _params;
    }

private:
    std::string _api; //empty for default (0) look PlaychainAPI
    std::string _method;
    std::string _params;
};

struct BlockchainDigestTransaction
{
    BlockchainDigestTransaction() = default;
    BlockchainDigestTransaction(const BlockchainRequest& request, const std::string& digest)
        : _request(request)
        , _digest(digest)
    {
    }

    bool valid() const;

    BlockchainRequest request() const
    {
        return _request;
    }
    operator BlockchainRequest() const
    {
        return request();
    }

    std::string str() const
    {
        return request().str();
    }
    operator std::string() const
    {
        return str();
    }

    const std::string& digest() const
    {
        return _digest;
    }

private:
    BlockchainRequest _request;
    std::string _digest;
};

using BlockchainResponse = std::string;

} // namespace tp
