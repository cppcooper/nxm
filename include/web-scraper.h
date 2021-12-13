#pragma once
#include <unordered_map>
#include <unordered_set>
#include <command-structure.h>
#include <gumbonode.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
namespace nlm = nlohmann;

class NxmWebScraper {
private:
    enum type{
        onsite = 1,
        offsite = 2,
        invalid = 0xFF
    };
    using mod_id = std::string;
    using url = std::string;
    std::unordered_map<mod_id, std::unordered_set<mod_id>> dependencies;
    std::unordered_multimap<mod_id, url> off_site_dependencies;
    std::unordered_map<mod_id, cpr::Response> pages;
    const Nxm &cli;
public:
    NxmWebScraper(const Nxm &cli) : cli(cli) {}
    void process_args();

protected:
    bool query_mod_page(std::string mod);
    void scrape_dependencies();
    void scrape_requirements(const std::string &mod, const cpr::Response &r);
    void parse_requirements(const std::string &mod, const std::vector<GWNode> &links, const type &req_type);
};