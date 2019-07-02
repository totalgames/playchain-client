#include <boost/program_options.hpp>

#include <iostream>
#include <iomanip>
#include <termios.h>

#include <playchain/playchain_helper.h>

namespace {
namespace bpo = boost::program_options;

void set_program_options(bpo::options_description& cli)
{
    // clang-format off
    cli.add_options()
            ("help,h", "Print this help message and exit.")
            ("wif,k",  bpo::value<std::string>(), "WIF private key");
    // clang-format on
}

bool check_mandatory_options(std::ostream& stream, const bpo::variables_map& options)
{
    if (!options.count("wif"))
    {
        stream << "WIF key is requied"
               << "\n";
        return false;
    }

    return true;
}
} // namespace

int main(int argc, char* argv[])
{
    static const char options_title[] = "Public Key From WIF Private Key";
    bpo::options_description cli(options_title);
    boost::program_options::variables_map options;

    try
    {
        set_program_options(cli);

        bpo::store(bpo::parse_command_line(argc, argv, cli), options);
        bpo::notify(options);
    }
    catch (const bpo::error& e)
    {
        std::cerr << "Error parsing command line: " << e.what() << "\n";
        return 1;
    }

    if (options.count("help") || !check_mandatory_options(std::cerr, options))
    {
        cli.print(std::cout);
        return 1;
    }

    try
    {
        auto&& priv_key = tp::priv_key_from_wif(options["wif"].as<std::string>());
        auto&& pub_key = tp::public_key_from_key(priv_key);

        std::cout << tp::public_key_to_string(pub_key, true) << '\n';
        std::cout << std::flush;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 2;
    }

    return 0;
}
