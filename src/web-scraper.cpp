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
    // scrape dependencies for the command line mods
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
    // Check the response to our query for the mod page
    if(r.status_code == 200){
        // We'll need to parse the page as a GWDocument to keep things simple
        auto doc = GWDocument::parse(r.text);
        std::cout << "\x1b[A\r" << "Scraping mod " << mod << " page.." << std::endl;
        try {
            // First landmark
            auto parents = doc.rootNode().getElementsByClassName("accordionitems");
            if(!parents.empty()) {
                // Second landmark
                auto req_sections = parents[0].getElementsByClassName("tabbed-block");
                size_t loop_length = req_sections.size() < 2 ? req_sections.size() : 2;
                for (int j = 0; j < loop_length; ++j) {
                    // Third landmark
                    std::string section_title = req_sections[j].getElementsByTagName(HtmlTag::H3)[0].innerText();
                    if (std::regex_match(section_title, globals::dep_pattern)) {
                        // Our destination, the requirement links
                        auto links = req_sections[j].getElementsByTagName(HtmlTag::A);
                        // Parsing requirements depends on whether they are on or off site
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
                std::cerr << "This probably just means this is an adult mod, so we'll just skip it.. or you can modify the source code and make a PR" << std::endl;
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
    std::cout << "\x1b[A\r                                          " << std::endl;
}

void NxmWebScraper::parse_requirements(const std::string &mod, const std::vector<GWNode> &links, const type &req_type) {
    // iterate all the links
    for (auto link: links) {
        std::string url = link.getAttribute("href");
        // onsite and offsite are different
        switch (req_type) {
            case onsite: {
                // First we get the mod id of the requirement
                int mod_pos = url.find_last_of("/") + 1;
                url = std::string(std::string_view(url.c_str() + mod_pos, url.size() - mod_pos));
                // Then we record the relationship
                const auto &[iter, inserted] = dependencies[mod].emplace(url);
                if (inserted) {
                    // Check onsite requirements for more requirements
                    if (query_mod_page(url)) {
                        scrape_requirements(url, pages[url]);
                    }
                }
                break;
            }
            case offsite:
                off_site_dependencies.emplace(mod, url);
                break;
            case invalid:
                break;
        }
    }
}