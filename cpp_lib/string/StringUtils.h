#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <list>
#include <sstream>
#include <memory>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <locale>
#include <functional>

namespace cppframework{
namespace str{
/**
 * @addtogroup cppframework
 * @{
 * @defgroup cppframework_str String utilities
 * @{
 */
class Util{
private:
    class IArgument{
    public:
        virtual ~IArgument(){}
        virtual std::string toString() = 0;
        virtual void        buildString(std::ostringstream& os) = 0;
    };
    template <typename T>
    class Argument : public IArgument{
    public:
        explicit Argument(T t) : IArgument(){
            m_val = t;
        }
        ~Argument(){}
        std::string toString(){
            std::ostringstream s;
            s<<m_val;
            return s.str();
        }
        void        buildString(std::ostringstream& os){
                os<<m_val;
        }

    private:
        T m_val;
    };
    template <typename T, typename... Args>
    static void extract_arg(int &deepIndex, std::vector<std::shared_ptr<IArgument>> &vargs, T t){
        vargs.push_back(std::make_shared<Argument<T>>(t));
        deepIndex++;
    }
    template <typename T, typename... Args>
    static void extract_arg(int &deepIndex, std::vector<std::shared_ptr<IArgument>> &vargs, T t, Args... args){
        vargs.push_back(std::make_shared<Argument<T>>(t));
        deepIndex++;
        if(deepIndex >= 50) return;
        extract_arg(deepIndex, vargs, args...);
    }
public:
    /**
     * @brief Trim charater 'tch' at begin and end of string
     */
    static void trim(std::string &s, char tch){
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
            [tch](char ch){
                if( ch == tch) return true;
                return false;
            }
        ));
        s.erase(std::find_if(s.rbegin(), s.rend(), 
            [tch](char ch){
                if( ch == tch) return true;
                return false;
            }
        ).base(), s.end());
    }
    /**
     * @brief Trim space at begin and end of string
     */
    static void trim(std::string& s){
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int,int>(std::isspace))));
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int,int>(std::isspace))).base(), s.end());
    }
    /**
     * @brief Split string 's' by 'delimiter' and push results to 'ret'
     * @note All results is space-trimed
     */
    static void split(std::string s, std::string delimiter, std::list<std::string>& ret){
        size_t pos = 0;
        std::string token;
        while((pos = s.find(delimiter)) != std::string::npos){
            token = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
            trim(token);
            if(token.length() > 0) ret.push_back(token);
        }
        trim(s);
        if(s.length() > 0) ret.push_back(s);
    
    }
    static void split(const char* _s, const char* _delimiter, std::vector<std::string>& ret){
        std::string s         = std::string(_s);
        std::string delimiter = std::string(_delimiter);
        split(s, delimiter, ret);
    }
    static void split(std::string& s, const char* _delimiter, std::vector<std::string>& ret){
        std::string delimiter = std::string(_delimiter);
        split(s, delimiter, ret);
    }
    static void split(std::string& s, std::string& delimiter, std::vector<std::string>& ret){
        std::list<std::string> lst;
        split(s, delimiter, lst);
        ret.insert(ret.begin(), lst.begin(), lst.end());
    }
    /**
     * @brief Build string with format and arguments
     * @code
      eg: format("val1 = {} val2 = {}", 10, "abc")
      {:format_parameters_1,format_parameters_2}
      format_parameter:
           hex             : Show number in hexa string format, eg 0x1234abcd
           upper           : Show letters in upper format, eg abcd -> ABCD
           left/right      : Set position of fill characters, eg left: |12.34   |
           base            : Show base, eg bc123 in hexa -> 0xbc12
           precision:val   : Set float precision, eg precision:5 : 0.12345678 -> 0x12345
           width:val       : Set width parameter eg: width:6 : 0.12 -> |0.12  |
     * @endcode
     */
    template <typename... Args>
    static std::string format(std::string str, Args... args){
        std::ostringstream os;
        int deepIndex = 0;
        std::list<std::string> f_css;

        std::vector<std::shared_ptr<IArgument>> vargs;
        extract_arg(deepIndex, vargs, args...);

        unsigned int argIndex = 0;
        while(argIndex < vargs.size()){
            size_t pos_begin = str.find("{");
            size_t pos_end = str.find("}");
            if((pos_begin == std::string::npos) || (pos_end == std::string::npos)) 
                break;
            
            f_css.clear();
            if(pos_end > pos_begin){
                std::string szInner = str.substr(pos_begin+1, pos_end-pos_begin-1);
                if(szInner.length() > 0){
                    if(szInner.at(0) == ':'){
                        szInner.at(0) = ' ';
                        split(szInner, ",", f_css);
                    }
                }
            }
            std::string sz = str.substr(0, pos_begin);
            os<<sz;
            
            // format
            std::ios init(0);
            init.copyfmt(os);
			os << std::fixed;
            for(auto &css : f_css){
                if(css == "hex") os<<std::hex;
                else if(css == "upper") os<<std::uppercase;
                else if(css == "left") os<<std::left;
                else if(css == "right") os<<std::right;
                else if(css == "internal") os<<std::internal;
                else if(css == "base") os<<std::showbase;
                else{
                    if(css.find("precision") != std::string::npos){
                        std::list<std::string> sub_css;
                        split(css, ":", sub_css);
                        if(sub_css.size() > 1){
                            std::istringstream s(*(++sub_css.begin()));
                            int ival = 0;
                            s >> ival;
                            if(ival >= 0 && ival < 100){
                                os<<std::setprecision(ival);
                            }
                        }
                    }else if(css.find("width") != std::string::npos){
                        std::list<std::string> sub_css;
                        split(css, ":", sub_css);
                        if(sub_css.size() > 1){
                            std::istringstream s(*(++sub_css.begin()));
                            int ival = 0;
                            s >> ival;
                            if(ival >= 0 && ival < 100){
                                os<<std::setw(ival);
                            }
                        }
                    }
                }
            }
            vargs[argIndex]->buildString(os);
            os.copyfmt(init);

            str = str.erase(0, pos_end+1);
            if(str.length() <= 0) break;
            argIndex++;
        }
        os<<str;
        return os.str();
    }
    static std::string format(std::string str){
        return str;
    }
    /**
     * @brief Convert string to T value
     */
    template <typename T>
    static T convert(std::string sz, T t_default){
        T t = t_default;
        std::istringstream s(sz);
        s >> t;
        return t;
    }
    /**
     * @brief Convert T value to string
     */
    template <typename T>
    static std::string convert(T t){
        std::ostringstream s;
        s << t;
        return s.str();
    }

    static bool isDigit(std::string str) {
        for(size_t i = 0; i < str.size(); i++) {
            if(!isdigit(str[i]))
                return false;
        }
        return true;
    }
};
/**
 * @} @}
 */

}}
#endif

