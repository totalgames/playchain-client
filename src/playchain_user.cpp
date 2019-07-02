#include <playchain/playchain_user.h>
#include <playchain/playchain_helper.h>

#include "private_key_sec.h"

#include "convert_helper.h"
#include "sha256.h"
#include <cstring>

namespace tp {

using namespace playchain;

struct PlaychainUserContext
{
    PlaychainUserContext(
        const std::string& name,
        const PlaychainUserId& id,
        const std::string& private_key_wif, const uint64_t noise)
        : name(name)
        , id(id)
        , private_key(private_key_wif, noise)
    {
    }

    std::string name;
    PlaychainUserId id;
    private_key_sec private_key;
};

PlaychainUser::PlaychainUser(
    const std::string& name,
    const PlaychainUserId& id,
    const std::string& private_key_wif)
{
    static uint32_t i = 0;

    uint64_t rnd = create_pseudo_random_from_time(++i);

    m_context.reset(new PlaychainUserContext(name, id, private_key_wif, rnd));
}

PlaychainUser::~PlaychainUser() {}

const std::string PlaychainUser::name() const
{
    return m_context->name;
}

const PlaychainUserId PlaychainUser::id() const
{
    return m_context->id;
}

#ifdef SECP256K1
std::string PlaychainUser::signDigest(const std::string& hex_digest) const
{
    CompactSignature signature = sign_digest(convert_digest(hex_digest), m_context->private_key.decrypt());

    return playchain::to_hex(signature);
}
#else //SECP256K1
std::string PlaychainUser::signDigest(const std::string&) const
{
    PLAYCHAIN_ERROR("Required SECP256K1 lib");
    return {};
}
#endif //!SECP256K1

CompressedPublicKey PlaychainUser::getPublicKey() const
{
    return m_context->private_key.get_public();
}

std::string PlaychainUser::getSerializedPublicKey() const
{
    return m_context->private_key.get_public_str();
}

} // namespace tp
