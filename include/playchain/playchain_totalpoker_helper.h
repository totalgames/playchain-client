#pragma once

#include <playchain/playchain_types.h>

namespace tp {

struct PlaychainTotalpokerTableInfo
{
    PlaychainTotalpokerTableInfo() = default;

    bool valid() const
    {
        return table.valid();
    }

    PlaychainTableInfo table;
    PlaychainMoney big_blind_price = 0;
    PlaychainPercent rake = 0;
    size_t min_blind_count = 0;
    size_t max_blind_count = 0;
    size_t blind_chips_count = 0;
    std::string info;

    static const char* gameMark();
    static std::string createMetadata(const PlaychainMoney big_blind_price,
                                      const PlaychainPercent& rake,
                                      const size_t min_blind_count,
                                      const size_t max_blind_count,
                                      const size_t blind_chips_count,
                                      const std::string& info = {});

    static PlaychainTotalpokerTableInfo create(const PlaychainTableInfo& other, const std::string& game_mark = {});

protected:
    PlaychainTotalpokerTableInfo(const PlaychainTableInfo& other);
};

} // namespace tp
