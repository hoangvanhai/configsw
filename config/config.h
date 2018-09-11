#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <ISingleton.h>
#include <SerialSocket.h>

using namespace lib::pattern;
using namespace communication::endpoint::serialport;

namespace app {

struct serialport {
    QString conf_name;
    QString port_name;
    int baudrate;
    WordBits databits;
    Parity paritybits;
    StopBits stopbits;
    bool hwflowctl;
};

struct battcharg{
    QString  conf_name;
    int      id;
    double   floatVolt;
    double   boostVolt;
    double   boostCurr;
    double   boostTime;
    bool     vUsb;
    bool     dataLog;
    bool     charg;
};

struct appsetting {
    serialport control;
    serialport program;
    serialport stream;
    battcharg batt;
    battcharg battdef;
    QString display;
    QString protocol;
    QString palette;
};



class config : public singleton<config>
{


public:
    config();
    ~config();
    void open_file(const QString &file);
    void load_config_all();
    void save_config_all(appsetting setting);
    void load_config_serial(serialport &port);
    void save_config_serial(const serialport &port);

    void print_config_serial(const serialport &port);
    void print_config_batt(const battcharg &batt);

    void load_battcharg_cfg(battcharg &setting, battcharg def);
    void save_battcharg_cfg(battcharg &setting);

    bool contains(const QString &key);
    QVariant load_key_value(const QString &key, QVariant def);
    void set_key_value(const QString &key, const QVariant &val);
    appsetting get_app_setting() const {return setting_;}
    void set_app_setting(appsetting setting) {setting_ = setting;}

private:
    QString filename_;
    appsetting setting_;
    std::shared_ptr<QSettings> handle_;
};

}
#endif // CONFIG_H
