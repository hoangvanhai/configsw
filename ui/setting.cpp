#include "setting.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QSizePolicy>

Setting::Setting(QWidget *parent) : QMainWindow(parent)
{    
    initVariable();
    createElement();
    createLayout();
    createConnection();
}

void Setting::initVariable()
{

}

void Setting::createElement()
{
    hLayout = new QHBoxLayout();
    contentsWidget = new QListWidget;
    stacker = new SlidingStackedWidget(this);




}

void Setting::createIcons()
{
    /** create icon
      */
    serial = new QListWidgetItem(contentsWidget);

    serial->setIcon(QIcon("://icon/settings.png"));
    serial->setText(tr("Serial Port"));
    serial->setTextAlignment(Qt::AlignCenter);
    serial->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    harware = new QListWidgetItem(contentsWidget);
    harware->setIcon(QIcon("://icon/device.png"));
    harware->setText(tr("Hardware"));
    harware->setTextAlignment(Qt::AlignCenter);
    harware->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    permiss = new QListWidgetItem(contentsWidget);
    permiss->setIcon(QIcon("://icon/manager.png"));
    permiss->setText(tr("Permission"));
    permiss->setTextAlignment(Qt::AlignCenter);
    permiss->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void Setting::createLayout()
{
    createIcons();
    QWidget *centerWidget = new QWidget;
    contentsWidget->setCurrentRow(0);
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(40, 40));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(85);
    contentsWidget->setSpacing(6);

    stacker->addWidget(new SerialPort(this));
    stacker->addWidget(new Hardware(this));
    stacker->addWidget(new Permission(this));
    stacker->setSpeed(600);
    stacker->setVerticalMode(false);
    stacker->setAnimation(QEasingCurve::OutBack);

    hLayout->addWidget(contentsWidget);
    hLayout->addWidget(stacker, 1);
    centerWidget->setLayout(hLayout);
    setCentralWidget(centerWidget);

    statusBar()->showMessage("ready");
}

void Setting::createConnection()
{
    connect(contentsWidget, SIGNAL(currentRowChanged(int)), stacker, SLOT(slideInIdx(int)));
}

void Setting::showMessage(QString msg)
{
    statusBar()->showMessage(msg);
}

