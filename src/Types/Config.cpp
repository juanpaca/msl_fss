#include "msl_fss/Types/Config.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace msl_fss {

double Config::getDouble(const std::string& key, double def) const {
    auto it = values_.find(key);
    if (it == values_.end()) return def;
    return std::strtod(it->second.c_str(), nullptr);
}

int Config::getInt(const std::string& key, int def) const {
    auto it = values_.find(key);
    if (it == values_.end()) return def;
    return std::atoi(it->second.c_str());
}

std::string Config::getString(const std::string& key,
                              const std::string& def) const {
    auto it = values_.find(key);
    if (it == values_.end()) return def;
    return it->second;
}

bool Config::has(const std::string& key) const {
    return values_.count(key) > 0;
}

void Config::set(const std::string& key, const std::string& value) {
    values_[key] = value;
}

Config Config::fromFile(const std::string& path) {
    Config cfg;
    std::ifstream in(path);
    if (!in) return cfg;  // empty config on missing file (dummy behavior)

    std::string line;
    while (std::getline(in, line)) {
        // strip inline comment
        auto hash = line.find('#');
        if (hash != std::string::npos) line = line.substr(0, hash);
        // parse key = value
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        auto trim = [](std::string& s) {
            const auto b = s.find_first_not_of(" \t");
            const auto e = s.find_last_not_of(" \t");
            s = (b == std::string::npos) ? "" : s.substr(b, e - b + 1);
        };
        trim(key);
        trim(value);
        if (!key.empty()) cfg.set(key, value);
    }
    return cfg;
}

} // namespace msl_fss