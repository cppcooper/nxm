#include <command-structure.h>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
namespace nlm = nlohmann;

fs::path GetHome() {
    std::string home_path;
#ifdef unix
    home_path = getenv("HOME");
#else //if defined(_WIN32)
    home_path = getenv("HOMEDRIVE");
    std::string path = getenv("HOMEPATH");
    home_path.append(path);
#endif
    return home_path;
}


extern int nxm(std::string &apikey, const Nxm &cli);
int main(int argc, char *argv[]) {
    Nxm cli;
    // we need to load the config file (has API key inside)
    fs::path home = GetHome();
    fs::path config_path(home.string().append("/.nxm.json"));
    // and if the file doesn't exist we create one and inform the user to populate it with an API key
    if (!fs::exists(config_path)) {
        std::fstream config_file(config_path, std::fstream::out);
        config_file << "{\n\"apikey\" : \"\"\n}\n";
        config_file.close();
        std::cerr << "No API key configured. Acquire and place an apikey into " << config_path << std::endl;
    } else {
        // If the file does exist we'll try to load it
        size_t size = fs::file_size(config_path);
        char* buffer = new char[size];
        std::fstream config_file(config_path);
        config_file.read(buffer, size);
        config_file.close();
        if (config_file.good()) {
            try {
                // Now we parse the file for the API key
                nlm::json config = nlm::json::parse(buffer);
                auto key = config["apikey"];
                // We can't continue if the key doesn't exist, or it doesn't contain a string
                if (!key.is_null() && key.is_string()) {
                    std::string apikey = key.get<std::string>();
                    // Now it makes sense to parse the command line arguments
                    CLI11_PARSE(cli.nxm, argc, argv);
                    // Run the program
                    int status = nxm(apikey, cli);
                    // Parse return code and print corresponding info
                    if (status != 0) {
                        switch (status) {
                            case -1:
                                std::cerr << "Couldn't send request to nexus." << std::endl;
                                break;
                            case -2:
                                std::cerr << "The API key (field: \"apikey\") in ~/.nxm.json is invalid." << std::endl;
                                break;
                            case 2:
                                return 1;
                            default:
                                std::cerr << "An unknown error has occurred. Please report this." << std::endl;
                        }
                        return 1;
                    }
                    return 0;
                } else {
                    // If the API key can't be retrieved we advise the user to configure things
                    std::cerr << "No API key configured. Acquire and place an apikey into " << config_path << std::endl;
                }
            } catch (nlm::json::exception e) {
                std::cerr << e.what();
            }
        }
    }
    return 1;
}