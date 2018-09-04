#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <memory>

class FIFO
{
public:
    FIFO();
    ~FIFO();
    bool        Init(uint32_t sSize);
    void        DeInit();
    bool        Pop(uint8_t &data);
    uint8_t     Pop();
    bool        Push(uint8_t data);
    uint32_t    Push(uint8_t *data, uint32_t length);
    void        EnableProtect();
    void        DisableProtect();
    void        RewindHeader();
    bool        IsEnablePush();
    bool        IsEmpty();
    bool        IsFull();
    uint32_t    GetCount();
    uint32_t    RemainSpace();
    void        Reset();        // Reset count, head, tail
    uint32_t    Recv(uint8_t *data, uint32_t len);

private:
    std::shared_ptr<uint8_t>     arrBuff;
    uint32_t    maxSize;
    uint32_t    size;
    uint32_t    head;
    uint32_t    tail;
    bool        protect;
    uint32_t    protectPtr;
    uint16_t    id16;
    uint16_t    id8;

};

#endif // FIFO_H
