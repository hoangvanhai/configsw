/**
 * @addtogroup cppframework
 * @{
 * @defgroup cppframework_dll Dynamic library utilities
 * @addtogroup cppframework_dll
 * @{
 */ 
#ifndef __DLL_FUNCTION_H__
#define __DLL_FUNCTION_H__

#include <cstdint>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>

#ifdef __linux__
/* linux */
#include <dlfcn.h>
/* Function in libc library */
#include <unistd.h>
#include <getopt.h>
typedef int     (*getopt_dll)(int argc, char * const argv[], const char *optstring);
typedef int     (*getopt_long_dll)(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex);
typedef int     (*open_dll)(const char *pathname, int flags, mode_t mode);
typedef int     (*close_dll)(int fd);
typedef ssize_t (*read_dll)(int fd, void *buf, size_t count);
#elif (defined(_MSC_VER) || defined(__GNUC__))
#include <Windows.h>
#endif

/**
 * @brief Find and return function pointer in shared libraries
 * @param func Function name
 * @return T   Function pointer
 * @code
    typedef void*   (*memcpy_dll)(void *dest, const void *src, size_t n);
    memcpy_dll _memcpy = dll_function<memcpy_dll>("memcpy");
    if(_memcpy){
        std::cout<<"found"<<std::endl;
        _memcpy(dest, src, count);
    }else{
        std::cout<<"not found"<<std::endl;
    }
   @endcode
 */ 
template<typename T>
T dll_function(std::string func){
    static std::string szDllNames[] = {
#if defined(__linux__)
        "libc.so.6",
#elif (defined(_MSC_VER) || defined(__GNUC__))
        "msvcrt.dll",
#endif
        "null"
    };
    class DllHandle{
    public:
        explicit DllHandle(std::string& name):
        m_dllName(name)
        {
            m_handle = 0;
        }
        ~DllHandle(){
#if defined(__linux__)
            dlclose(m_handle);
#elif (defined(_MSC_VER) || defined(__GNUC__))
            FreeLibrary(m_handle);
#endif
            m_handle = 0;
        }
        bool isOpen(){
            return (m_handle != 0);
        }
        int  openDll(){
#if defined(__linux__)
            m_handle = dlopen((const char*)m_dllName.c_str(), RTLD_LAZY);
#elif (defined(_MSC_VER) || defined(__GNUC__))
            //wchar_t tmpb[512];
            //MultiByteToWideChar(CP_ACP, 0, m_dllName.c_str(), -1, tmpb, 512);
//             m_handle = LoadLibrary(m_dllName.c_str());
            //#error Uncomment here for implemented
#endif
            if(!m_handle){
                std::cout<<"open library "<<m_dllName<<" failed"<<std::endl;
                return -1;
            }
            return 0;
        }
        T find(std::string& functionName){
            if(m_handle){
#if defined(__linux__)
                T fxn = (T)dlsym(m_handle, functionName.c_str());
#elif (defined(_MSC_VER) || defined(__GNUC__))
                T fxn = (T)GetProcAddress(m_handle, functionName.c_str());
#endif
                return fxn;
            }
            return 0;
        }
        std::string name(){ return m_dllName; }
    private:
#if defined(__linux__)
        void* m_handle;
#elif (defined(_MSC_VER) || defined(__GNUC__))
        HINSTANCE m_handle;
#endif
        std::string m_dllName;
    };
    static std::vector<std::shared_ptr<DllHandle>> lstHandle;
    static std::once_flag       onceFlag;
    static std::recursive_mutex lock;
	
	std::lock_guard<std::recursive_mutex> lockObj(lock);
	
    std::call_once(onceFlag, [&]{
        int i = 0;
        while(szDllNames[i] != "null"){
            std::shared_ptr<DllHandle> dll = std::make_shared<DllHandle>(szDllNames[i]);
            lstHandle.push_back(dll);
            i++;
        }
    });
    
    T foundFxn = 0;
    for(auto dll : lstHandle){
        if(!dll->isOpen()){
            dll->openDll();
        }
        T fxn = dll->find(func);
        if(fxn){
            foundFxn = fxn;
            break;
        }
    }
    return foundFxn;
}
#endif
/**
 * @} @}
 */ 
