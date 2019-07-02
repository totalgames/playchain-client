#pragma once

#include <boost/test/unit_test.hpp>

#include <playchain/request_builder.h>
#include <playchain/response_parser.h>

#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace playchain {
namespace test {
    using namespace tp;

    struct builder_fixture
    {
        builder_fixture()
        {
            PlaychainSettings settings;
            settings.make_same_transactions_uniq = false;

            _builder.reset(new PlaychainRequestBuilder("d00c8b97e30d17e5609ead47e90ba29dae83d9c894387139cda0ac7cb0637b84", settings));
        }

        void set_chain_info();
        std::vector<std::string> get_signatires_from_params_json(const std::string& json);

        const PlaychainRequestBuilder& builder()
        {
            BOOST_REQUIRE(_builder);
            return *_builder;
        }

    private:
        std::unique_ptr<PlaychainRequestBuilder> _builder;
    };

    struct parser_fixture
    {
        parser_fixture()
        {
        }

        PlaychainResponseParser parser;
    };

    std::string format_json(const std::string&);
    std::string remove_formatting(const std::string&);

    template <typename Stream>
    void dump_json(Stream& s, const std::string& json)
    {
        s << ">>>\n";
        s << format_json(json) << '\n';
        s << "<<<\n";
        s << std::flush;
    }
} // namespace test
} // namespace playchain

#ifdef NDEBUG
#define DUMP_JSON(json) (json)
#else
#define DUMP_JSON(json) \
    playchain::test::dump_json(std::cerr, json)
#endif
