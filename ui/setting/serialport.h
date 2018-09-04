#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QtWidgets>
#include <config/config.h>
#include <QSerialPortInfo>


class SerialPort : public QWidget
{
    Q_OBJECT
public:
    explicit SerialPort(QWidget *parent = 0);
    void initVariable();
    void createElement();
    void createLayout();
    void createConnection();
    void loadSerialSetup(const app::serialport &port);
    void saveSerialSetup(app::serialport &port);
    bool FindText(QString &s);
private slots:
    void scanSerialPort();
    void setSerialPort();
    void loadAllUiSetup();

private:
    QComboBox *listSerial;
    QRadioButton *rbtn9600, *rbtn19200, *rbtn38400,
    *rbtn115200, *rbtn230400, *rbtn460800, *rbtn921600,
    *rbtnOther, *rbtnD5, *rbtnD6, *rbtnD7, *rbtnD8,
    *rbtnNone, *rbtnEven, *rbtnOdd, *rbtnOne, *rbtn1_5,
    *rbtnTwo, *rbtnAscii, *rbtnHex, *rbtnRaw, *rbtnProto;

    QSpinBox *spinBaurate;

    QComboBox *listApp;
};


#endif // SERIALPORT_H
