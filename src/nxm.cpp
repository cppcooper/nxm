//export module nxm;
#define export
#include <command-structure.h>
#include <string>
#include <iostream>
#include <gumbo-query/Document.h>
#include <cpr/cpr.h>

std::string dep_pattern = "(Nexus requirements|Off-site requirements)";

namespace command {
    enum type{
        INVALID,
        download,
        track,
        untrack,
        endorse,
        abstain,
        list,
        list_games,
        list_tracked,
        list_trending
    };
}

command::type ParseSubcommand(CLI::App* cli){
    if(cli->get_name() == "download")
        return command::download;
    else if(cli->get_name() == "track")
        return command::track;
    else if(cli->get_name() == "untrack")
        return command::untrack;
    else if(cli->get_name() == "endorse")
        return command::endorse;
    else if(cli->get_name() == "abstain")
        return command::abstain;
    else if(cli->get_name() == "list")
        return command::list;
    else if(cli->get_name() == "games")
        return command::list_games;
    else if(cli->get_name() == "tracked")
        return command::list_tracked;
    else if(cli->get_name() == "endorsed")
        return command::list_tracked;
    else if(cli->get_name() == "trending")
        return command::list_trending;
    return command::INVALID;
}

export int nxm(std::string &apikey, Nxm &cli){
    cpr::Response r = cpr::Get(cpr::Url("https://api.nexusmods.com/v1/users/validate.json"),
                               cpr::Header{{"apikey",apikey}});
    if(r.status_code == 200){
        auto nxm_sub = cli.nxm.get_subcommands();
        if(nxm_sub.size() == 1){
            auto command = nxm_sub[0];
            switch(ParseSubcommand(command)){
                case command::INVALID:
                    return 1;
                    break;
                case command::download:

                    if(!cli.print){

                    }
                    break;
                case command::track:
                    break;
                case command::untrack:
                    break;
                case command::endorse:
                    break;
                case command::abstain:
                    break;
                case command::list:
                    break;
                case command::list_games:
                    break;
                case command::list_tracked:
                    break;
                case command::list_trending:
                    break;
            }
            return 0;
        }
    }
    return 1;
}