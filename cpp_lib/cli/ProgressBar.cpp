#include "ProgressBar.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <csignal>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

using namespace cppframework::cli;
#ifndef SIGWINCH
#define SIGWINCH 0
#endif

int ProgressBar::s_maxCol = 80;
void ProgressBar::SIGWINCH_handler(int sig){
#ifdef __linux__
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    s_maxCol = w.ws_col;
#endif
}
ProgressBar::ProgressBar(int count){
#ifdef __linux__
    std::signal(SIGWINCH, SIGWINCH_handler);
#endif
    SIGWINCH_handler(SIGWINCH);
      
    for(int i = 0; i < count; i++){
        struct Bar bar;
        bar.label = "";
        bar.value = 0;
        m_bars.push_back(bar);
    }
    m_barLength     = 50;
    m_labelMaxLength = 0;
    m_printPercent  = true;
}
ProgressBar::~ProgressBar(){
}
int ProgressBar::setLabels(std::list<std::string>& labels){
    
    int index = 0;
    for(auto label : labels){
        if(index < (int)m_bars.size()){
            m_bars[index].label = label;
            if((int)label.length() > m_labelMaxLength) m_labelMaxLength = label.length();
        }else break;
        index++;
    }
    
    return 0;
}
int ProgressBar::start(){
    for(auto bar : m_bars){
        printBar(bar);
        printf("\r\n");
    }
    fflush(stdout);
    return 0;
}
int ProgressBar::finish(){
    printf("\r");
    fflush(stdout);
    return 0;
}
int ProgressBar::update(std::list<float>& progress){
    
    m_barLength = s_maxCol - m_labelMaxLength - 1 - 2;
    
    if(m_printPercent){
        m_barLength -= (1 + 6);
    }
    m_barLength = m_barLength * 2 / 3;
    
    if(m_barLength < 1) m_barLength = 1; 
    
    int index = 0;
    for(auto val : progress){
        if(index >= (int)m_bars.size()) break;
        m_bars[index].value = val;
        index++;
    }
    
    // move up to m_bars.size() lines
    printf("\033[%dA", (int)m_bars.size());
    index = 0;
    for(auto bar : m_bars){
        printBar(bar);
        // move down 1 line
        printf("\033[%dB", 1);
    }
    fflush(stdout);
    return 0;
}
void ProgressBar::printBar(struct Bar& bar){
    int i;
    printf("\r");
    // clear current line
    printf("\033[K");
    // goto begin of line
    printf("\r");
    // print label
    printf("%s ", bar.label.c_str());
    for(i = bar.label.length(); i < m_labelMaxLength; i++){
        printf(" ");
    }
    // print value
    int valLen = bar.value * m_barLength / 100;
    for(i = 0; i < valLen; i++){
        printf("\u2587");
    }
    // print space
    for(; i < m_barLength; i++){
        printf(" ");
    }
    if(m_printPercent){
        if(bar.value == 0 || bar.value == 100)
            printf(" %d%%", (int)bar.value);
        else
            printf(" %.2f%%", bar.value);
    }
}
