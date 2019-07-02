#include <playchain/playchain_totalpoker_helper.h>

#include "playchain_defines.h"
#include "pack_helper.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <sstream>

namespace tp {
using namespace playchain;

PlaychainTotalpokerTableInfo::PlaychainTotalpokerTableInfo(const PlaychainTableInfo& other)
    : table(other)
{
}

const char* PlaychainTotalpokerTableInfo::gameMark()
{
    return "Total-Poker";
}

std::string PlaychainTotalpokerTableInfo::createMetadata(const PlaychainMoney big_blind_price,
                                                         const PlaychainPercent& rake,
                                                         const size_t min_blind_count,
                                                         const size_t max_blind_count,
                                                         const size_t blind_chips_count,
                                                         const std::string& info)
{
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);

    PLAYCHAIN_ASSERT(big_blind_price > 0, "Invalid min big blind price");
    PLAYCHAIN_ASSERT(min_blind_count > 0, "Invalid min big blind count");
    PLAYCHAIN_ASSERT(max_blind_count > 0, "Invalid max big blind count");
    PLAYCHAIN_ASSERT(min_blind_count <= max_blind_count, "Invalid min/max big blind count");
    PLAYCHAIN_ASSERT(blind_chips_count > 0, "Invalid big blind chips count");

    writer.StartObject();
    std::string game_name { gameMark() };
    pack_field(writer, "game", game_name);
    pack_field(writer, "bb_price", big_blind_price);
    pack_field(writer, "rake", rake);
    pack_field(writer, "min_bb", (uint32_t)min_blind_count);
    pack_field(writer, "max_bb", (uint32_t)max_blind_count);
    pack_field(writer, "chips_bb", (uint32_t)blind_chips_count);
    if (!info.empty())
        pack_field(writer, "info", info);
    writer.EndObject();

    return { buff.GetString() };
}

PlaychainTotalpokerTableInfo PlaychainTotalpokerTableInfo::create(const PlaychainTableInfo& other, const std::string& game_mark)
{
    try
    {
        PlaychainTotalpokerTableInfo result { other };

        rapidjson::Document document;
        document.Parse(other.metadata.c_str());

        PLAYCHAIN_ASSERT_JSON(!document.HasParseError());

        PLAYCHAIN_ASSERT_JSON(document.HasMember("game") && document["game"].IsString());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("bb_price") && document["bb_price"].IsInt64());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("rake") && document["rake"].IsInt());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("min_bb") && document["min_bb"].IsInt());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("max_bb") && document["max_bb"].IsInt());
        PLAYCHAIN_ASSERT_JSON(document.HasMember("chips_bb") && document["chips_bb"].IsInt());

        std::string game_name = document["game"].GetString();
        if (!game_mark.empty())
        {
            PLAYCHAIN_ASSERT_JSON(game_name == game_mark);
        }
        else
        {
            PLAYCHAIN_ASSERT_JSON(game_name == gameMark());
        }

        result.big_blind_price = document["bb_price"].GetInt64();
        PLAYCHAIN_ASSERT_JSON(result.big_blind_price > 0);

        result.rake = document["rake"].GetInt();
        PLAYCHAIN_ASSERT_JSON(result.rake > 0);

        result.min_blind_count = document["min_bb"].GetInt();
        PLAYCHAIN_ASSERT_JSON(result.min_blind_count > 0);
        result.max_blind_count = document["max_bb"].GetInt();
        PLAYCHAIN_ASSERT_JSON(result.max_blind_count > 0);
        PLAYCHAIN_ASSERT_JSON(result.max_blind_count >= result.min_blind_count);
        result.blind_chips_count = document["chips_bb"].GetInt();
        PLAYCHAIN_ASSERT_JSON(result.blind_chips_count > 0);

        if (document.HasMember("info"))
        {
            PLAYCHAIN_ASSERT_JSON(document["info"].IsString());

            result.info = document["info"].GetString();
        }

        return result;
    }
    catch (std::exception& /*e*/)
    {
        //LOG_ERROR(e.what());
    }

    return {};
}

} // namespace tp
