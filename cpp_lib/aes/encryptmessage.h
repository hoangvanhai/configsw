#ifndef ENCRYPTMESSAGE_H
#define ENCRYPTMESSAGE_H

#include <QObject>
#include <message.h>
#include <crypto.h>
#include <QByteArray>

class EncryptMessage : public Message
{
    Q_OBJECT
public:
    explicit EncryptMessage(QObject *parent = 0);
    static char tempOutput[65536 * 2];
    static char mKey[32];
    static char mPayload[65536];
    static int payloadLength;
    static char mIv[16];

    QByteArray iv;
    QByteArray encryptPayload;

    bool setData(const QByteArray &value);
    QByteArray dataDecrypt(QByteArray key);

    void printMessageData();
signals:

public slots:
};

#endif // ENCRYPTMESSAGE_H
