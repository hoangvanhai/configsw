#include "Console.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>


using namespace cppframework::cli;

/* Internal class */
#if defined(__linux__)
#include <sys/select.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
namespace cppframework{
namespace cli{
    class Console_Internal{
    public:
        static int read_data_from_stdin(void* data, int dataSize, int timeout){
            fd_set fd;
            struct timeval tv;
            int ret = -1;

            FD_ZERO(&fd);
            FD_SET(0, &fd);

            tv.tv_sec = timeout / 1000;
            tv.tv_usec= (timeout % 1000) * 1000;

			do{
				ret = select(0 + 1, &fd, 0, 0, &tv);
			}
			#if defined(__linux__)
			while((ret < 0) && (errno == EINTR));
			#elif defined(_WIN32)
			while(0);
			#endif
            if(ret > 0){
                if(FD_ISSET(0, &fd)){
#if defined(__ANDROID__)
                    ret = 0;
#else
                    ret = syscall(SYS_read, 0, data, dataSize);
#endif
                }
            }
            return ret;
        }
        static void set_stdin_echo(bool on){
            struct termios st;
            tcgetattr(STDIN_FILENO, &st);
            st.c_lflag = on ? (st.c_lflag | (ECHO | ICANON) ) : (st.c_lflag & ~(ECHO | ICANON));
            tcsetattr(STDIN_FILENO, TCSANOW, &st);
        }
    };
}}
#elif defined(_MSC_VER)
#include <windows.h>
namespace cppframework{
namespace cli{
    class Console_Internal{
    public:
        static int read_data_from_stdin(void* data, int dataSize, int timeout){
            int ret = 0;
            HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
            DWORD event = WaitForSingleObject(handle, timeout);
            switch (event){
            case WAIT_OBJECT_0:{
                INPUT_RECORD record[512];
                DWORD num;
                int rlen = dataSize;
                if (dataSize > 512) rlen = 512;
                if (!ReadConsoleInput(handle, record, rlen, &num)){
                    break;
                }
                char* pdata = (char*)data;
                for (DWORD i = 0; i < num; i++){
                    if (record[i].EventType != KEY_EVENT){
                        break;
                    }
                    if (record[i].Event.KeyEvent.bKeyDown){
                        break;
                    }
                    char val = record[i].Event.KeyEvent.uChar.AsciiChar;
                    *pdata = val;
                    if (m_echo){
                        std::cout << val;
                        if (val == 13 || val == 10) std::cout << std::endl;
                    }
                    ret ++;
                    pdata++;
                }
                break;
            }
            case WAIT_TIMEOUT:{
                break;
            }
            default:
                break;
            }
            return ret;
        }
        static void set_stdin_echo(bool on){
            DWORD mode;
            HANDLE hdl = GetStdHandle(STD_INPUT_HANDLE);
            GetConsoleMode(hdl, &mode);
            mode = on ? (mode | ENABLE_ECHO_INPUT) : (mode & ~(ENABLE_ECHO_INPUT));
            SetConsoleMode(hdl, mode);
            m_echo = on;
        }
    private:
        static bool m_echo;
    };
    bool Console_Internal::m_echo = false;
}}
#else
#error Platform is not supported
#endif
/*----------------*/

std::shared_ptr<Console> Console::m_inst;
std::once_flag Console::m_instFlag;

Console::Console():
m_bufPtr(std::shared_ptr<uint8_t>(new uint8_t[m_bufSize], [](uint8_t* p){delete[] p;}))
{
    m_bufSize = 4096;
    m_bufLen  = 0;
    m_inputDelimiter.push_back((char)10);
    //m_inputDelimiter.push_back((char)13);
}
Console::~Console(){
}

std::shared_ptr<Console> Console::getInstance(){
    std::call_once(m_instFlag, [&]{
        m_inst = std::make_shared<Console>();
    });
    return m_inst;
}
void Console::destroyInstance(){
    m_inst.reset();
}
int Console::setInputMaxLength(int val){
    int err = -1;
    
    std::shared_ptr<Console> inst = getInstance();
    if(!inst) return err;
    
    if(val > 0 && val <= 4096){
        if(val != inst->m_bufSize){
            inst->m_bufSize = val;
            inst->m_bufPtr  = std::shared_ptr<uint8_t>(new uint8_t[inst->m_bufSize], [](uint8_t* p){delete[] p;});
        }
        err = 0;
    }
    return err;
}
int Console::addInputDelimiter(std::string val){
    int err = -1;
    
    std::shared_ptr<Console> inst = getInstance();
    if(!inst) return err;
    
    for(auto s : val){
        bool existed = false;
        for(auto d : inst->m_inputDelimiter){
            if(s == d){
                existed = true;
                break;
            }
        }
        if(!existed)
            inst->m_inputDelimiter.push_back(s);
    }
    err = 0;
    return err;
}
void Console::flush(){
    fflush(stdout);
    // fflush(stdin);
    fflush(stderr);
}
std::string Console::getline(bool passwdMode){
    int len = 0;
    std::ostringstream str;

    std::shared_ptr<Console> inst = getInstance();
    if(!inst) return "";

    uint8_t *buf = inst->m_bufPtr.get();
    if(inst->m_bufLen > 0){
        int i;
        bool hasData = false;
        for(i = 0; i < inst->m_bufLen; i ++){
            for(auto s : inst->m_inputDelimiter){
                if(buf[i] == s){
                    buf[i] = 0;
                    i++;
                    hasData = true;
                    break;
                }
            }
            if(hasData) break;
        }
        str<<buf;
        // remove delimiter
        for(;i < inst->m_bufLen; i ++){
            hasData = false;
            for(auto s : inst->m_inputDelimiter){
                if(buf[i] == s){
                    hasData = true;
                    break;
                }
            }
            if(!hasData) break;
        }
        
        int j = 0;
        for(; i < inst->m_bufLen; i++){
            buf[j++] = buf[i];
        }
        if(j > 0 || hasData){
            inst->m_bufLen = j;
            buf[inst->m_bufLen] = 0;
            return str.str();
        }
    }
    
    if(passwdMode) Console_Internal::set_stdin_echo(false);

    while(1){
        int ret = Console_Internal::read_data_from_stdin(buf, inst->m_bufSize, 100);
        if(ret > 0 && ret < inst->m_bufSize){
            buf[ret] = 0;
            int newLineIndex = 0;
            bool hasData = false;
            for(; newLineIndex < ret; newLineIndex++){
                for(auto s : inst->m_inputDelimiter){
                    if(buf[newLineIndex] == s){
                        buf[newLineIndex] = 0;
                        newLineIndex++;
                        hasData = true;
                        break;
                    }
                }
                if(hasData) break;
            }
            str<<buf;
            
            for(; newLineIndex < ret; newLineIndex++){
                bool isIgnore = false;
                for(auto s : inst->m_inputDelimiter){
                    if(buf[newLineIndex] == s){
                        isIgnore = true;
                        break;
                    }
                }
                if(!isIgnore) break;
            }
            int i = 0;
            for(; newLineIndex < ret; newLineIndex++){
                buf[i] = buf[newLineIndex];
                i++;
            }
            inst->m_bufLen = i;
            buf[inst->m_bufLen] = 0;
            
            len += ret;
            if(len >= inst->m_bufSize) break;
            if(i > 0) break;
            if(hasData) break;
        }else if(ret < 0){
            inst->m_bufLen = 0;
            break;
        }
    }
    if(passwdMode) Console_Internal::set_stdin_echo(true);
    
    return str.str();
}
