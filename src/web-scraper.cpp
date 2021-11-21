#include <command-structure.h>
#include <iostream>
#include <string>
#include <regex>
#include <cpr/cpr.h>
#include <gumbodocument.h>
#include <gumbonode.h>
#include <nlohmann/json.hpp>

namespace nlm = nlohmann;

namespace globals {
    std::regex dep_pattern("(Nexus requirements|Off-site requirements)");
}
enum type{
    onsite = 1,
    offsite = 2,
    invalid = 0xFF
};

// prototype to scrape a single mod page
long scrape_mod_page(const Nxm &cli, nlm::json &parent, const std::string &mod);
// Nxm can batch many mods together
nlm::json scrape_dependencies(const Nxm &cli) {
    int status = 0;
    nlm::json dependencies;
    for(auto mod : cli.mods) {
        dependencies["dependencies"] = {};
        status += scrape_mod_page(cli, dependencies["dependencies"], std::to_string(mod));
        if(status != 0){
            std::cerr << "web scraper has encountered some sort of problem" << std::endl;
            exit(2);
        }
    }
    return dependencies;
}

// prototype to page parser
long add_dependencies(const Nxm &cli, nlm::json &json, const GWNode &root);
// scrape a single mod page
long scrape_mod_page(const Nxm &cli, nlm::json &parent, const std::string &mod) {
    std::string resource = "https://www.nexusmods.com/{game_domain}/mods/{mod_id}";
    auto replace = [](std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return;
        str.replace(start_pos, from.length(), to);
    };
    replace(resource, "{game_domain}", cli.arg1);
    std::string uri = resource;
    replace(uri, "{mod_id}", mod);
    // todo: implement caching of web pages
    cpr::Response r = cpr::Get(cpr::Url{uri});
    if(r.status_code == 200) {
        //std::cout << r.text << std::endl;
        std::ofstream f(std::filesystem::path("/tmp/mod.html"));
        f.write(r.text.c_str(),r.text.size());
        auto doc = GWDocument::parse(r.text);
        std::cout << "Scraping mod " << mod << " page.." << std::endl;
        return add_dependencies(cli, parent["dependencies: mod " + mod], doc.rootNode());
    }
    return r.status_code;
}

// prototype to a convenience function to package requirements into a json
nlm::json package_requirements(const Nxm &cli, const std::vector<GWNode> &links, const type &req_type);
// parses the page and packages links into a json
long add_dependencies(const Nxm &cli, nlm::json &json, const GWNode &root){
    try {
        auto parents = root.getElementsByClassName("accordionitems");
        if(!parents.empty()) {
            auto req_sec_parent = parents[0];
            auto req_sections = req_sec_parent.getElementsByClassName("tabbed-block");
            size_t loop_length = req_sections.size() < 2 ? req_sections.size() : 2;
            for (int j = 0; j < loop_length; ++j) {
                std::string section_title = req_sections[j].getElementsByTagName(HtmlTag::H3)[0].innerText();
                if (std::regex_match(section_title, globals::dep_pattern)) {
                    auto links = req_sections[j].getElementsByTagName(HtmlTag::A);
                    if (section_title == "Nexus requirements") {
                        json["onsite"] = package_requirements(cli, links, onsite);
                    } else {
                        json["offsite"] = package_requirements(cli, links, offsite);
                    }
                }
            }
        } else {
            // todo: search document for adult content
            std::cerr << "first GWNode list was empty" << std::endl;
            std::cerr << "This probably just means this is an adult mod" << std::endl;
            return 2;
        }
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        std::cerr << "The page format may have changed" << std::endl;
        return 2;
    }
    return 0;
}

// packages links based on type
nlm::json package_requirements(const Nxm &cli, const std::vector<GWNode> &links, const type &req_type) {
    nlm::json package;
    std::string buffer;
    int tmp = 0;
    int loop_count = 0;
    for (auto link: links) {
        switch (req_type) {
            case onsite:
                // First we get the mod id
                buffer = link.getAttribute("href");
                tmp = buffer.find_last_of("/") + 1;
                buffer = std::string(std::string_view(buffer.c_str() + tmp, buffer.size() - tmp));
                // Now we can build our json package
                package[loop_count] = {
                        {"name",   link.innerText()},
                        {"mod_id", buffer},
                        {"href",   link.getAttribute("href")}};
                scrape_mod_page(cli, package[loop_count++], buffer);
                break;
            case offsite:
                package[loop_count++] = {
                        {"name", link.innerText()},
                        {"href", link.getAttribute("href")}};
                break;
            case invalid:
                break;
        }
    }
    return package;
}