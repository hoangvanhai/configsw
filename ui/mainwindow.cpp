#include "mainwindow.h"
#include <QtWidgets>
#include <config/config.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initVariable();
    createElement();
    createContent();
    createLayout();
    createConnection();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initVariable()
{
    app::config::instance()->open_file("setting.ini");
    app::config::instance()->load_config_all();
}

void MainWindow::createElement()
{
    tabWidget = new QTabWidget(this);
    setting = new Setting(this);
    develop = new Develop(this);    
    centerWidget = new QWidget;
    mainLayout = new QVBoxLayout;
}

void MainWindow::createContent()
{
    setWindowTitle("acitdev tool kit");
}

void MainWindow::createLayout()
{
    setWindowFlags(Qt::FramelessWindowHint  |
                   Qt::WindowSystemMenuHint |
                   Qt::WindowMinimizeButtonHint);
    QStyle *style   = qApp->style();
    QIcon closeIcon = style->standardIcon(QStyle::SP_TitleBarCloseButton);
    QIcon maxIcon   = style->standardIcon(QStyle::SP_TitleBarMaxButton);
    QIcon minIcon   = style->standardIcon(QStyle::SP_TitleBarMinButton);

    min = new QPushButton;
    max = new QPushButton;
    close = new QPushButton;

    min->setIcon(minIcon);
    max->setIcon(maxIcon);
    close->setIcon(closeIcon);


    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(min, 0, Qt::AlignTop);
    layout->addWidget(max, 0, Qt::AlignTop);
    layout->addWidget(close, 0, Qt::AlignTop);


    tabWidget->addTab(setting, tr("SETTING"));
    tabWidget->addTab(develop, tr("CHARGER"));
    tabWidget->setCurrentWidget(develop);

    QString wgstyle = QString( "QTabBar::tab { height: 35px; width: 180 px;}" );
    tabWidget->setStyleSheet( wgstyle );

    mainLayout->addLayout(layout);
    mainLayout->addWidget(tabWidget);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);

//    QFont font("Times New Roman", 12);
//    font.setStyleHint(QFont::Monospace);
//    qApp->setFont(font);


    setWindowIcon(QIcon("://icon/battery.png"));
    setMinimumSize(1224, 768);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());
    //showMaximized();

}



void MainWindow::createConnection()
{
    connect(min, SIGNAL(clicked(bool)), this, SLOT(minimizeWindow()));
    connect(max, SIGNAL(clicked(bool)), this, SLOT(maximizeWindow()));
    connect(close, SIGNAL(clicked(bool)), this, SLOT(closeWindow()));
}

void MainWindow::createTitleButton()
{
    QStyle *style   = qApp->style();
    QIcon closeIcon = style->standardIcon(QStyle::SP_TitleBarCloseButton);
    QIcon maxIcon   = style->standardIcon(QStyle::SP_TitleBarMaxButton);
    QIcon minIcon   = style->standardIcon(QStyle::SP_TitleBarMinButton);

    QPushButton *min = new QPushButton;
    QPushButton *max = new QPushButton;
    QPushButton *close = new QPushButton;

    min->setIcon(minIcon);
    max->setIcon(maxIcon);
    close->setIcon(closeIcon);


    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(min, 0, Qt::AlignRight);
    layout->addWidget(max, 0, Qt::AlignRight);
    layout->addWidget(close, 0, Qt::AlignRight);
    mainLayout->addLayout(layout, Qt::AlignRight);
}


// Frameless windows implement

void MainWindow::maximizeWindow() {
    if(isMaximized()) {
        showNormal();
    } else {
        setWindowState(Qt::WindowMaximized);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isMouseDown) {
        QPoint movePoint;
        mousePoint = event->globalPos();
        movePoint.setX(mousePoint.x() - mouseOffsetX);
        movePoint.setY(mousePoint.y() - mouseOffsetY);
        move(movePoint);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        isMouseDown = true;
        mouseOffsetX = event->globalPos().x() - this->x();
        mouseOffsetY = event->globalPos().y() - this->y();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        isMouseDown = false;
    }
}
