#pragma once

#include <playchain/playchain_types.h>
#include <playchain/playchain_settings.h>

#include <tuple>
#include <vector>
#include <map>

namespace tp {

template <typename T>
struct ParsedResponse
{
    ParsedResponse() = default;
    ParsedResponse(T&& data)
        : _result(true)
        , _data(data)
    {
    }

    bool valid() const
    {
        return _result;
    }

    operator const T&() const
    {
        return _data;
    }

private:
    bool _result = false;
    T _data;
};

class PlaychainResponseParser
{
public:
    PlaychainResponseParser(const PlaychainSettings& settings = PlaychainSettings {});
    ~PlaychainResponseParser() = default;

    const PlaychainSettings& settings() const
    {
        return m_settings;
    }

    void updateSettings(const PlaychainSettings& settings) const
    {
        m_settings = settings;
    }

    static ParsedResponse<std::string> parseGetChainIdResponse(const BlockchainResponse&);
    ParsedResponse<std::vector<PlaychainTableInfoExt>> parseGetTablesInfoResponse(const BlockchainResponse&) const;
    //return invalid table if table has not yet allocated
    ParsedResponse<PlaychainTableInfo> parseCheckIfTableAllocatedForPendingBuyinResponse(const BlockchainResponse& response) const;
    ParsedResponse<std::vector<PlaychainPlayerTableInfo>> parseListTablesWithPlayerRequest(const BlockchainResponse& response) const;

    ParsedResponse<std::map<std::string, PlaychainUserId>> parseGetAccountIdByNameResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainBlockHeaderInfo> parseGetLastIrreversibleBlockHeaderResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<PlayerInvitationInfo>> parseListPlayerInvitationsResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<InvitedPlayerInfo>> parseListInvitedPlayersResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainUserBalanceInfo> parseGetPlaychainBalanceResponse(const BlockchainResponse&) const;
    ParsedResponse<std::pair<PlaychainUserId, CompressedPublicKey>> parseLoginResponse(const BlockchainResponse&) const;
    ParsedResponse<bool> parseTransactionResponse(const BlockchainResponse&) const;

    ParsedResponse<bool> parseLegacyLoginResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainMoney> parseLegacyGetAccountBalanceResponse(const BlockchainResponse&) const;
    ParsedResponse<std::map<std::string, PlaychainUserId>> parseLegacyGetAccountIdByNameResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainBlockHeaderInfo> parseLegacyGetLastBlockHeaderResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainSettings> parsePlaychainSettingFromProperties(const BlockchainResponse& blockchain,
                                                                          const BlockchainResponse& playchain) const;

    ParsedResponse<bool> parseSubscriptionResponse(const BlockchainResponse& response) const
    {
        return parseTransactionResponse(response);
    }
    ParsedResponse<std::vector<PlaychainTableInfoExt>>
    parseChangeTableInfoNotification(const BlockchainResponse&, const int identifier) const;
    ParsedResponse<int> parseNotificationCookie(const BlockchainResponse&) const;

    ParsedResponse<PlaychainPlayerId> parseGetPlayerIdByAccountIdResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<PlaychainRoomInfo>> parseListRoomsResponse(const BlockchainResponse&) const;
    ParsedResponse<PlaychainRoomInfoExt> parseGetRoomInfoResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<PlaychainTableInfo>> parseGetTablesInfoByMetadataResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<PlaychainTableId>> parseListTablesResponse(const BlockchainResponse&) const;

    PlaychainMoney getFeeFromTransaction(const BlockchainDigestTransaction&) const;

    ParsedResponse<BlockchainWitness> parseGetBlockchainWitnessResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<BlockchainGameWitness>> parseGetBlockchainGameWitnessesResponse(const BlockchainResponse&) const;
    ParsedResponse<std::vector<BlockchainAccount>> parseGetBlockchainAccountsResponse(const BlockchainResponse&) const;

private:
    mutable PlaychainSettings m_settings;
};

} // namespace tp
