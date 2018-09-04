
#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__
#include <list>
#include <vector>
#include <string>

namespace cppframework{
namespace cli{
    /**
    * @addtogroup cppframework
    * @{
    * @defgroup cppframework_cli Command Line Interface
    * @{
    * @addtogroup cppframework_cli
    * @{
    */
    /**
     * @brief Print progressbars in terminal
     */ 
    class ProgressBar{
    public:
        /**
         * @brief Constructor
         * @param count Number of progressbar are created
         */ 
        explicit ProgressBar(int count);
        virtual ~ProgressBar();
        /**
         * @brief Set progressbar label
         * @param labels Labels
         * @retval 0 Successful
         */ 
        int setLabels(std::list<std::string>& labels);
        /**
         * @brief Begin draw progressbars
         * @retval 0 Successful
         */ 
        int start();
        /**
         * @brief Finish draw progressbars
         * @retval 0 Successful
         */ 
        int finish();
        /**
         * @brief Update progressbar value
         * @param progress Values from 0-100
         * @retval 0 Successful
         */ 
        int update(std::list<float>& progress);
        
    private:
        struct Bar{
            std::string label;
            float value;
        };
        std::vector<struct Bar> m_bars;
        
        int  m_barLength;
        bool m_printPercent;
        int  m_labelMaxLength;
        
        void printBar(struct Bar& bar);
        
        static int s_maxCol;
        static void SIGWINCH_handler(int sig);
    };
    /**
    * @}
    * @}
    * @}
    */
}}

#endif
