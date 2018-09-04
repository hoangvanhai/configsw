/**
 * @addtogroup cppframework
 * @{
 * @defgroup pattern Design Pattern Template
 * @addtogroup pattern
 * @{
 *
 */ 
#ifndef __PATTERN_I_SINGLETON_H__
#define __PATTERN_I_SINGLETON_H__

#include <memory>
#include <mutex>

namespace lib {
namespace pattern {
template <typename Derived>
class singleton {
  public:
    singleton() = default;
    virtual ~singleton() = default;

    static std::shared_ptr<Derived> instance() {
        std::call_once(s_init_flag_, []() { s_inst_ = std::make_shared<Derived>(); });
        return s_inst_;
    }
    static void destroy() { s_inst_.reset(); }

  protected:
    static std::shared_ptr<Derived> s_inst_;
    static std::once_flag s_init_flag_;
};

template <typename Derived>
std::shared_ptr<Derived> singleton<Derived>::s_inst_;
template <typename Derived>
std::once_flag singleton<Derived>::s_init_flag_;

}
}

/**
 * @brief Define a singleton class
   @code
   in .h file
        PATTERN_SINGLETON(ClassName,
            // class's content here
        )
    in .cpp file
        PATTERN_SINGLETON_IMPLEMENT(ClassName)
    use
        // init instance
        std::shared_ptr<ClassName> inst = ClassName::getInstance();
        // destroy object
        ClassName::destroyInstance();
   @endcode
 */
#define PATTERN_SINGLETON(name, content) \
class name{ \
public: \
    static std::shared_ptr<name> getInstance(){\
        std::call_once(m_initFlag, [](){\
            m_inst = std::make_shared<name>();\
        });\
        return m_inst;\
    }\
    static void destroyInstance(){\
        if(m_inst) m_inst.reset();\
    }\
private:\
    static std::shared_ptr<name> m_inst;\
    static std::once_flag        m_initFlag;\
    content\
};
/**
 * @brief Implement a singleton
 */
#define PATTERN_SINGLETON_IMPLEMENT(name) \
    std::shared_ptr<name> name::m_inst;\
    std::once_flag        name::m_initFlag;

#endif
/**
 * @} @}
 */ 
