#include <command-structure.h>
#include <iostream>
#include <string>
#include <regex>
#include <cpr/cpr.h>
#include <gumbodocument.h>
#include <gumbonode.h>

namespace globals {
    std::regex dep_pattern("(Nexus requirements|Off-site requirements)");
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

enum step {
    one = 1,
    two,
    three
};
enum type{
    onsite = 1,
    offsite = 2,
    invalid = 0xFF
};

int search_dependencies(step i, const GWNode &root, type k = invalid){
    step next = (step)(i + 1);
    std::vector<GWNode> nodes;
    std::string buffer;
    int tmp = 0;
    int status = 0;
    switch(i) {
        case one:
            nodes = root.getElementsByClassName("accordionitems");
            if (!nodes.empty()) {
                return search_dependencies(next, nodes[0]);
            }
            std::cout << "None" << std::endl;
            return 0;
        case two:
            nodes = root.getElementsByClassName("tabbed-block");
            for (int j = 0; j < (2 > nodes.size() ? 1 : 2); ++j) {
                if (!nodes[j].getElementsByTagName(HtmlTag::H3).empty()) {
                    buffer = nodes[j].getElementsByTagName(HtmlTag::H3)[0].innerText();
                    if (std::regex_match(buffer, globals::dep_pattern)) {
                        if(buffer == "Nexus requirements"){
                            status += search_dependencies(next, nodes[j], onsite);
                        } else {
                            status += search_dependencies(next, nodes[j], offsite);
                        }
                        continue;
                    } else if (j == 0) {
                        std::cout << " None" << std::endl;
                        return 0;
                    }
                } else {
                    std::cerr << "Error: HTML parsing: couldn't find h3 tag under tabbed-block" << std::endl;
                    return 2;
                }
            }
            return status;
        case three:
            switch(k) {
                case onsite:
                    std::cout << "nxm dependencies" << std::endl;
                    break;
                case offsite:
                    std::cout << "offsite dependencies" << std::endl;
                    break;
                default:
                    std::cerr << "An unknown error has occurred" << std::endl;
                    return 2;
            }
            nodes = root.getElementsByTagName(HtmlTag::A);
            for (auto link: nodes) {
                switch (k) {
                    case onsite:
                        buffer = link.getAttribute("href");
                        tmp = buffer.find_last_of("/") + 1;
                        buffer = std::string(std::string_view(buffer.c_str() + tmp, buffer.size() - tmp));
                        printf(" %-10s %-s70 %-40s\n", buffer.c_str(), link.getAttribute("href").c_str(), link.innerText().c_str());
                        break;
                    case offsite:
                        std::cout << " " << link.innerText() << " " << link.getAttribute("href") << std::endl;
                        break;
                }
            }
            return 0;
    }
    return -(int)i;
}

int web_scraper(const Nxm &cli) {
    std::string uri = "https://www.nexusmods.com/{game_domain}/mods/{mod_id}";
    replace(uri, "{game_domain}", cli.arg1);
    replace(uri, "{mod_id}", cli.arg2);
    // todo: implement caching
    cpr::Response r = cpr::Get(cpr::Url{uri});
    if(r.status_code == 200) {
        auto doc = GWDocument::parse(r.text);
        std::cout << "Dependencies for mod " << cli.arg2 << std::endl;
        search_dependencies(step::one, doc.rootNode());
        return 0;
    }
    return r.status_code;
}