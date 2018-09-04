/**
 * @addtogroup pattern
 * @{
 * @defgroup pattern_observer
 * @addtogroup pattern_observer
 * @{
 * @note Not Thread-safe
 @code
class Object{
public:
    Object(){}
    virtual ~Object(){}
};
class A : public Observer<Object>{
public:
    A() : Observer<Object>(){
    }
    virtual ~A(){}
    void update(std::shared_ptr<Object> obj) override{
        std::cout<<"A received notification"<<std::endl;
    }
};
class B : public Observer<Object>{
public:
    B() : Observer<Object>(){
    }
    virtual ~B(){}
    void update(std::shared_ptr<Object> obj) override{
        std::cout<<"B received notification"<<std::endl;
    }
};
class C : public Observable<Object>{
public:
    C(): Observable<Object>(){
    }
    virtual ~C(){}
};

int main(int argc, char** argv){
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();
    std::shared_ptr<C> c = std::make_shared<C>();
    std::shared_ptr<Object> obj = std::make_shared<Object>();
    
    c->addObserver(a);
    c->addObserver(b);
    
    c->notifyObservers(obj);
    
    return 0;
}
 @endcode
 */ 
#ifndef __PATTERN_IOBSERVER_H__
#define __PATTERN_IOBSERVER_H__
#include <memory>
#include <list>

namespace pattern{
    namespace observer{
        template <typename T>
        /**
         * @brief Objects are inherited this class will be received notifications from other object
         */ 
        class Observer{
        public:
            Observer(){}
            virtual ~Observer(){}
            /**
             * @brief This method is called when Observable object call 'notifyObservers' method
             * @param obj Data object
             * @note Derived class must implement this method for process notify from Observable object
             */ 
            virtual void update(std::shared_ptr<T> obj) = 0;
        };
        template <typename T>
        /**
         * @brief Object are inherited this class can be send notifications to other objects
         */ 
        class Observable{
        public:
            Observable(){}
            virtual ~Observable(){}
            /**
             * @brief Add a Observer object to list
             * @param _observer Observer object
             * @retval 0 Successful
             */ 
            int addObserver(std::shared_ptr<Observer<T>> _observer){
                if(_observer !=  nullptr){
                    m_lstObserver.push_back(_observer);
                    return 0;
                }
                return -1;
            }
            /**
             * @brief Send a notification with 'data' to all Observer objects in list
             * @param data Notification's data, it will be passed to receiver's 'update' method
             */ 
            void notifyObservers(std::shared_ptr<T> data){
                for(auto obj : m_lstObserver){
                    obj->update(data);
                }
            }
        private:
            std::list<std::shared_ptr<Observer<T>>> m_lstObserver;
        };
    }
}

#endif
/**
 * @}
 * @}
 */ 
