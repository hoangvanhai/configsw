#include "config.h"
#include <iostream>
#include <QDebug>
#include <QDir>

namespace app {
config::config()
{    
#if __linux__
    setting_.stream.port_name = "ttyUSB0";
#else
    setting_.stream.port_name = "COM1";
#endif
    setting_.stream.baudrate = 115200;
    setting_.control = setting_.stream;
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


void config::print_config_batt(const battcharg &batt)
{
    qDebug() << "config name: " << batt.conf_name <<
                "id: " << batt.id <<
                "float volt: " << batt.floatVolt <<
                "boost volt: " << batt.boostVolt <<
                "boost curr: " << batt.boostVolt <<
                "boost time: " << batt.boostTime <<
                "vusb: " << batt.vUsb <<
                "charg: " << batt.charg <<
                "log: " << batt.dataLog;

}


void config::load_battcharg_cfg(battcharg &setting, battcharg def)
{
    handle_->beginGroup(setting.conf_name);
    setting.id          = load_key_value("id", def.id).toInt();
    setting.floatVolt   = load_key_value("floatvolt", def.floatVolt).toDouble();
    setting.boostVolt   = load_key_value("boostvolt", def.boostVolt).toDouble();
    setting.boostCurr   = load_key_value("boostcurr", def.boostCurr).toDouble();
    setting.boostTime   = load_key_value("boosttime", def.boostTime).toDouble();
    setting.vUsb        = load_key_value("vusb", def.vUsb).toBool();
    setting.dataLog     = load_key_value("datalog", def.dataLog).toBool();
    setting.charg       = load_key_value("charg", def.charg).toBool();
    handle_->endGroup();
}



void config::save_battcharg_cfg(battcharg &setting)
{
    handle_->beginGroup(setting.conf_name);
    set_key_value("id",         setting.id);
    set_key_value("floatvolt",  setting.floatVolt);
    set_key_value("boostvolt",  setting.boostVolt);
    set_key_value("boostcurr",  setting.boostCurr);
    set_key_value("boosttime",  setting.boostTime);
    set_key_value("vusb",       setting.vUsb);
    set_key_value("datalog",    setting.dataLog);
    set_key_value("charg",      setting.charg);
    handle_->endGroup();
}

void config::load_config_all()
{
    setting_.gen.conf_name = "interface";
    load_config_general(setting_.gen);
    setting_.control.conf_name = "debug_port";
    load_config_serial(setting_.control);
    setting_.stream.conf_name = "stream_port";
    load_config_serial(setting_.stream);
    setting_.program.conf_name = "program_port";
    load_config_serial(setting_.program);

    battcharg def;
    def.id = 1;
    def.floatVolt = 13.8;
    def.boostVolt = 14.2;
    def.boostCurr = 5;
    def.boostTime = 120;
    def.vUsb = true;
    def.charg = true;
    def.dataLog = true;
    setting_.batt.conf_name = "batt_charg";
    load_battcharg_cfg(setting_.batt, def);
    setting_.battdef.conf_name = "batt_charg_def";
    load_battcharg_cfg(setting_.battdef, def);
}

void config::save_config_all(app::appsetting setting)
{
    set_app_setting(setting);
    save_config_serial(setting_.control);
    save_config_serial(setting_.stream);
    save_config_serial(setting_.program);
    save_battcharg_cfg(setting_.batt);
    save_config_general(setting_.gen);
}

void config::load_config_general(general &gen)
{
    handle_->beginGroup(gen.conf_name);
    gen.display = load_key_value("display", "ascii").toString();
    gen.protocol = load_key_value("protocol", "raw").toString();
    gen.palette = load_key_value("palette", "dark").toString();
    gen.filePathImport = load_key_value("file_import", QDir::homePath()).toString();
    gen.filePathExport = load_key_value("file_export", QDir::homePath()).toString();
    gen.model = load_key_value("model", "unknown").toString();
    handle_->endGroup();
}

void config::save_config_general(const general &gen)
{
    handle_->beginGroup(gen.conf_name);
    set_key_value("display",        gen.display);
    set_key_value("protocol",       gen.protocol);
    set_key_value("palette",        gen.palette);
    set_key_value("file_import",    gen.filePathImport);
    set_key_value("file_export",    gen.filePathExport);
    set_key_value("model",          gen.model);
    handle_->endGroup();
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
