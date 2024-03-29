#include <command-structure.h>
#include <command.h>
#include <iostream>
#include <filesystem>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
namespace nlm = nlohmann;

namespace globals {
    std::string apikey;
}

int parse_response(const Command &c, const Nxm &cli);
int nxm(std::string &apikey, const Nxm &cli){
    // We need to validate the API key first
    globals::apikey = apikey;
    cpr::Response r = cpr::Get(cpr::Url("https://api.nexusmods.com/v1/users/validate.json"),
                               cpr::Header{{"apikey",apikey}});
    if(r.status_code == 200){
        auto nxm_sub = cli.nxm.get_subcommands();
        if(nxm_sub.size() == 1){
            // Prepare the command the user specified in the command line
            Command command(nxm_sub[0], cli);
            if(command.good()) {
                // Execute
                int status = command.sendRequest();
                if (status != 0) {
                    return status;
                }
                // Parse the response to the command
                return parse_response(command, cli);
            } else {
                std::cerr << command.getError() << std::endl;
                return 2;
            }
        }
        std::cerr << "Command was malformed. See `nxm --help`" << std::endl;
        return 2;
    }
    return -2;
}

int parse_response(const Command &c, const Nxm &cli){
    //std::cout << c.getURI() << std::endl;
    if(c.responseCode() == 200) {
        nlm::json json = nlm::json::parse(c.output());
        bool print_once = false;
        switch (c.type()) {
            case type::download:
                //todo: need premium to view format
                std::cout << "not implemented yet" << std::endl;
                break;
            case type::list_games:
                for(const auto &game : json){
                    if(game["domain_name"].is_string()){
                        std::cout << game["domain_name"].get<std::string>() << std::endl;
                    }
                }
                break;
            case type::list_tracked:
            case type::list_endorsed:
                for(const auto &mod : json){
                    if(mod["domain_name"].get<std::string>() == cli.arg1 ) {
                        std::cout << mod["mod_id"] << std::endl;
                    }
                }
                break;
            case type::list_trending:
                for(const auto &mod : json){
                    std::cout << mod["mod_id"] << " " << mod["name"] << std::endl;
                }
                break;
            case type::list_files:
                for(const auto &file : json["files"]){
                    if(!print_once){
                        print_once = true;
                        std::cout << file["name"] << std::endl;
                    }
                    std::cout << file["file_id"] << " " << file["file_name"] << std::endl;
                }
                break;
            // These commands are actions
            case type::track:
            case type::untrack:
            case type::endorse:
            case type::abstain:
                // if we're this far then we likely have done the action
                std::cout << "did thing " << c.name() << std::endl;
                break;
            case type::INVALID:
            case type::list:
            default:
                return -1;
        }
        return 0;
    }
    if(c.type() == type::list_dependencies){
        for(auto &j : c.getJson()){
            std::cout << std::setw(4) << j << std::endl;
        }
        return 0;
    }
    if(c.responseCode() != 0) {
        std::cerr << "Error " << c.responseCode() << std::endl << c.output() << std::endl;
    }
    return c.responseCode();
}
