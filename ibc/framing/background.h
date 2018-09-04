#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <thread>
#include <functional>
#ifdef __linux__
#include <sys/prctl.h>
#endif
namespace ibc {

class background
{
public:
    background();
    ~background();

    void bg_start();
    void bg_stop();
    void bg_set_work(
            const std::function<void(const void *param)> &work,
            void *param) {
        background_ = work;
        param_ = param;
    }

    void        bg_set_delay(uint32_t delay) {delay_ = delay;}
    uint32_t    bg_get_delay() const {return delay_;}
private:
    void background_function();
    volatile bool run_;
    std::thread thread_;
    std::function<void(const void *param)> background_;
    void        *param_;
    uint32_t    delay_;
};
}
#endif // BACKGROUND_H
