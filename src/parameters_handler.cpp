#include "parameters_handler.hpp"

namespace FSMonitor
{
parametrs parameters_handler::operator()(int argc, char** argv)
{
    parametrs result;

    for(int i = 0; i < argc; i++)
    {
        std::string str;
        switch (argv[i][0])
        {
        case 'r':
            str = argv[i];
            str.substr(str.find('=') + 1, str.size());
            mask = str;
            break;
        case '-':
            parse_flags(argc, argv, i);
            break;
        default:
            path.assign(argv[i]);
            if(!std::filesystem::exists(path))
            {
                std::cout << "Incorrect base catalog path" << std::endl;
                throw std::exception();
            }
            break;
        }
    }

    std::get<0>(result) = path;
    std::get<1>(result) = thread_count;
    std::get<2>(result) = filter(mask, restr_types);
    std::get<3>(result) = trust_flag;

    return result;
}

void parameters_handler::parse_flags(int argc, char** argv, int begin)
{
    for(int i = begin; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            std::cout << "Invalid options/paramters order" << std::endl;
            throw std::exception();
        }

        std::string types;
        int restricted_types = 0;
        switch (argv[i][1])
        {
        case 'h':
            display_help();
            throw std::exception();
            break;
        case 'c':
            thread_count = argv[i][2] - '0';
            if(thread_count < 1 || thread_count > std::thread::hardware_concurrency())
                thread_count = std::thread::hardware_concurrency();
            break;
        case 'f':
            trust_flag = true;
            break;
        case 't':
            types = argv[i];
            for(size_t i = 2; i < types.size(); i++)
            {
                switch (types[i])
                {
                    case 'n':
                        restricted_types |= filter::none;
                        break;
                    case 'r':
                        restricted_types |= filter::regular;
                        break;
                    case 'd':
                        restricted_types |= filter::directory;
                        break;
                    case 's':
                        restricted_types |= filter::symlink;
                        break;
                    case 'b':
                        restricted_types |= filter::block;
                        break;
                    case 'c':
                        restricted_types |= filter::character;
                        break;
                    case 'f':
                        restricted_types |= filter::fifo;
                        break;
                    case 'o':
                        restricted_types |= filter::socket;
                        break;
                    case 'u':
                        restricted_types |= filter::unknown;
                        break;
                    default:
                        std::cout << "Unreckognized restricted type" << std::endl;
                        throw std::exception();
                }
            }
            restr_types = (filter::flags)restricted_types;
            break;
        default:
            std::cout << "Unreckognized option" << std::endl;
            throw std::exception();
            break;
        }
    }
}

void parameters_handler::display_help()
{
    std::cout << "./fsm [PATH] regex=[REGEX] [OPTIONS]" << std::endl;
    std::cout << "  PATH - base catalog of scanning" << std::endl;
    std::cout << "  REGEX - regex value filtering filenames" << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    -h        - display help message\n"
                    "    -c[num]   - init scanning threads number\n"
                    "    -f        - toggle trust filesystem mode\n"
                    "    -t{types} - add filesystem types to filter\n"
                    "    types:\n"
                    "      n - none\n"
                    "      r - regular\n"
                    "      d - catalog\n"
                    "      s - symlink\n"
                    "      b - block\n"
                    "      c - charachter\n"
                    "      f - fifo\n"
                    "      o - socket\n"
                    "      u - unknown"
                    << std::endl;
    std::cout << "  Some ignoring types may not cause any difference on concrete platform." << std::endl;
    std::cout << "  Filesystem trust mode may cause incorrect validation of files in case of malware root access" << std::endl;
    std::cout << "  By default regex parametr is \"empty\" and thread_count = std::thread::hardware_concurrency()" << std::endl;
    std::cout << "  It is also not recommended, but allowed to use this utility outside user-space for volatile files like sockets" << std::endl;
}

}