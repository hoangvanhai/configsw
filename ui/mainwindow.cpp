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
    app::config::instance()->open_file("setup.ini");
    app::config::instance()->load_config_all();
}

void MainWindow::createElement()
{
    tabWidget = new QTabWidget(this);
    setting = new Setting(this);
    develop = new Develop(this);    
    editor = new ChargEditor(this);
    centerWidget = new QWidget;
    mainLayout = new QVBoxLayout;
    comTheme = new QComboBox(this);
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

    comTheme->addItem("Light", QChart::ChartThemeLight);
    comTheme->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    comTheme->addItem("Dark", QChart::ChartThemeDark);
    comTheme->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    comTheme->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    comTheme->addItem("High Contrast", QChart::ChartThemeHighContrast);
    comTheme->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    comTheme->addItem("Qt", QChart::ChartThemeQt);    
    comTheme->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(new QLabel(tr("BATTERY CHARGER MANAGERMENT")));
    layout->addStretch(1);
    layout->addWidget(new QLabel(tr("Theme")));
    layout->addWidget(comTheme, 0);
    layout->addWidget(min, 0);
    layout->addWidget(max, 0);
    layout->addWidget(close, 0);


    tabWidget->addTab(setting, tr("SETTING"));    
    tabWidget->addTab(editor, tr("EDITOR"));
    tabWidget->addTab(develop, tr("MONITOR"));
    tabWidget->setCurrentWidget(editor);

    QString wgstyle = QString( "QTabBar::tab { height: 32px; width: 180 px;}" );
    tabWidget->setStyleSheet( wgstyle );

    mainLayout->addLayout(layout);
    mainLayout->addWidget(tabWidget);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);

    setWindowIcon(QIcon(":/icon/app.ico"));
    setMinimumSize(1224, 768);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());
    //showMaximized();
}



void MainWindow::createConnection()
{
    connect(min, SIGNAL(clicked(bool)), this, SLOT(minimizeWindow()));
    connect(max, SIGNAL(clicked(bool)), this, SLOT(maximizeWindow()));
    connect(close, SIGNAL(clicked(bool)), this, SLOT(closeWindow()));

    connect(comTheme, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTheme()));
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
    min->setToolTip(tr("minimize window"));
    max->setIcon(maxIcon);
    min->setToolTip(tr("maximize/restore window"));
    close->setIcon(closeIcon);
    min->setToolTip(tr("close window"));


    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(min, 0, Qt::AlignRight);
    layout->addWidget(max, 0, Qt::AlignRight);
    layout->addWidget(close, 0, Qt::AlignRight);
    mainLayout->addLayout(layout, Qt::AlignRight);
}

void MainWindow::updateTheme()
{
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                comTheme->itemData(comTheme->currentIndex()).toInt());
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        qApp->setPalette(pal);
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
