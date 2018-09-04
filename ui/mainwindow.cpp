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
    program = new Program(this);
    tool = new Tool(this);
    centerWidget = new QWidget;
    mainLayout = new QVBoxLayout;
}

void MainWindow::createContent()
{
    setWindowTitle("acitdev tool kit");
}

void MainWindow::createLayout()
{
    tabWidget->addTab(setting, tr("Setting"));
    tabWidget->addTab(develop, tr("Develop"));
    tabWidget->addTab(program, tr("Program"));
    tabWidget->addTab(tool, tr("Tool"));

    tabWidget->setCurrentWidget(develop);


    mainLayout->addWidget(tabWidget);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);

    QFont font("Times New Roman", 10);
    font.setStyleHint(QFont::Monospace);
    qApp->setFont(font);

    setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
    setMinimumSize(1224, 768);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());
    //showMaximized();
}



void MainWindow::createConnection()
{

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

