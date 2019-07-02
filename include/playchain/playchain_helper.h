#pragma once

#include <playchain/playchain_types.h>

#include <array>

namespace tp {

PrivateKey priv_key_from_brain_key(const std::string& brain_key);
std::string priv_key_to_wif(const PrivateKey& key);
PrivateKey priv_key_from_wif(const std::string& wif_key);

CompressedPublicKey public_key_from_key(const PrivateKey& key);
///pub_key - ex. 02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f
///              PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY (wellformatted)
CompressedPublicKey public_key_from_string(const std::string& pub_key);
///for wellformatted it returns PLC6BcNK8CWGj6herX8nvhwEJ625QuaPAtmZPZ6yxQafFnSFnX9VY
///otherwise raw key 02aa923ff63544ea12f0057dd81830db49cf590ba52ee7ae7e004b3f4fc06be56f (for legacy only)
std::string public_key_to_string(const CompressedPublicKey& pub_key, bool wellformatted = true);

///for wellformatted it will add prefix 'PLC' to address
std::string address_from_public_key(const CompressedPublicKey&, bool wellformatted = true);

Digest convert_digest(const std::string& hex_digest);
CompactSignature convert_signature(const std::string& hex_sign);

CompactSignature sign_digest(const Digest& digest, const PrivateKey& key, bool check_canonical = true);
bool check_signature(const CompactSignature& sign, const Digest& digest, const CompressedPublicKey& key, bool check_canonical = true);

std::string to_hex(const char* d, uint32_t s);
std::string to_hex(const uint8_t* d, uint32_t s);

size_t from_hex(const std::string& hex_str, char* out_data, size_t out_data_len);
size_t from_hex(const std::string& hex_str, uint8_t* out_data, size_t out_data_len);

uint32_t create_pseudo_random_from_time(const uint32_t offset = 0);
std::string create_pseudo_random_string_from_time(const uint32_t offset = 0);

PlaychainPercent get_percent(const float&);

} // namespace tp
