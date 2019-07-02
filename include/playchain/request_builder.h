#pragma once

#include <playchain/playchain_types.h>
#include <playchain/playchain_settings.h>

#include <string>
#include <set>
#include <vector>
#include <memory>

namespace tp {

struct PlaychainRequestBuilderContext;

class PlaychainRequestBuilder
{
public:
    PlaychainRequestBuilder(const std::string& chain_id, const PlaychainSettings& settings = PlaychainSettings {});
    PlaychainRequestBuilder(const PlaychainRequestBuilder&);
    ~PlaychainRequestBuilder();

    const PlaychainSettings& settings() const
    {
        return m_settings;
    }

    void updateSettings(const PlaychainSettings& settings) const
    {
        m_settings = settings;
    }

    std::string getChainId() const;
    time_t getLastBlockchainTime() const;

    static BlockchainRequest makeGetChainIdRequest();

    BlockchainRequest makeLoginRequest(const std::string& player) const;
    BlockchainRequest makeGetTablesInfoRequest(const std::set<PlaychainTableId>& ids) const;
    BlockchainRequest makeGetAccountIdByNameRequest(const std::set<std::string>& names) const;
    BlockchainRequest makeGetAccountIdByNameRequest(const std::string& name) const
    {
        return makeGetAccountIdByNameRequest(std::set<std::string> { name });
    }
    template <typename... Names>
    BlockchainRequest makeGetAccountIdByNameRequest(const std::string& name, const Names&... names) const
    {
        return makeGetAccountIdByNameRequest(std::set<std::string> { name, names... });
    }
    BlockchainRequest makeGetLastIrreversibleBlockHeaderRequest() const;
    BlockchainRequest makeListPlayerInvitationsRequest(const PlaychainUserId& player, const std::string& last_page_uid, const uint32_t limit) const;
    BlockchainRequest makeListInvitedPlayersRequest(const PlaychainUserId& player, const std::string& last_page_uid, const uint32_t limit) const;
    BlockchainRequest makeGetPlaychainBalanceRequest(const PlaychainUserId& account) const;

    BlockchainDigestTransaction makeCreatePendingBuyinTransaction(
        const std::string& protocol_version,
        const PlaychainUserId& player,
        const std::string& uid,
        const PlaychainMoney amount,
        const std::string& metadata) const;
    BlockchainDigestTransaction makeCancelPendingBuyinTransaction(
        const PlaychainUserId& player,
        const std::string& pending_buyin_uid) const;
    BlockchainDigestTransaction makeResolvePendingBuyinTransaction(
        const PlaychainUserId& table_owner,
        const PlaychainTableId& table,
        const PlaychainPendingBuyinId& pending_buyin) const;
    BlockchainDigestTransaction makeCancelAllPendingBuyinsTransaction(
        const PlaychainUserId& player) const;

    BlockchainRequest makeCheckIfTableAllocatedForPendingBuyinRequest(
        const PlaychainUserId& player,
        const std::string& pending_buyin_uid) const;

    BlockchainRequest makeListTablesWithPlayerRequest(
        const PlaychainUserId& player,
        const uint32_t limit = 1) const;

    BlockchainDigestTransaction makeCreatePlayerInvitationTransaction(
        const PlaychainUserId& inviter,
        const std::string& uid,
        const uint32_t& lifetime_in_sec,
        const std::string& metadata) const;
    //to create mandat with user private key
    std::string getPlayerInvitationDigest(const std::string& inviter,
                                          const std::string& uid) const;
    BlockchainDigestTransaction makeCancelPlayerInvitationTransaction(
        const PlaychainUserId& inviter,
        const std::string& uid) const;
    BlockchainDigestTransaction makeResolvePlayerInvitationTransaction(
        const PlaychainUserId& inviter,
        const std::string& uid,
        const std::string& mandat,
        const std::string& new_pub_key,
        const std::string& new_account_name) const;

    //---- Legacy:
    ///formatted_key - ex. 02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f
    ///                    PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY
    BlockchainRequest makeLegacyLoginRequest(
        const std::string& player, const std::string& formatted_key) const;
    BlockchainRequest makeLegacyGetAccountBalanceRequest(const std::string& player) const;
    BlockchainRequest makeLegacyCreateAccountWithPubkeyRequest(
        const std::string& registrator,
        const std::string& player,
        const std::string& formatted_key) const;

    BlockchainRequest makeLegacyGetAccountIdByNameRequest(const std::set<std::string>& names) const;
    BlockchainRequest makeLegacyGetAccountIdByNameRequest(const std::string& name) const
    {
        return makeLegacyGetAccountIdByNameRequest(std::set<std::string> { name });
    }
    template <typename... Names>
    BlockchainRequest makeLegacyGetAccountIdByNameRequest(const std::string& name, const Names&... names) const
    {
        return makeLegacyGetAccountIdByNameRequest(std::set<std::string> { name, names... });
    }

    BlockchainRequest makeLegacyGetLastBlockHeaderRequest() const;

    BlockchainRequest makeGetBlockchainPropertiesRequest() const;
    BlockchainRequest makeGetPlaychainPropertiesRequest() const;
    //

    BlockchainDigestTransaction makeBuyinTransaction(const PlaychainUserId& player,
                                                     const PlaychainUserId& table_owner,
                                                     const PlaychainTableId& table,
                                                     const PlaychainMoney amount) const;
    BlockchainDigestTransaction makeBuyoutTransaction(
        const PlaychainUserId& player,
        const PlaychainUserId& table_owner,
        const PlaychainTableId& table,
        const PlaychainMoney amount,
        const std::string& reason) const;

    BlockchainDigestTransaction makeVoteForStartGameTransaction(const PlaychainUserId& voter,
                                                                const PlaychainUserId& table_owner,
                                                                const PlaychainTableId& table,
                                                                const GameInitialData& state) const;
    BlockchainDigestTransaction makeVoteForGameResultTransaction(const PlaychainUserId& voter,
                                                                 const PlaychainUserId& table_owner,
                                                                 const PlaychainTableId& table,
                                                                 const GameResult& state) const;

    BlockchainDigestTransaction makeGameResetTransaction(const PlaychainUserId& table_owner,
                                                         const PlaychainTableId& table,
                                                         const bool rollback_table) const;

    BlockchainDigestTransaction makeWithdrawPlaychainVestingBalanceTransaction(
        const PlaychainUserId& account,
        const WithdrawableBalanceInfo& to_withdraw) const;

    BlockchainRequest makeBroadcastTransaction(
        const BlockchainRequest& trx,
        const std::set<std::string>& signatures) const;
    BlockchainRequest makeBroadcastTransaction(
        const BlockchainRequest& trx,
        const std::string& signature) const
    {
        return makeBroadcastTransaction(trx, std::set<std::string> { signature });
    }
    template <typename... Names>
    BlockchainRequest makeBroadcastTransaction(const BlockchainRequest& trx,
                                               const std::string& signature, const Names&... signatures) const
    {
        return makeBroadcastTransaction(trx, std::set<std::string> { signature, signatures... });
    }

    //for identifier == -1 this method creates uniq identifier
    //otherwise it returns passed identifier
    std::pair<BlockchainRequest, int> makeSubscribeChangeTableInfoNotificationRequest(const std::set<PlaychainTableId>& ids, const int identifier = -1) const;
    BlockchainRequest makeCancelSubscriptionForChangeTableInfoNotificationRequest(const std::set<PlaychainTableId>& ids) const;
    BlockchainRequest makeCancelSubscriptionForChangeTableInfoNotificationRequest() const;

    //
    BlockchainRequest makeGetPlayerIdByAccountIdRequest(const PlaychainUserId& account) const;
    BlockchainRequest makeListRoomsRequest(const PlaychainUserId& owner, const uint32_t, const PlaychainRoomId& from = PlaychainRoomId {}) const;
    BlockchainRequest makeGetRoomInfoRequest(const PlaychainUserId& owner, const std::string&) const;
    BlockchainRequest makeGetTablesInfoByMetadataRequest(const PlaychainRoomId& room, const std::string& metadata, const uint32_t) const;
    BlockchainRequest makeListTablesRequest(const PlaychainRoomId& room, const uint32_t, const PlaychainTableId& from = PlaychainTableId {}) const;

    //
    BlockchainDigestTransaction makeTransferRequest(
        const PlaychainUserId& from,
        const PlaychainUserId& to,
        const PlaychainMoney amount) const;

    BlockchainDigestTransaction makeCreateAccountWithPubkeyRequest(
        const PlaychainUserId& registrator,
        const std::string& player,
        const std::string& formatted_key) const;
    BlockchainDigestTransaction makeCreatePlayerByRoomOwnerRequest(
        const PlaychainUserId& room_owner,
        const PlaychainUserId& account) const;
    BlockchainDigestTransaction makeCreateRoomRequest(
        const std::string& protocol_version,
        const PlaychainUserId& room_owner,
        const std::string& server_url,
        const std::string& metadata) const;
    BlockchainDigestTransaction makeCreateTableRequest(
        const PlaychainUserId& room_owner,
        const PlaychainRoomId& room,
        const std::string& metadata,
        const uint16_t required_witnesses,
        const PlaychainMoney min_accepted_proposal_asset) const;

    BlockchainDigestTransaction makeUpdateRoomRequest(
        const std::string& protocol_version,
        const PlaychainRoomId& room,
        const PlaychainUserId& room_owner,
        const std::string& server_url,
        const std::string& metadata) const;
    BlockchainDigestTransaction makeUpdateTableRequest(
        const PlaychainTableId& table,
        const PlaychainUserId& room_owner,
        const std::string& metadata,
        const uint16_t required_witnesses,
        const PlaychainMoney min_accepted_proposal_asset) const;
    BlockchainDigestTransaction makeAliveTableRequest(
        const std::set<PlaychainTableId>& tables,
        const PlaychainUserId& room_owner) const;

    BlockchainDigestTransaction makeUpdateWitnessRequest(
        const PlaychainWitnessId& witness,
        const PlaychainUserId& witness_account,
        const std::string& new_url,
        const std::string& new_signing_key) const;

    BlockchainRequest makeGetBlockchainWitnessRequest(const PlaychainUserId& witness_account) const;

    BlockchainRequest makeGetBlockchainGameWitnessesRequest() const;

    BlockchainRequest makeGetBlockchainAccountsRequest(const std::vector<PlaychainUserId>& accounts) const;

    /* (To get data for this method, you will most likely need a request to the blockchain.
     *  But at least one call must be done)
     * -------------------------------------------------------------------------------------
     * setChainInfo may be updated at least half TRANSACTION_EXPIRATION_PERIOD
     *
     * Method initializes necessary part of transaction data that used by blockchain for
     * chain_id, expiration and TaPoS checks
     *
    */
    void setChainInfo(const PlaychainBlockHeaderInfo& info);

private:
    mutable PlaychainSettings m_settings;
    std::unique_ptr<PlaychainRequestBuilderContext> m_context;
};

} // namespace tp
