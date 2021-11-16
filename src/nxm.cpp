//export module nxm;
#define export
#include <command-structure.h>
#include <command.h>
#include <iostream>
#include <filesystem>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <gumbo-query/Document.h>

namespace fs = std::filesystem;
namespace nlm = nlohmann;

namespace globals {
    std::string dep_pattern = "(Nexus requirements|Off-site requirements)";
    std::string apikey;
}

int parse_response(const Command &c, const Nxm &cli);
export int nxm(std::string &apikey, const Nxm &cli){
    globals::apikey = apikey;
    cpr::Response r = cpr::Get(cpr::Url("https://api.nexusmods.com/v1/users/validate.json"),
                               cpr::Header{{"apikey",apikey}});
    if(r.status_code == 200){
        auto nxm_sub = cli.nxm.get_subcommands();
        if(nxm_sub.size() == 1){
            Command command(nxm_sub[0], cli);
            if(command.good()) {
                if (command.sendRequest() == 0) {
                    return parse_response(command, cli);
                }
            } else {
                std::cerr << command.getError() << std::endl;
            }
            return 1;
        }
    } else {
        std::cerr << "The API key (field: \"apikey\") in ~/.nxm.json is invalid." << std::endl;
    }
    return 1;
}

extern int web_scraper(const Nxm &cli);
int parse_response(const Command &c, const Nxm &cli){
    if(c.responseCode() == 200) {
        nlm::json json = nlm::json::parse(c.output());
        switch (c.type()) {
            case type::download:
                break;
            case type::track:
                break;
            case type::untrack:
                break;
            case type::endorse:
                break;
            case type::abstain:
                break;
            case type::list_games:
                for(const auto &game : json){
                    if(game["domain_name"].is_string()){
                        std::cout << game["domain_name"].get<std::string>() << std::endl;
                    }
                }
                break;
            case type::list_tracked:
                break;
            case type::list_endorsed:
                break;
            case type::list_trending:
                break;
            case type::list_files:
                break;
            case type::list_dependencies:
                return web_scraper(cli);
            case type::INVALID:
            case type::list:
                return -1;
        }
        return 0;
    }
    return c.responseCode();
}