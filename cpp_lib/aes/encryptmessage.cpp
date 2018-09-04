#include "encryptmessage.h"
#include <QDebug>
#include "timecount.h"

char EncryptMessage::tempOutput[65536 * 2] = { 0 };
char EncryptMessage::mKey[32] = { 0 };
char EncryptMessage::mPayload[65536] = { 0 };
char EncryptMessage::mIv[16] = { 0 };

int EncryptMessage::payloadLength = 0;

EncryptMessage::EncryptMessage(QObject *parent) : Message(parent)
{

}

bool EncryptMessage::setData(const QByteArray &value) {
    if (value.length() < 16) return false;

    payloadLength = value.length() - 16;

    memcpy(mIv, value.data(), 16);
    memcpy(mPayload, value.data() + 16, payloadLength);
//    for (int i = 0; i < value.length(); i++) {
//        if (i < 16) {
//            iv.append(value.at(i));
//        } else {
//            encryptPayload.append(value.at(i));
//        }
//    }

    return true;
}



QByteArray EncryptMessage::dataDecrypt(QByteArray key){
    Crypto* crypto = new Crypto();

    memcpy(mKey, key.data(), 32);
//    memcpy(mPayload, encryptPayload.data(), encryptPayload.length());
//    memcpy(mIv, iv.data(), 16);
    crypto->SetKey(mKey, 32);

    crypto->Decrypt(mPayload, payloadLength, mIv, tempOutput);
    delete crypto;

    QByteArray result(tempOutput, payloadLength);
    return result;
}

void EncryptMessage::printMessageData()
{
    qInfo() << iv.length() << encryptPayload.length();
}

