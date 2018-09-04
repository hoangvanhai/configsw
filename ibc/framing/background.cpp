#include "background.h"

namespace ibc {

background::background()
{
    run_ = false;
    delay_ = 10;
}

background::~background()
{
    bg_stop();
}

void background::background_function()
{
    while(run_) {
        if(background_) {
            background_(param_);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
    }
}

void background::bg_start()
{
    bg_stop();
    run_ = true;
    thread_ = std::thread([&]() {
        #ifdef __linux__
        prctl(PR_SET_NAME, "background", 0, 0, 0);
        #endif
        background_function();
    });

}

void background::bg_stop()
{
    run_ = false;
    if(thread_.joinable())
        thread_.join();
}

}
