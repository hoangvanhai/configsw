#include "permission.h"
#include "config/config.h"

Permission::Permission(QWidget *parent) : QWidget(parent)
{
    createUiElem();
    createConnection();
}

void Permission::createUiElem()
{
    app::appsetting setting = app::config::instance()->get_app_setting();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QPushButton *btnReset =  new QPushButton("Factory reset");
    listTheme = new QComboBox;
    connect(listTheme, SIGNAL(activated(int)), this, SLOT(updateAppTheme()));
    {
    QGroupBox *groupFactory = new QGroupBox("General setting");
    QHBoxLayout *hLayout = new QHBoxLayout;
    listTheme->addItems(QStringList() << "dark" << "white");
    hLayout->addWidget(btnReset, 0, Qt::AlignLeft);
    hLayout->addWidget(listTheme, 0, Qt::AlignLeft);

    groupFactory->setLayout(hLayout);
    mainLayout->addWidget(groupFactory, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    {
    QGroupBox *groupAdmin = new QGroupBox("Administractor");
    QHBoxLayout *hLayout = new QHBoxLayout;
    QPushButton *btnAdmin = new QPushButton("Login as developer");
    QLineEdit *editPasswd = new QLineEdit;
    hLayout->addWidget(editPasswd, 0, Qt::AlignLeft);
    hLayout->addWidget(btnAdmin, 0, Qt::AlignLeft);
    groupAdmin->setLayout(hLayout);
    mainLayout->addWidget(groupAdmin, 1, Qt::AlignTop | Qt::AlignLeft);
    }

    listTheme->setCurrentText(setting.palette);
    updateAppTheme();
}

void Permission::createConnection()
{

}

void Permission::updateAppTheme()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    setting.palette = listTheme->currentText();
    app::config::instance()->save_config_all(setting);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if(listTheme->currentText() == "dark") {
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,          QColor(47, 47, 47));
        darkPalette.setColor(QPalette::WindowText,      QColor(231, 231, 231));
        darkPalette.setColor(QPalette::Base,            QColor(81,81,81));
        darkPalette.setColor(QPalette::AlternateBase,   QColor(68,68,68));
        darkPalette.setColor(QPalette::ToolTipBase,     QColor(47, 47, 47));
        darkPalette.setColor(QPalette::ToolTipText,     QColor(231, 231, 231));
        darkPalette.setColor(QPalette::Text,            QColor(231, 231, 231));
        darkPalette.setColor(QPalette::Button,          QColor(61, 61, 61));
        darkPalette.setColor(QPalette::ButtonText,      QColor(231, 231, 231));
        darkPalette.setColor(QPalette::BrightText,      Qt::red);
        darkPalette.setColor(QPalette::Link,            QColor(127, 194, 65));
        darkPalette.setColor(QPalette::Highlight,       QColor(127, 194, 65));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled,        QPalette::Button, QColor(88, 88, 88));
        qApp->setPalette(darkPalette);
        std::cout << "curr text = dark\n";
    } else if(listTheme->currentText() == "white"){
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,          Qt::lightGray);
        darkPalette.setColor(QPalette::WindowText,      Qt::black);
        darkPalette.setColor(QPalette::Base,            Qt::gray);
        darkPalette.setColor(QPalette::AlternateBase,   Qt::white);
        darkPalette.setColor(QPalette::ToolTipText,     Qt::black);
        darkPalette.setColor(QPalette::Text,            Qt::black);
        darkPalette.setColor(QPalette::Button,          Qt::white);
        darkPalette.setColor(QPalette::ButtonText,      Qt::black);
        darkPalette.setColor(QPalette::HighlightedText, Qt::red);
        darkPalette.setColor(QPalette::BrightText,      Qt::red);
        qApp->setPalette(darkPalette);
        std::cout << "curr text = white\n";
    }

    QFont font("Times New Roman", 12);
    font.setStyleHint(QFont::Monospace);
    qApp->setFont(font);

    fflush(stdout);
}
