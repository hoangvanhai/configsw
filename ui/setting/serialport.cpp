#include "serialport.h"


SerialPort::SerialPort(QWidget *parent) :
    QWidget(parent)
{
    initVariable();
    createElement();
    createLayout();
    createConnection();
}

void SerialPort::initVariable()
{

}

void SerialPort::createElement()
{

    rbtnNone = new QRadioButton("None");
    rbtnEven = new QRadioButton("Even");
    rbtnOdd = new QRadioButton("Odd");

    rbtn9600 = new QRadioButton("9600");
    rbtn19200 = new QRadioButton("19200");
    rbtn38400 = new QRadioButton("38400");
    rbtn115200 = new QRadioButton("115200");
    rbtn230400 = new QRadioButton("230400");
    rbtn460800 = new QRadioButton("460800");
    rbtn921600 = new QRadioButton("921600");
    rbtnOther = new QRadioButton("other");
    spinBaurate = new QSpinBox;

    rbtnD8 = new QRadioButton("8 bits");
    rbtnD7 = new QRadioButton("7 bits");
    rbtnD6 = new QRadioButton("6 bits");
    rbtnD5 = new QRadioButton("5 bits");

    rbtnOne = new QRadioButton("1 bit");
    rbtn1_5 = new QRadioButton("1.5 bits");
    rbtnTwo = new QRadioButton("2 bits");

    rbtnAscii = new QRadioButton("Ascii");
    rbtnHex = new QRadioButton("Hex");

    rbtnRaw = new QRadioButton("Raw");
    rbtnProto = new QRadioButton("Protocol");
    listApp = new QComboBox;
    listSerial = new QComboBox;
}

void SerialPort::createLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    {
    QGroupBox *groupSelect = new QGroupBox("Serial Select");
    QHBoxLayout *hLayout = new QHBoxLayout;
    listApp->addItems(QStringList() << "Debug" << "Streaming IBC" << "Program");
    hLayout->addWidget(new QLabel(tr("Type")), 0, Qt::AlignRight);
    hLayout->addWidget(listApp, 0, Qt::AlignLeft);
    hLayout->addSpacerItem(new QSpacerItem(10,10));
    hLayout->addWidget(new QLabel(tr("Port")), 0, Qt::AlignRight);
    hLayout->addWidget(listSerial,1, Qt::AlignLeft);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupSelect = new QGroupBox("Baud Rate");
    QHBoxLayout *hLayout = new QHBoxLayout;


    hLayout->addWidget(rbtn9600, 0,Qt::AlignTop);
    hLayout->addWidget(rbtn19200, 0, Qt::AlignTop);
    hLayout->addWidget(rbtn38400, 0, Qt::AlignTop);
    hLayout->addWidget(rbtn115200, 0,  Qt::AlignTop);
    hLayout->addWidget(rbtn230400, 0, Qt::AlignTop);
    hLayout->addWidget(rbtn460800, 0,  Qt::AlignTop);
    hLayout->addWidget(rbtn921600, 0,Qt::AlignTop);
    hLayout->addWidget(rbtnOther, 0,  Qt::AlignTop);
    hLayout->addWidget(spinBaurate, 1, Qt::AlignTop);


    spinBaurate->setMinimumWidth(90);
    spinBaurate->setMaximum(5000000);
    spinBaurate->setValue(115200);

    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);

    }

    {
    QGroupBox *groupSelect = new QGroupBox("Data Bits");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(rbtnD5, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnD6, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnD7, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnD8, 1, Qt::AlignTop);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupSelect = new QGroupBox("Parity Bits");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(rbtnNone, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnEven, 0,Qt::AlignTop);
    hLayout->addWidget(rbtnOdd, 0, Qt::AlignTop);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupSelect = new QGroupBox("Stop Bits");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(rbtnOne, 0, Qt::AlignTop);
    hLayout->addWidget(rbtn1_5, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnTwo, 1, Qt::AlignTop);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupSelect = new QGroupBox("Debug Display");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(rbtnAscii, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnHex, 1, Qt::AlignTop);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupSelect = new QGroupBox("Debug Protocol");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(rbtnRaw, 0, Qt::AlignTop);
    hLayout->addWidget(rbtnProto, 1, Qt::AlignTop);
    groupSelect->setLayout(hLayout);
    mainLayout->addWidget(groupSelect,0, Qt::AlignTop | Qt::AlignLeft);
    }

    QPushButton *btnSet = new QPushButton("Save");
    connect(btnSet, SIGNAL(clicked(bool)), this, SLOT(setSerialPort()));
    mainLayout->addWidget(btnSet, 1, Qt::AlignTop | Qt::AlignLeft);

    setLayout(mainLayout);


}

void SerialPort::createConnection()
{
    connect(listSerial, SIGNAL(activated(int)), this, SLOT(scanSerialPort()));
    connect(listSerial, SIGNAL(highlighted(QString)), this, SLOT(scanSerialPort()));
    connect(listApp, SIGNAL(activated(int)), this, SLOT(loadAllUiSetup()));
    scanSerialPort();
    loadAllUiSetup();
}

void SerialPort::loadAllUiSetup()
{
    app::appsetting setting = app::config::instance()->get_app_setting();

    if(listApp->currentText() == "Debug") {
        loadSerialSetup(setting.debug);
    } else if(listApp->currentText() == "Streaming IBC") {
        loadSerialSetup(setting.stream);
    } else if(listApp->currentText() == "Program") {
        loadSerialSetup(setting.program);
    } else {
        qDebug() << "list app not handle";
    }

    if(setting.display == "ascii") {
        rbtnAscii->setChecked(true);
    } else if(setting.display == "hex") {
        rbtnHex->setChecked(true);
    } else {
        qDebug() << "set display failed";
    }

    if(setting.protocol == "raw") {
        rbtnRaw->setChecked(true);
    } else  if(setting.protocol == "isc") {
        rbtnProto->setChecked(true);
    } else {
        qDebug() << "set protocol failed";
    }
}

void SerialPort::loadSerialSetup(const app::serialport &port)
{
    //qDebug() << "load setting for " << port.conf_name;
    //app::config::instance()->print_config_serial(port);

    int idx = listSerial->findText(port.port_name);
    if(idx != -1) {
        listSerial->setCurrentIndex(idx);
    }

    switch (port.baudrate) {
    case 9600: rbtn9600->setChecked(true);
        break;
    case 19200: rbtn19200->setChecked(true);
        break;
    case 38400: rbtn38400->setChecked(true);
        break;
    case 115200: rbtn115200->setChecked(true);
        break;
    case 230400: rbtn230400->setChecked(true);
        break;
    case 460800: rbtn460800->setChecked(true);
        break;
    case 921600: rbtn921600->setChecked(true);
        break;
    default: rbtnOther->setChecked(true);
        break;
    }

    switch(port.databits) {
    case WordBits_5: rbtnD5->setChecked(true);
        break;
    case WordBits_6: rbtnD6->setChecked(true);
        break;
    case WordBits_7: rbtnD7->setChecked(true);
        break;
    case WordBits_8: rbtnD8->setChecked(true);
        break;
    default: break;
    }

    switch(port.paritybits) {
    case Parity_None: rbtnNone->setChecked(true);
        break;
    case Parity_Even: rbtnEven->setChecked(true);
        break;
    case Parity_Odd: rbtnOdd->setChecked(true);
        break;
    default: break;
    }

    switch(port.stopbits) {
    case StopBits_1: rbtnOne->setChecked(true);
        break;
    case StopBits_1_5: rbtn1_5->setChecked(true);
        break;
    case StopBits_2: rbtnTwo->setChecked(true);
        break;
    default: break;
    }
}

void SerialPort::saveSerialSetup(app::serialport &port)
{
    port.port_name = listSerial->currentText();

    if(rbtn9600->isChecked()) {
        port.baudrate = 9600;
    }else if(rbtn19200->isChecked()) {
        port.baudrate = 19200;
    }else if(rbtn38400->isChecked()){
        port.baudrate = 38400;
    }else if(rbtn115200->isChecked()) {
        port.baudrate = 115200;
    }else if(rbtn230400->isChecked()) {
        port.baudrate = 230400;
    }else if(rbtn460800->isChecked()) {
        port.baudrate = 460800;
    }else if(rbtn921600->isChecked()) {
        port.baudrate = 921600;
    }else if(rbtnOther->isChecked()) {
        port.baudrate = spinBaurate->value();
    } else {
        qDebug() << "baudrate not set\n";
    }


    if(rbtnD5->isChecked()) {
        port.databits = WordBits_5;
    }else if(rbtnD6->isChecked()){
        port.databits = WordBits_6;
    }else if(rbtnD7->isChecked()){
        port.databits = WordBits_7;
    }else if(rbtnD8->isChecked()){
        port.databits = WordBits_8;
    } else {
        qDebug() << "data bits not set\n";
    }

    if(rbtnNone->isChecked()) {
        port.paritybits = Parity_None;
    }else if(rbtnEven->isChecked()){
        port.paritybits= Parity_Even;
    }else if(rbtnOdd->isChecked()){
        port.paritybits = Parity_Odd;
    } else {
        qDebug() << "parity bits not set\n";
    }


    if(rbtnOne->isChecked()) {
        port.stopbits = StopBits_1;
    }else if(rbtn1_5->isChecked()){
        port.stopbits= StopBits_1_5;
    }else if(rbtnTwo->isChecked()){
        port.stopbits = StopBits_2;
    }else {
        qDebug() << "stop bits not set\n";
    }
}


void SerialPort::scanSerialPort()
{
    QList<QSerialPortInfo> lst = QSerialPortInfo::availablePorts();
    if(lst.count() != listSerial->count())
        listSerial->clear();

    foreach (const QSerialPortInfo &info, lst) {
        QString s = info.portName();
        if(FindText(s) == false)
            listSerial->addItem(s);
    }
}

void SerialPort::setSerialPort()
{
    app::appsetting setting = app::config::instance()->get_app_setting();

    if(listApp->currentText() == "Debug") {
        saveSerialSetup(setting.debug);
    } else if(listApp->currentText() == "Streaming IBC") {
        saveSerialSetup(setting.stream);
    } else if(listApp->currentText() == "Program") {
        saveSerialSetup(setting.program);
    } else {
        qDebug() << "not handle";
    }

    if(rbtnAscii->isChecked()) {
        setting.display == "ascii";
    } else if(rbtnHex->isChecked()) {
        setting.display == "hex";
    } else {
        qDebug() << "failed set display\n";
    }

    if(rbtnRaw->isChecked()) {
        setting.protocol = "raw";
    } else if(rbtnProto->isChecked()) {
        setting.protocol = "isc";
    } else {
        qDebug() << "failed set protocol\n";
    }

    app::config::instance()->save_config_all(setting);
}


bool SerialPort::FindText(QString &s)
{
    for(int i = 0; i < listSerial->count(); i++) {
        if(listSerial->itemText(i) == s) {
            return true;
        }
    }
    return false;
}
