#ifndef __DYNAMIC_LINK_LIBRARY_H__
#define __DYNAMIC_LINK_LIBRARY_H__

#include <string>
namespace cppframework{
    namespace dll{
        void* loadFile(std::string name);
        int   unload(void* handle);
        void* resolveSymbol(void* handle, std::string name);
        std::string errorToString();
    }
}

#endif
