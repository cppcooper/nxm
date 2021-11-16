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
int main(int argc, char *argv[]){
    Nxm cli;
    fs::path home = GetHome();
    fs::path config_path(home.string().append("/.nxm.json"));
    if(!fs::exists(config_path)){
        std::fstream config_file(config_path, std::fstream::out);
        config_file << "{\n\"apikey\" : \"\"\n}\n";
        config_file.close();
        std::cerr << "No API key configured. Acquire and place an apikey into " << config_path << std::endl;
    } else {
        size_t size = fs::file_size(config_path);
        char* buffer = new char[size];
        std::fstream config_file(config_path);
        config_file.read(buffer,size);
        //std::cout << buffer << std::endl;
        try {
            nlm::json config = nlm::json::parse(buffer);
            auto key = config["apikey"];
            if (!key.is_null() && key.is_string()) {
                std::string apikey = key.get<std::string>();
                CLI11_PARSE(cli.nxm, argc, argv);
                return nxm(apikey, cli);
            } else {
                std::cerr << "No API key configured. Acquire and place an apikey into " << config_path << std::endl;
            }
        } catch (nlm::json::exception e) {
            std::cerr << e.what();
        }
    }
    return 1;
}