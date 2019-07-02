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
            ("name,n",  bpo::value<std::string>(), "Account name")
            ("secret,s", bpo::value<std::string>(), "Password or brain key")
            ("wif,w", bpo::value<std::string>(), "WIF-key")
            ("public,p", bpo::value<std::string>(), "Public key string")
            ("print-public",  bpo::value<bool>()->default_value(true), "Print compressed and formatted public key")
            ("print-public-raw",  bpo::value<bool>()->default_value(false), "Print compressed raw public key")
            ("print-private",  bpo::value<bool>()->default_value(false), "Print private key")
            ("print-address",  bpo::value<bool>()->default_value(false), "Print address from public key");
    // clang-format on
}

bool check_mandatory_options(std::ostream& stream, const bpo::variables_map& options)
{
    if (!options.count("name") && !options.count("public"))
    {
        stream << "Account name is requied"
               << "\n";
        return false;
    }

    return true;
}

char getch_without_echo()
{
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return (char)ch;
}

std::string get_secret(const std::string& prompt, bool show_asterisk)
{
    const char backspace_char = 127;
    const char return_char = '\n';

    std::string ret;
    char ch = 0;

    std::cerr << prompt << " ";

    while ((ch = getch_without_echo()) != return_char)
    {
        if (ch == backspace_char)
        {
            if (ret.length() != 0)
            {
                if (show_asterisk)
                    std::cerr << "\b \b";
                ret.resize(ret.length() - 1);
            }
        }
        else
        {
            ret += ch;
            if (show_asterisk)
                std::cerr << '*';
        }
    }
    std::cerr << std::endl;
    return ret;
}
} // namespace

int main(int argc, char* argv[])
{
    static const char options_title[] = "Keys From Login";
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
        std::string name;
        tp::CompressedPublicKey pub_key;

        if (options.count("name"))
        {
            name = options["name"].as<std::string>();

            tp::PrivateKey priv_key;
            std::string pwd;
            if (!options.count("secret") && !options.count("wif"))
            {
                pwd = get_secret("Enter secret or brain key:", true);
            }
            else if (options.count("secret"))
            {
                pwd = options["secret"].as<std::string>();
            }

            if (!pwd.empty())
            {
                priv_key = tp::priv_key_from_brain_key(name + "active" + pwd);
            }else if (options.count("wif"))
            {
                priv_key = tp::priv_key_from_wif(options["wif"].as<std::string>());
            }

            if (options["print-private"].as<bool>())
            {
                std::cout << tp::priv_key_to_wif(priv_key) << '\n';
            }

            pub_key = tp::public_key_from_key(priv_key);
        }
        else if (options.count("public"))
        {
            pub_key = tp::public_key_from_string(options["public"].as<std::string>());
        }

        if (options["print-public"].as<bool>())
        {
            std::cout << tp::public_key_to_string(pub_key, true) << '\n';
        }
        if (options["print-public-raw"].as<bool>())
        {
            std::cout << tp::public_key_to_string(pub_key, false) << '\n';
        }
        if (options["print-address"].as<bool>())
        {
            std::cout << tp::address_from_public_key(pub_key, true) << '\n';
        }
        std::cout << std::flush;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 2;
    }

    return 0;
}
