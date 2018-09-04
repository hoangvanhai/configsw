#include "fifo.h"
#include <iostream>
/**
 * @brief FIFO::FIFO
 */
FIFO::FIFO()
{
    arrBuff = nullptr;
    maxSize = 0;
    size = 0;
    head = 0;
    tail = 0;
    protect = false;
    protectPtr = 0;
    id16 = 0;
    id8 = 0;
}

FIFO::~FIFO()
{    
}

/**
 * @brief FIFO::FIFO
 * @param buffer
 * @param sSize
 */
bool FIFO::Init(uint32_t sSize)
{    

    if(arrBuff == nullptr) {
        std::shared_ptr<uint8_t> pMem(new uint8_t[sSize], [](uint8_t* p){delete[] p;});
        arrBuff = pMem;
    }

    if(arrBuff == nullptr)
        return false;

    maxSize = sSize;
    size = 0;
    head = 0;
    tail = 0;
    protect = false;
    protectPtr = 0;

    return true;
}

void FIFO::DeInit()
{
    arrBuff.reset();
}

bool FIFO::Push(uint8_t data)
{
    if(arrBuff == nullptr) {
        return false;
    }
    if(size < maxSize) {
        reinterpret_cast<uint8_t*>(arrBuff.get())[tail] = data;
        tail++;

        if(tail >= maxSize) {
            tail = 0;
        }
        size++;
        return true;
    }
    return false;
}

uint32_t FIFO::Push(uint8_t *data, uint32_t length)
{    
    uint32_t ret = 0, idx = 0;
    uint32_t remain = RemainSpace();    
    if(length > remain) {
        std::cout << "req " << length << " rem " << remain << " ";
        length = remain;
    }

    for(idx = 0; idx < length; idx++) {
        if(Push(data[idx]))
            ret++;
    }
    return ret;
}

bool FIFO::Pop(uint8_t &data) {
    if(arrBuff == NULL) {
        return false;
    }
    if(size > 0) {        
        data = reinterpret_cast<uint8_t*>(arrBuff.get())[head];
        head++;
        if(head >= maxSize) {
            head = 0;
        }
        size--;
        return true;
    }
    data=0xFF;
    return false;
}

uint8_t FIFO::Pop()
{
    uint8_t byte;
    if(Pop(byte))
        return byte;
    else
        return 0xFF;
}


void FIFO::EnableProtect()
{
    protect = true;
    protectPtr = head;
}

void FIFO::DisableProtect()
{
    protect = false;
}

void FIFO::RewindHeader()
{
    if(protect) {
        while(head != protectPtr) {
            if(head == 0) {
                head = maxSize - 1;
            }else{
                head--;
            }
            size++;
            if(head == tail)
                break;
        }
    }
}


bool FIFO::IsEnablePush() {
    if(size == maxSize ||
            (protect == true && size != 0 && tail == head)) {
        return false;
    }
    return true;
}



void FIFO::Reset()
{
    size = 0;
    head = 0;
    tail = 0;
    protect = false;
}

bool FIFO::IsFull() {
    return IsEnablePush();
}


bool FIFO::IsEmpty() {
    return (size == 0);
}


uint32_t FIFO::GetCount() {
    return size;
}

uint32_t FIFO::RemainSpace()
{
    return (maxSize - GetCount());
}


uint32_t FIFO::Recv(uint8_t *data, uint32_t len)
{
    uint32_t idx;
    uint8_t byte = 0;

    if(!data) return 0;

    if(len > GetCount()) {
        len = GetCount();
    }

    for(idx = 0; idx < len; idx++) {
        Pop(byte);
        *data++ = byte;
    }

    return len;
}
