#include "StackOfLayer.h"

using namespace communication;
using namespace communication::protocol;

StackOfLayer::StackOfLayer() : ILayer(){
    m_enable.store(false);
}
StackOfLayer::~StackOfLayer(){
    
}
/* Override ILayer */
int StackOfLayer::start(){ 
    m_enable.store(true);
    reset();
    return 0;
}
int StackOfLayer::stop(){ 
    m_enable.store(false);
    return 0;
}
int StackOfLayer::writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    if((int)m_layers.size() <= 0) return 0;
    std::shared_ptr<ILayer> highest = m_layers[(int)m_layers.size() - 1];
    return highest->writeToLower(data, dataLength, obj);
}
int StackOfLayer::writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    if((int)m_layers.size() <= 0) return 0;
    std::shared_ptr<ILayer> lowest = m_layers[0];
    return lowest->writeToUpper(data, dataLength, obj);
}
int StackOfLayer::waitEvent(int eventNeedToWait, int timeoutInMilisecond){
    if((int)m_layers.size() <= 0) return communication::Event_Error;
    std::shared_ptr<ILayer> highest = m_layers[(int)m_layers.size() - 1];
    return highest->waitEvent(eventNeedToWait, timeoutInMilisecond);
}
int StackOfLayer::reset(){
    int size = (int)m_layers.size();
    if(size <= 0) return -1;
    
    std::shared_ptr<ILayer> lower = nullptr;
    StackOfLayer* inst = this;
    for(int i = 0; i < size; i++){
        std::shared_ptr<ILayer> layer = m_layers[i];
        if(lower){
            layer->setCallback(
                [i, inst](const void* data, int dataLength, std::shared_ptr<communication::Object> obj) -> int{
                    if(!inst->m_enable.load()) return 0;
                    return inst->m_layers[i - 1]->writeToLower(data, dataLength, obj);
                },
                nullptr,
                [i, inst](int event, int timeout) -> int{
                    if(!inst->m_enable.load()) return 0;
                    return inst->m_layers[i - 1]->waitEvent(event, timeout);
                }
            );
            lower->setCallback(
                nullptr,
                [i, inst](const void* data, int dataLength, std::shared_ptr<communication::Object> obj) -> int{
                    if(!inst->m_enable.load()) return communication::Event_Error;
                    return inst->m_layers[i]->writeToUpper(data, dataLength, obj);
                },
                nullptr
            );
        }
        lower = layer;
    }
    m_layers[0]->setCallback(
        [&](const void* data, int dataLength, std::shared_ptr<communication::Object> obj) -> int{
            if(m_wLower)
                return m_wLower(data, dataLength, obj);
            return 0;
        },
        nullptr,
        [&](int event, int timeout) -> int{
            if(m_waitLower)
                return m_waitLower(event, timeout);
            return communication::Event_Error;
        }
    );
    m_layers[size - 1]->setCallback(
        nullptr,
        [&](const void* data, int dataLength, std::shared_ptr<communication::Object> obj) -> int{
            if(m_wUpper)
                return m_wUpper(data, dataLength, obj);
            return 0;
        },
        nullptr
    );
    
    return 0;
}
/* Own API */
int StackOfLayer::push(std::shared_ptr<ILayer> layer){
    m_layers.push_back(layer);
    return 0;
}
int StackOfLayer::count(){
    return (int)m_layers.size();
}
std::shared_ptr<ILayer> StackOfLayer::operator[](int layerIndex){
    if(layerIndex < 0 || layerIndex >= (int)m_layers.size()) return nullptr;
    return m_layers[layerIndex];
}
int StackOfLayer::erase(int layerIndex){
    if(layerIndex < 0 || layerIndex >= (int)m_layers.size()) return -1;
    std::vector<std::shared_ptr<ILayer>>::iterator iter = m_layers.begin();
    while(layerIndex--){ ++iter; }
    m_layers.erase(iter);
    return 0;
}
int StackOfLayer::replace(int layerIndex, std::shared_ptr<ILayer> layer){
    if(layerIndex < 0 || layerIndex >= (int)m_layers.size()) return -1;
    m_layers[layerIndex] = layer;
    return 0;
}
// end of file
            
