//export module nxm;
#define export
#include <command-structure.h>
#include <command.h>
#include <iostream>
#include <gumbo-query/Document.h>
#include <cpr/cpr.h>

namespace globals {
    std::string dep_pattern = "(Nexus requirements|Off-site requirements)";
    std::string apikey;
}

export int nxm(std::string &apikey, const Nxm &cli){
    globals::apikey = apikey;
    cpr::Response r = cpr::Get(cpr::Url("https://api.nexusmods.com/v1/users/validate.json"),
                               cpr::Header{{"apikey",apikey}});
    if(r.status_code == 200){
        auto nxm_sub = cli.nxm.get_subcommands();
        if(nxm_sub.size() == 1){
            Command command(nxm_sub[0], cli);
            switch(command.sendRequest()){
                case -1: // no request was sent
                    return 1;
                case 0: // command sent request
                    break;
                case 1: // sub command sent request
                    break;
            }
            std::cout << command.output();
            return 0;
        }
    } else {
        std::cerr << "The API key (field: \"apikey\") in ~/.nxm.json is invalid." << std::endl;
    }
    return 1;
}
