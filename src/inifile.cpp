#include "inifile.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define  SECTION_KEY_CAT_SYMBOL "$$"

namespace sc {

inifile::inifile() 
{
    memset(errmsg_, 0x0, sizeof(errmsg_));
}


inifile::~inifile()
{
}

bool inifile::load(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "r"); 
    if(nullptr == fp)
    {
        sprintf(errmsg_, "load inifile(%s) failed: %s", filename.c_str(), strerror(errno));
        return true;
    }

    std::string line;
    std::string section;
    std::string key;
    std::string value;
    char tmp[4096]   = {0};
    while(nullptr != fgets(tmp, sizeof(tmp), fp))
    {	
        line = std::string(tmp);
        trim(line);
        if(is_empty_line(line) || is_comment_line(line))
        {
            continue;
        }

        if(is_section_start(line))
        {
            get_section(line.c_str(), section);
        }
        else
        {
            if(parse_line(line, key, value) < 0)
            {
                continue;
            }
            trim(key);
            trim(value);
            key += SECTION_KEY_CAT_SYMBOL + section;  
            kvmap_[key] = value;
        }
    }

    fclose(fp);
    filename_ = filename;

    return false;
}

bool inifile::reload(const std::string& filename)
{
    kvmap_.clear(); 

    if(filename == "")
        return load(filename_);
    else
        return load(filename);

    return false;
}


bool inifile::is_comment_line(const std::string& str) const
{
    return str[0] == '#';
}

bool inifile::is_empty_line(const std::string& str) const
{
    return str.length() == 0;
}

bool inifile::is_section_start(const std::string& str) const
{
    return str[0] == '[' ;
}

int inifile::get_section(const char* line, std::string& section)
{
   if(nullptr == line || line[0] != '[')
        return -1;

    const char* ptr = strchr(line, ']');
    if(nullptr == ptr)
        return -2;

	section = std::string(line, 1, (ptr-(line+1)));

    trim(section);

    return 0; 
}

int inifile::parse_line(std::string& line, std::string& key, std::string& value)
{
    std::size_t pos =0;
    int len = line.length();

    if(len <= 0) return -1;

    pos = line.find('#');
    if(pos != std::string::npos)
    {
        line.erase(pos);
    }

    pos = line.find('=');
    key = line.substr(0, pos);
    value = line.substr(pos+1);

	trim(key);
	trim(value);

	return 0;
}

void inifile::trim(std::string& str)
{
    int i = 0;

	//trim head
    int len = str.length();
    while (isspace(str[i]) && str[i] != '\0') {
        i++;
    }

    if (i != 0) {
        str = std::string(str, i, len - i);
    }

    //trim tail
    len = str.length();
    for (i = len - 1; i >= 0; --i) {
        if (!isspace(str[i])) {
            break;
        }
    }

    str = std::string(str, 0, i + 1);
}

void inifile::dump(void) const
{
    fprintf(stdout, "total items: %lu\n", kvmap_.size());
    for(auto& x : kvmap_)
    {
        fprintf(stdout, "[%s] : [%s]\n", x.first.c_str(), x.second.c_str());
    }
}

}

#if 0
int main(int argc, char* argv[])
{
    const std::string F =  {"./test.ini"};   
    
    sc::inifile ini;

    ini.load(F);

    ini.dump();
    
    return 0;
}
#endif

