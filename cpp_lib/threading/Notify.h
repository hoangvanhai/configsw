#ifndef __helper_NOTIFY_H__
#define __helper_NOTIFY_H__

#include <mutex>
#include <condition_variable>

namespace cppframework{
    namespace threading{
        /**
        * @addtogroup cppframework
        * @{
        * @defgroup cppframework_threading Thread utilities
        * @{
        */
        /**
         * @brief Pend and Post a event in multi-threading application
         */
        class Notify{
        public:
            Notify(){
                m_triggerLock = std::unique_lock<std::mutex>(m_triggerMutex);
            }
            virtual ~Notify(){}
            /**
             * @brief Wait event in miliseconds
             * @note in Microsoft C++: must call 'wait_for' in create thread, if diffrent thread, this call will throw a exception
             */
            std::cv_status wait_for(int miliseconds){
                std::cv_status ret = m_trigger.wait_for(m_triggerLock, std::chrono::milliseconds(miliseconds));
                return ret;
            }
            /**
             * @brief Post event to all waiting call
             */ 
            void notify_all(){
                m_trigger.notify_all();
            }
        private:
            std::mutex                      m_triggerMutex;
            std::unique_lock<std::mutex>    m_triggerLock;
            std::condition_variable         m_trigger;
        };
        /**
        * @}
        * @}
        */
    }
}

#endif
