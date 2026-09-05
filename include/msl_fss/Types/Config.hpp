#ifndef MSL_FSS_CONFIG_HPP
#define MSL_FSS_CONFIG_HPP

#include <map>
#include <string>

namespace msl_fss {

// Dummy representation of a simulation configuration. In the real product the
// configuration is a Lua file read through the MSL infraestrutura
// (LuaScript + MFEMMeshReaders + MFEMBoundaryCondition). Here parameters are
// stored in a simple key/value map so the module structure can be validated
// without the Lua dependency.
class Config {
public:
    // Returns the value mapped to key, or def when the key is absent.
    double getDouble(const std::string& key, double def = 0.0) const;
    int getInt(const std::string& key, int def = 0) const;
    std::string getString(const std::string& key, const std::string& def = "") const;

    void set(const std::string& key, const std::string& value);
    bool has(const std::string& key) const;

    // Reads a trivial "key = value" text file (one per line, '#' comments).
    static Config fromFile(const std::string& path);

private:
    std::map<std::string, std::string> values_;
};

} // namespace msl_fss

#endif // MSL_FSS_CONFIG_HPP
