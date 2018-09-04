#ifndef __PASSWD_MANAGER_H__
#define __PASSWD_MANAGER_H__

#include <string>
#include <map>

namespace cppframework{
namespace passwd{
class Database{
public:
    Database();
    virtual ~Database();
    int setDatabasePath(std::string dbPath);
    int load();
    int save();
    
    int add(std::string username, std::string passwd);
    int change(std::string username, std::string passwd);
    int remove(std::string username);
    int check(std::string username, std::string passwd);
private:
    std::string m_dbPath;
    struct Item{
        std::string passwd;
        std::string salt;
        std::string hash;
    };
    std::map<std::string, struct Item> m_items;
};
}}

#endif
