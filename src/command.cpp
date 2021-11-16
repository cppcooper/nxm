#include <command.h>

namespace globals {
    extern std::string apikey;
}

int Command::sendRequest(){
    switch(request_type){
        case type::GET:
            r = cpr::Get(cpr::Url(uri),
                         cpr::Header{{"apikey", globals::apikey}});
            break;
        case type::POST:
            switch(command_type){
                case type::endorse:
                case type::abstain:
                    r = cpr::Post(cpr::Url(uri),
                                  cpr::Header{{"apikey", globals::apikey}});
                    break;
                // URI is void of parameters
                case type::track:
                    r = cpr::Post(cpr::Url(uri),
                                  cpr::Header{{"apikey", globals::apikey}},
                                  cpr::Parameters{{"domain_name", cli.arg1},
                                                  {"mod_id",      cli.arg2}});
                    break;
            }
            break;
        case type::DELETE:
            r = cpr::Delete(cpr::Url(uri),
                            cpr::Header{{"apikey", globals::apikey}},
                            cpr::Parameters{{"domain_name", cli.arg1},
                                            {"mod_id",      cli.arg2}});
            break;
        case type::NONE:
            if(command_type != type::list){
                return -1;
            }
            if (command->get_subcommands().size() != 1) {
                return -1;
            }
            auto sub_command = command->get_subcommands()[0];
            Command c2(sub_command, cli, this);
            return c2.sendRequest();
    }
    switch(command_type){
        case type::list_games:
        case type::list_tracked:
        case type::list_endorsed:
        case type::list_trending:
        case type::list_files:
            if(parent) {
                parent->r = cpr::Response(r);
                parent->command_type = command_type;
                parent->request_type = request_type;
            } else {
                return -1;
            }
    }
    return 0;
}

void Command::make_uri() {
    std::stringstream buffer;
    buffer << "http://api.nexusmods.com";
    switch(command_type){
        case type::INVALID:
            break;
        case type::download:
            buffer << download_uri;
            break;
        case type::track:
            buffer << track_uri;
            break;
        case type::untrack:
            buffer << untrack_uri;
            break;
        case type::endorse:
            buffer << endorse_uri;
            break;
        case type::abstain:
            buffer << abstain_uri;
            break;
        case type::list:
        case type::list_games:
            buffer << list_games_uri;
            break;
        case type::list_tracked:
            buffer << list_tracked_uri;
            break;
        case type::list_endorsed:
            buffer <<  list_endorsed_uri;
            break;
        case type::list_trending:
            buffer <<  list_trending_uri;
            break;
        case type::list_files:
            buffer <<  list_files_uri;
            break;
    }
    uri = buffer.str();
    auto replace = [](std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    };
    switch(command_type){
        case type::download:
            valid_args = !(cli.arg1.empty() || cli.arg2.empty() || cli.arg3.empty());
            error = "usage: nxm download <game> <mod> <file>";
            replace(uri, "{game_domain_name}", cli.arg1);
            replace(uri, "{mod_id}", cli.arg2);
            replace(uri, "{id}", cli.arg3);
            break;
        case type::endorse:
        case type::abstain:
            valid_args = !(cli.arg1.empty() || cli.arg2.empty());
            error = "usage: nxm (endorse|abstain) <game> <mod>";
            replace(uri, "{game_domain_name}", cli.arg1);
            replace(uri, "{id}", cli.arg2);
            break;
        case type::list_trending:
            valid_args = !(cli.arg1.empty());
            error = "usage: nxm list trending <game>";
            replace(uri, "{game_domain_name}", cli.arg1);
            break;
        case type::list_files:
            valid_args = !(cli.arg1.empty() || cli.arg2.empty());
            error = "usage: nxm list mod-files <game> <mod>";
            replace(uri, "{game_domain_name}", cli.arg1);
            replace(uri, "{mod_id}", cli.arg2);
            break;
        default:
            valid_args = true;
    }
}

void Command::parse_command_type(const std::string &command){
    if(command == "download") {
        command_type = type::download;
        request_type = type::GET;
    }
    else if(command == "track") {
        command_type = type::track;
        request_type = type::POST;
    }
    else if(command == "untrack") {
        command_type = type::untrack;
        request_type = type::DELETE;
    }
    else if(command == "endorse") {
        command_type = type::endorse;
        request_type = type::POST;
    }
    else if(command == "abstain") {
        command_type = type::abstain;
        request_type = type::POST;
    }
    else if(command == "list") {
        command_type = type::list;
        request_type = type::NONE;
    }
    else if(command == "games") {
        command_type = type::list_games;
        request_type = type::GET;
    }
    else if(command == "tracked") {
        command_type = type::list_tracked;
        request_type = type::GET;
    }
    else if(command == "endorsed") {
        command_type = type::list_endorsed;
        request_type = type::GET;
    }
    else if(command == "trending") {
        command_type = type::list_trending;
        request_type = type::GET;
    }
    else if(command == "mod-files") {
        command_type = type::list_files;
        request_type = type::GET;
    }
    else {
        command_type = type::INVALID;
        request_type = type::NONE;
    }
}
