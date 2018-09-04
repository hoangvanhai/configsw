#include "INI.h"

#include <fstream>
#include <algorithm>
#include <list>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include "../string/StringUtils.h"

using namespace cppframework;

namespace cppframework{namespace ini{

Section::Section(){
    
}
Section::~Section(){
    
}
const std::string& Section::Name() const{
    return m_name;
}
const std::string Section::Value(const std::string &nameOfValue) const {
    for(auto &s : m_values){
        if(s.key == nameOfValue){
            return s.value;
        }
    }
    return std::string();
}

int Section::SetValue(const std::string &nameOfValue, const std::string &val) {
    for(auto &s : m_values){
        if(s.key == nameOfValue){
            s.value = val;
            return 0;
        }
    }
    return -1;
}

const std::shared_ptr<Section> Section::FindSection(const std::string &nameOfSection) const {
    std::shared_ptr<Section> ret;
    
    std::list<std::string> lst;
    cppframework::str::Util::split(nameOfSection, "/", lst);
    
    if(lst.size() == 0) return ret;
    std::list<std::string>::const_iterator iter = lst.begin();
    std::string currentName = *iter;
    std::string nextName = "";
    ++iter;
    while(iter != lst.end()){
        nextName += ("/" + *iter);
        ++iter;
    }
    for(auto &s : m_sections){
        if(s->Name() == currentName){
            ret = s;
            break;
        }
    }
    if(nextName.length() > 0 && ret){
        ret = ret->FindSection(nextName);
    }
    return ret;
}
int Section::ValueCount() const{
    return (int)m_values.size();
}
int Section::SectionCount() const {
    return (int)m_sections.size();
}
void Section::Clear(){
    m_values.clear();
    m_sections.clear();
}
int  Section::SetName(const std::string &nameOfSection){
    m_name = nameOfSection;
    return 0;
}
int  Section::Insert(const std::string &nameOfValue, const std::string &value) {
    bool existed = false;
    
    for(auto &s : m_values){
        if(s.key == nameOfValue){
            s.value = value;
            existed = true;
            break;
        }
    }
    if(!existed){
        struct KEYVALUE kv;
        kv.key = nameOfValue;
        kv.value = value;
        m_values.push_back(kv);
    }
    return 0;
}
int  Section::Insert(const std::string &nameOfSection, const std::shared_ptr<Section> &section) {
    
    if(section == nullptr) return -1;
    
    std::list<std::shared_ptr<Section>>::iterator iter = m_sections.begin();
    
    while(iter != m_sections.end()){
        if((*iter)->Name() == nameOfSection){
            m_sections.erase(iter);
            break;
        }
        ++iter;
    }
    m_sections.push_back(section);
    section->SetName(nameOfSection);
    return 0;
}
int Section::IterateValue  (const std::function<int(const std::string &key, const std::string &value)>& callback) const {
    int err = 0;
    for(auto &s : m_values){
        err = callback(s.key, s.value);
        if(err){ 
            break;
        }
    }
    return err;
}
int Section::IterateSection(const std::function<int(const std::shared_ptr<Section>& section)>& callback) const {
    int err = 0;
    for(auto &s : m_sections){
        err = callback(s);
        if(err){ 
            break;
        }
    }
    return err;
}

int Writer::_save(const std::string& root, std::ofstream& file, const std::shared_ptr<Section> &section, int deepLevel){    
    std::string path;
    if(root.length() == 0){
        path = section->Name();;
    }else{
        path = root+"/"+section->Name();
    }
    for(int i = 0; i < deepLevel*2; i++)
        file<<"  ";
    file<<"["<<path<<"]"<<std::endl;
    section->IterateValue([&](std::string nameOfValue, std::string value)->int{
        for(int i = 0; i < deepLevel*2+2; i++)
            file<<"  ";
        file<<nameOfValue<<"="<<value<<std::endl;
        return 0;
    });
    section->IterateSection([&](std::shared_ptr<Section> _section)->int{
        _save(path, file, _section, deepLevel + 1);
        return 0;
    });
    return 0;
}
int Writer::Save(const std::string& filePath, const std::shared_ptr<Section>& section, bool append){
    if(!section) return -2;
    
    std::ofstream file(filePath, append ? std::ofstream::app : std::ofstream::out);
    if(file.bad()) return -3;
    
    section->IterateSection([&](std::shared_ptr<Section> _section)->int{
        _save("", file, _section, 0);
        return 0;
    });
    return 0;
}

std::shared_ptr<Section> Reader::Load(const std::string &filePath){
    std::ifstream file(filePath, std::ifstream::in);
    
    if(!file.good()) return nullptr;

    std::shared_ptr<Section> root = std::make_shared<Section>();
    root->SetName("root");    
    root->Clear();
    std::string line;
    
    std::shared_ptr<Section> currentSection = root;
    std::list<std::string> lstString;
    
    while(std::getline(file, line)){
        // std::cout<<line<<std::endl;
        // trim
        _trim_comment(line);
        cppframework::str::Util::trim(line);
        // ignore empty line
        if(line.length() == 0) continue;
        
        if((line.at(0) == '[') && (line.at(line.length()-1) == ']')){
            // new sections
            // split by '/'
            line.at(0) = ' ';
            line.at(line.length()-1) = ' ';
            cppframework::str::Util::trim(line);
            
            lstString.clear();
            cppframework::str::Util::split(line, "/", lstString);
            currentSection = root;
            for(auto &s : lstString){
                if(currentSection->FindSection(s) == nullptr){
                    std::shared_ptr<Section> newSection = std::make_shared<Section>();
                    newSection->SetName(s);
                    currentSection->Insert(s, newSection);
                    currentSection = newSection;
                }else{
                    currentSection = currentSection->FindSection(s);
                }
            }
        }else{
            if(currentSection == nullptr) continue;
            lstString.clear();
            cppframework::str::Util::split(line, "=", lstString);
            std::list<std::string>::const_iterator iter = lstString.begin();
            if(lstString.size() > 0){
                std::string key = *iter;
                std::string value;
                if(lstString.size() > 1){
                    ++iter;
                    value = *iter;
                }
                currentSection->Insert(key, value);
            }
        }
    }
    
    return root;
}
void Reader::_trim_comment(std::string& s){
    s.erase(std::find_if(s.begin(), s.end(), 
        [](char ch){
        if( ch == ';') return true;
        return false;
    }), s.end());
}

}}
// end of file
