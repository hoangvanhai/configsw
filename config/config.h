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


struct appsetting {
    serialport control;
    serialport program;
    serialport stream;
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
