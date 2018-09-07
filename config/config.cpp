#include "config.h"
#include <iostream>
#include <QDebug>


namespace app {
config::config()
{    
#if __linux__
    setting_.stream.port_name = "ttyUSB0";
#else
    setting_.stream.port_name = "COM1";
#endif
    setting_.stream.baudrate = 115200;
    setting_.chart = setting_.stream;
    setting_.program = setting_.stream;
    handle_ = nullptr;
}

config::~config()
{
}

void config::open_file(const QString &file)
{
    filename_ = file;
    handle_ = std::make_shared<QSettings>(filename_, QSettings::IniFormat);
}

void config::load_config_serial(serialport &port)
{
#ifdef __linux__
    QString port_name = "ttyUSB0";
#else
    QString port_name = "COM1";
#endif

    handle_->beginGroup(port.conf_name);
    port.port_name = load_key_value("name", port_name).toString();
    port.baudrate = load_key_value("baudrate", 115200).toInt();
    port.databits = (WordBits)load_key_value("databits", WordBits_8).toInt();
    port.paritybits = (Parity)load_key_value("paritybits", Parity_None).toInt();
    port.stopbits = (StopBits)load_key_value("stopbits", StopBits_1).toInt();
    handle_->endGroup();

    //print_config_serial(port);
}

void config::save_config_serial(const serialport &port)
{
    handle_->beginGroup(port.conf_name);
    set_key_value("name",       port.port_name);
    set_key_value("baudrate",   port.baudrate);
    set_key_value("databits",   port.databits);
    set_key_value("paritybits", port.paritybits);
    set_key_value("stopbits",   port.stopbits);
    handle_->endGroup();
}

void config::print_config_serial(const serialport &port)
{
    qDebug() << "config name: " << port.conf_name <<
                "name: " << port.port_name <<
                "baudrate: " << port.baudrate <<
                "data bits: " << port.databits <<
                "parity bits: " << port.paritybits <<
                "stop bits: " << port.stopbits;
}

void config::load_config_all()
{
    setting_.chart.conf_name = "debug_port";
    load_config_serial(setting_.chart);
    setting_.stream.conf_name = "stream_port";
    load_config_serial(setting_.stream);
    setting_.program.conf_name = "program_port";
    load_config_serial(setting_.program);
    setting_.display = load_key_value("display", "ascii").toString();
    setting_.protocol = load_key_value("protocol", "raw").toString();
    setting_.palette = load_key_value("palette", "dark").toString();
}

void config::save_config_all(app::appsetting setting)
{
    set_app_setting(setting);
    save_config_serial(setting_.chart);
    save_config_serial(setting_.stream);
    save_config_serial(setting_.program);
    set_key_value("display", setting.display);
    set_key_value("protocol", setting.protocol);
    set_key_value("palette", setting.palette);
}

QVariant config::load_key_value(const QString &key, QVariant def)
{
    if(contains(key)) {
        return handle_->value(key);
    } else {
        qDebug() << "gen def value " << def;
        handle_->setValue(key, def);
        return def;
    }
}

void config::set_key_value(const QString &key, const QVariant &val)
{
    handle_->setValue(key, val);
}

bool config::contains(const QString &key)
{
    return handle_->contains(key);
}


}
