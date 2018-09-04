#include "DynamicLinkLibrary.h"

#ifdef __linux__
#include <dlfcn.h>

void* cppframework::dll::resolveSymbol(void* handle, std::string name){
    if(!handle) return 0;
    
    return dlsym(handle, name.c_str());
}
void* cppframework::dll::loadFile(std::string name){
    if(name .length() <= 0)
        return dlopen(0, RTLD_NOW);
    else
        return dlopen(name.c_str(), RTLD_NOW | RTLD_NODELETE);
}
int   cppframework::dll::unload(void* handle){
    if(handle){
        dlclose(handle);
    }
    return 0;
}
std::string cppframework::dll::errorToString(){
    char* sz = dlerror();
    if(sz){
        return std::string(sz);
    }
    return "";
}
#endif

// end of file
