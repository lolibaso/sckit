#ifndef __sc_inifile_h
#define __sc_inifile_h

#include <string>
#include <unordered_map>

namespace sc {

typedef std::unordered_map<std::string, std::string> KVMap;

class inifile
{
public:
    inifile();
    ~inifile();

public:
    bool        load(const std::string&  filename);
    bool        reload(const std::string& filename = "");

public:
    std::string get_value(const std::string& section, const std::string& key, const std::string& def_value = "");
    int         get_value_int(const std::string& section, const std::string& key, const int& def_value = 0);
    double      get_value_double(const std::string& section, const std::string& key, const double& def_value = 0.0);
    bool        get_value_bool(const std::string& section, const std::string& key, const bool& def_value = false);
    void        dump(void) const;

private:
    static void trim(std::string& str);
    int         parse_line(std::string& line, std::string& key, std::string& value);
    int         get_section(const char* line, std::string& section);
    bool        is_section_start(const std::string& str) const;
    bool        is_empty_line(const std::string& str) const;
    bool        is_comment_line(const std::string& str) const;

private:
    std::string  filename_;
    KVMap        kvmap_;
    char         errmsg_[512];
};

}

#endif
