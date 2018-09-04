/**
 * @addtogroup pattern
 * @{
 * @defgroup pattern_chain_of_responsibility
 * @addtogroup pattern_chain_of_responsibility
 * @{
 * @note Use this pattern: nested function(handle) calls too much
 @code
#include <memory>
#include <iostream>

class Request{
public:
    Request(int val){value = val;}
    virtual ~Request(){}
    int value;
};

class ReceiverA : public pattern::chain_of_responsibility::IReceiver<Request>{
public:
    ReceiverA() : pattern::chain_of_responsibility::IReceiver<Request>()
    {
    }
    virtual ~ReceiverA(){
    }
    int handle(std::shared_ptr<Request> req){
        if(req->value % 2 == 0){
            std::cout<<"A "<<"handle "<<req->value<<std::endl;
            return 0;
        }else
            return pattern::chain_of_responsibility::IReceiver<Request>::handle(req);
    }
};
class ReceiverB : public pattern::chain_of_responsibility::IReceiver<Request>{
public:
    ReceiverB() : pattern::chain_of_responsibility::IReceiver<Request>()
    {
    }
    virtual ~ReceiverB(){
    }
    int handle(std::shared_ptr<Request> req){
        if(req->value % 3 == 0){
            std::cout<<"B "<<"handle "<<req->value<<std::endl;
            return 0;
        }else
            return pattern::chain_of_responsibility::IReceiver<Request>::handle(req);
    }
};
class ReceiverC : public pattern::chain_of_responsibility::IReceiver<Request>{
public:
    ReceiverC() : pattern::chain_of_responsibility::IReceiver<Request>()
    {
    }
    virtual ~ReceiverC(){
    }
    int handle(std::shared_ptr<Request> req){
        if(req->value % 5 == 0){
            std::cout<<"C "<<"handle "<<req->value<<std::endl;
            return 0;
        }else
            return pattern::chain_of_responsibility::IReceiver<Request>::handle(req);
    }
};

int main(int argc, char** argv){

    std::shared_ptr<Request> req = std::make_shared<Request>(0);
    std::shared_ptr<ReceiverA> receiver = std::make_shared<ReceiverA>();

    receiver->addReceiver(std::make_shared<ReceiverB>());
    receiver->addReceiver(std::make_shared<ReceiverC>());

    for(int i = 1; i < 20; i++){
        req->value = i;
        if(receiver->handle(req)){
            std::cout<<i<<" is not handled"<<std::endl;
        }
    }

    return 0;
}
 @endcode
 */ 
#ifndef __I_CHAIN_OF_RESPONSIBILITY_H__
#define __I_CHAIN_OF_RESPONSIBILITY_H__
#include <memory>
namespace pattern{
    /**
     * @brief Chain of Responsibility
     */ 
    namespace chain_of_responsibility{
        template <typename request>
        /**
         * @brief Receiver
         */ 
        class IReceiver{
        public:
            IReceiver(){
                m_pattern_NextReceiver = nullptr;
            }
            virtual ~IReceiver(){}
            /**
             * @brief Add a receiver object to end of current list
             * @param receiver Receiver object
             */ 
            void addReceiver(std::shared_ptr<IReceiver> receiver){
                if(m_pattern_NextReceiver == nullptr)
                    m_pattern_NextReceiver = receiver;
                else
                    m_pattern_NextReceiver->addReceiver(receiver);
            }
            /**
             * @brief Send request to next receiver object
             * @param req Request object
             * @retval 0 Request is handled
             */ 
            virtual int handle(std::shared_ptr<request> req){
                if(m_pattern_NextReceiver != nullptr) return m_pattern_NextReceiver->handle(req);
                else return -1;
            }
        private:
            std::shared_ptr<IReceiver> m_pattern_NextReceiver;
        };
    }
}

#endif
/**
 * @}
 * @}
 */ 
