#pragma once
#include <CLI/CLI.hpp>

struct Nxm {
    CLI::App nxm{"NXM CLI\n"
                 "This app provides a cli interface to Nexus Mods using rest api and web scraping.\n"
                 "Using subcommands it is wielded much like git, if you're familiar.\n\n"
                 "With `<subcommand> --help` you can view help for each subcommand, even nested.\n", "nxm"};
    CLI::App* download = nxm.add_subcommand("download");
    CLI::App* track = nxm.add_subcommand("track");
    CLI::App* untrack = nxm.add_subcommand("untrack");
    CLI::App* endorse = nxm.add_subcommand("endorse");
    CLI::App* abstain = nxm.add_subcommand("abstain");
    CLI::App* list = nxm.add_subcommand("list");
    CLI::App* list_games = list->add_subcommand("games", "all Nexus games");
    CLI::App* list_tracked = list->add_subcommand("tracked", "all tracked mods for a game");
    CLI::App* list_endorsed = list->add_subcommand("endorsed", "all endorsed mods for a game");
    CLI::App* list_trending = list->add_subcommand("trending", "top 10 trending mods for a game");
//    CLI::App* list = nxm.add_subcommand("list");
//    CLI::App* list_games = list->add_subcommand("games", "all Nexus games");
//    CLI::App* list_tracked = list->add_subcommand("tracked", "all tracked mods for a game");

    std::string arg1;
    std::string arg2;
    std::string arg3;
    //std::string arg4;
    bool print = false;
    Nxm(){
        auto add_game_option = [&](CLI::App* cli) {
            cli->add_option("game",arg1, "game domain, for a listing run `nxm list games`");
        };
        auto add_mod_option = [&](CLI::App* cli){
            add_game_option(cli);
            cli->add_option("mod",arg2, "mod id");
        };
        add_mod_option(download);
        download->add_option("path", arg3, "relative or absolute path");
        download->add_flag("-p,--print",print,"print a download link, instead of downloading");
        add_mod_option(track);
        add_mod_option(untrack);
        add_mod_option(endorse);
        add_mod_option(abstain);

        add_game_option(list_endorsed);
        add_game_option(list_tracked);
        add_game_option(list_trending);
    }
};