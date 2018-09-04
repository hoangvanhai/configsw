/**
 * @addtogroup communication_protocol
 * @{
 *
 */ 
#ifndef STACKOFLAYER_H
#define STACKOFLAYER_H

#include "ILayer.h"
#include <vector>
#include <atomic>

namespace communication{
    namespace protocol{
        /**
         * @brief Implement stack of layers as a normal layer, internal layers will be auto connected
         @code
            <--> Layer1 <--> Layer2 <--> Layer3 <--> ... <--> LayerN <-->
         @endcode
         */ 
        class StackOfLayer : public ILayer{
        public:
            StackOfLayer();
            virtual ~StackOfLayer();
            /* Override ILayer */
            int setAttribute(int attrId, void* attr, int attrSize = 0){ return -1; }
            int getAttribute(int attrId, void* attr, int attrSize = 0){ return -1; }
            int writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr);
            int writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr);
            int waitEvent(int eventNeedToWait, int timeoutInMilisecond);
            int start();
            int stop();
            int reset();
            /* Own API */
            /**
             * @brief Add a layer on top of stack
             * @param layer new layer
             * @retval 0 Successfuly
             */ 
            int push(std::shared_ptr<ILayer> layer);
            /**
             * @brief Get number of layers in stack
             * @return int Number of layers
             */ 
            int count();
            /**
             * @brief Get layer at 'layerIndex'
             * @param layerIndex Layer's index
             * @return std::shared_ptr<ILayer> Pointer to layer is found
             * @retval nullptr Not found
             */ 
            std::shared_ptr<ILayer> operator[](int layerIndex);
            /**
             * @brief Remove layer at index 'layerIndex'
             * @retval 0 Successfuly
             */ 
            int erase(int layerIndex);
            /**
             * @brief Replace layer at 'layerIndex' by 'layer'
             * @param layerIndex Layer's index will be replaced
             * @param layer New layer
             * @retval 0 Successfuly
             */ 
            int replace(int layerIndex, std::shared_ptr<ILayer> layer);
        private:
            
            std::vector<std::shared_ptr<ILayer>> m_layers;
            std::atomic_bool m_enable;
        };
    }
}



#endif // STACKOFLAYER_H
/**
 * @}
 */ 
