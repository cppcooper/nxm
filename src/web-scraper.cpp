#include <web-scraper.h>
#include <iostream>
#include <string>
#include <regex>
#include <cpr/cpr.h>
#include <gumbodocument.h>
#include <gumbonode.h>
#include <nlohmann/json.hpp>

namespace nlm = nlohmann;

void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to);
};

namespace globals {
    std::regex dep_pattern("(Nexus requirements|Off-site requirements)");
}

// webscraper processes args
void NxmWebScraper::process_args() {
    int status = 0;
    // Nxm can batch many mods together
    for(auto mod : cli.mods) {
        std::string required_mod = std::to_string(mod);
        if (query_mod_page(required_mod)) {
            scrape_requirements(required_mod, pages[required_mod]);
        }
    }
    // todo: build dependency tree
    for(const auto &[mod, deps] : dependencies){
        std::cout << mod << " dependencies:" << std::endl;
        for(const auto &modd : deps ){
            std::cout << "   " << modd << std::endl;
        }
    }
}

bool NxmWebScraper::query_mod_page(std::string mod) {
    if(!pages.contains(mod)){
        std::string uri = "https://www.nexusmods.com/{game_domain}/mods/{mod_id}";
        replace(uri, "{game_domain}", cli.arg1);
        replace(uri, "{mod_id}", mod);

        // todo: implement caching of web pages?
        cpr::Response r = cpr::Get(cpr::Url{uri});
        pages[mod] = r;
        return true;
    }
    return false;
}

void NxmWebScraper::scrape_requirements(const std::string &mod, const cpr::Response &r){
    if(r.status_code == 200){
        auto doc = GWDocument::parse(r.text);
        std::cout << "Scraping mod " << mod << " page.." << std::endl;
        try {
            auto parents = doc.rootNode().getElementsByClassName("accordionitems");
            if(!parents.empty()) {
                auto req_sec_parent = parents[0];
                auto req_sections = req_sec_parent.getElementsByClassName("tabbed-block");
                size_t loop_length = req_sections.size() < 2 ? req_sections.size() : 2;
                for (int j = 0; j < loop_length; ++j) {
                    std::string section_title = req_sections[j].getElementsByTagName(HtmlTag::H3)[0].innerText();
                    if (std::regex_match(section_title, globals::dep_pattern)) {
                        auto links = req_sections[j].getElementsByTagName(HtmlTag::A);
                        if (section_title == "Nexus requirements") {
                            parse_requirements(mod, links, onsite);
                        } else {
                            parse_requirements(mod, links, offsite);
                        }
                    }
                }
            } else {
                // todo: figure out how to get adult mod pages, cookies/login? and if that is the only edge case where this branch triggers
                std::cerr << "first GWNode list was empty" << std::endl;
                std::cerr << "This probably just means this is an adult mod, so we'll just skip it" << std::endl;
                exit(-42);
            }
        } catch (const std::exception &e){
            std::cerr << e.what() << std::endl;
            std::cerr << "The page format may have changed" << std::endl;
            exit(2);
        }
    } else {
        // todo: error
    }
}

//
void NxmWebScraper::parse_requirements(const std::string &mod, const std::vector<GWNode> &links, const type &req_type) {
    for (auto link: links) {
        switch (req_type) {
            case onsite: {
                /*mod as in what has a req*/
                // First we get the mod id of the requirement
                std::string required_mod = link.getAttribute("href");
                int mod_pos = required_mod.find_last_of("/") + 1;
                required_mod = std::string(std::string_view(required_mod.c_str() + mod_pos, required_mod.size() - mod_pos));
                // Then we record the relationship
                const auto &[iter,inserted] = dependencies[mod].emplace(required_mod);
                if(inserted) {
                    if (query_mod_page(required_mod)) {
                        scrape_requirements(required_mod, pages[required_mod]);
                    }
                }
                break;
            }
            case offsite:
                off_site_dependencies.emplace(mod, link.getAttribute("href"));
                break;
            case invalid:
                break;
        }
    }
}