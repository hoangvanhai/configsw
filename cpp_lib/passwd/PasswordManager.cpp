#include "PasswordManager.h"

#include "../hash/Base64.h"
#include "../hash/MD5.h"
#include "../string/StringUtils.h"
#include "../rand/Random.h"

#include <fstream>
#include <algorithm>
#include <cstring>

using namespace cppframework::passwd;

Database::Database(){
}
Database::~Database(){
    
}
int Database::setDatabasePath(std::string dbPath){
    m_dbPath = dbPath;
    return 0;
}
int Database::load(){
    int err = 0;
    
    std::ifstream file(m_dbPath);
    std::string line;
    
    if(file.bad()) return err;    
    while(std::getline(file, line)){
        str::Util::trim(line);
        if(line.length() <= 0) continue;
        
        std::list<std::string> lst;
        str::Util::split(line, ":", lst);
        if(lst.size() >= 2){
            std::list<std::string>::const_iterator iter = lst.begin();
            std::string username = *iter; ++iter;
            std::string passwd   = *iter;
            str::Util::trim(username);
            str::Util::trim(passwd);
            if(username.length() > 0 && passwd.length() > 0){
                lst.clear();
                str::Util::split(passwd, "$", lst);
                if(lst.size() >= 3){
                    iter = lst.begin();
                    std::string hash = *iter; ++iter;
                    std::string salt = *iter; ++iter;
                    std::string pw   = *iter;
                    
                    str::Util::trim(hash);
                    str::Util::trim(salt);
                    str::Util::trim(pw);
                    
                    if(hash.length() > 0){
                        struct Item item;
                        item.hash = hash;
                        item.salt = salt;
                        item.passwd = pw;
                        m_items.insert(std::pair<std::string, struct Item>(username, item));
                        err++;
                    }
                } 
            }
        }
    }
    file.close();    
    return err;
}
int Database::save(){
    int err = -1;
    
    std::fstream file(m_dbPath, std::ios::out);
    
    if(file.bad()) return err;
    
    for(auto &s : m_items){
        std::string username = s.first;
        struct Item item = s.second;
        file<<username;
        file<<":"<<"$"<<item.hash<<"$"<<item.salt<<"$"<<item.passwd<<std::endl;
    }
    
    file.close();
    err = 0;
    
    return err;
}
int Database::add(std::string username, std::string passwd){
    int err = -1;
    
    std::map<std::string, struct Item>::iterator iter = m_items.find(username);
    if(iter != m_items.end()){
        err = change(username, passwd);
    }else{
        uint8_t salt[16];
        uint8_t hash[16];
        for(int i = 0; i < 16; i++) salt[i] = cppframework::rand::Random::generate(0, 255);
        hash::MD5* md5 = new hash::MD5();
        md5->append(salt, 16);
        md5->append((const uint8_t*)passwd.c_str(), passwd.length());
        md5->finish(hash);
        delete md5;
        
        struct Item item;
        item.hash = "1";    // Fixed to MD5
        item.salt = hash::Base64::encode(salt, 16);
        item.passwd = hash::Base64::encode(hash, 16);
        
        m_items.insert(std::pair<std::string, struct Item>(username, item));
        err = 0;
    }    
    return err;
}
int Database::change(std::string username, std::string passwd){
    int err = -1;
    
    std::map<std::string, struct Item>::iterator iter = m_items.find(username);
    if(iter == m_items.end()) return err;
    
    uint8_t salt[16];
    uint8_t hash[16];
    for(int i = 0; i < 16; i++) salt[i] = cppframework::rand::Random::generate(0, 255);
    hash::MD5* md5 = new hash::MD5();
    md5->append(salt, 16);
    md5->append((const uint8_t*)passwd.c_str(), passwd.length());
    md5->finish(hash);
    delete md5;
    
    iter->second.salt   = hash::Base64::encode(salt, 16);
    iter->second.passwd = hash::Base64::encode(hash, 16);
    
    err = 0;
    
    return err;
}
int Database::remove(std::string username){
    int err = -1;
    
    std::map<std::string, struct Item>::iterator iter = m_items.find(username);
    if(iter == m_items.end()) return err;
    
    m_items.erase(iter);
    
    err = 0;
    
    return err;
}
int Database::check(std::string username, std::string passwd){
    int err = -1;
    
    std::map<std::string, struct Item>::iterator iter = m_items.find(username);
    if(iter == m_items.end()) return err;
    
    uint8_t salt[16];
    uint8_t hash[16];
    
    memset(salt, 0, 16);
    memset(hash, 0, 16);
    
    hash::Base64::decode(iter->second.salt,   salt, 16);
    
    hash::MD5* md5 = new hash::MD5();
    md5->append(salt, 16);
    md5->append((const uint8_t*)passwd.c_str(), passwd.length());
    md5->finish(hash);
    delete md5;
    
    std::string sz = hash::Base64::encode(hash, 16);
    
    if(sz == iter->second.passwd) err = 0;
    
    return err;
}
// end of file
