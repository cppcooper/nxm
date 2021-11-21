#pragma once
#include <command-structure.h>
#include <string>
#include <cstring>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace nlm = nlohmann;

namespace type {
    enum command{
        download,
        track,
        untrack,
        endorse,
        abstain,
        list,
        list_games,
        list_tracked,
        list_endorsed,
        list_trending,
        list_files,
        list_dependencies,
        INVALID = 0xFF
    };
    enum request{
        GET,
        POST,
        DELETE,
        NONE = 0xFF
    };
}

class Command {
private:
    const char* download_uri = "/v1/games/{game_domain_name}/mods/{mod_id}/files/{id}/download_link.json"; //GET
    const char* track_uri = "/v1/user/tracked_mods.json"; //POST
    const char* untrack_uri = "/v1/user/tracked_mods.json"; //DELETE
    const char* endorse_uri = "/v1/games/{game_domain_name}/mods/{id}/endorse.json"; //POST
    const char* abstain_uri = "/v1/games/{game_domain_name}/mods/{id}/abstain.json"; //POST
    const char* list_games_uri = "/v1/games.json"; //GET
    const char* list_tracked_uri = "/v1/user/tracked_mods.json"; //GET
    const char* list_endorsed_uri = "/v1/user/endorsements.json"; //GET
    const char* list_trending_uri = "/v1/games/{game_domain_name}/mods/trending.json"; //GET
    const char* list_files_uri = "/v1/games/{game_domain_name}/mods/{mod_id}/files.json"; //GET
protected:
    Command* parent = nullptr;
    const Nxm &cli;
    const CLI::App* command;
    cpr::Response response;
    nlm::json json;
    type::command command_type;
    type::request request_type;
    std::string uri;
    std::string error;
    bool valid_args = false;
    void make_uri();
    void parse_command_type(const std::string &command);
public:
    explicit Command(const CLI::App* command, const Nxm &cli) : command(command), cli(cli) {
        parse_command_type(command->get_name());
        make_uri();
    }
    explicit Command(const CLI::App* command, const Nxm &cli, Command* parent)
            : command(command),
              cli(cli),
              parent(parent) {
        parse_command_type(command->get_name());
        make_uri();
    }
    int sendRequest();
    const std::string& output() const { return response.text; }
    const nlm::json& getJson() const { return json; }
    const type::command& type() const { return command_type; }
    const int responseCode() const { return response.status_code; }
    const std::string& getURI() const { return uri; }
    const std::string& getError() const { return error; }
    const std::string& name() const { return command->get_name(); }
    bool good() const { return valid_args; }
};
