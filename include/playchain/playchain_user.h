#pragma once

#include <playchain/playchain_types.h>

#include <memory>

namespace tp {
struct PlaychainUserContext;

class PlaychainUser
{
public:
    PlaychainUser(const std::string& name,
                  const PlaychainUserId& id,
                  const std::string& private_key_wif);
    ~PlaychainUser();

    const std::string name() const;

    const PlaychainUserId id() const;

    std::string signDigest(const std::string& digest) const;

    CompressedPublicKey getPublicKey() const;
    std::string getSerializedPublicKey() const;

private:
    std::unique_ptr<PlaychainUserContext> m_context;
};

} // namespace tp
