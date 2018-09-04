#ifndef DEVELOP_H
#define DEVELOP_H

#include <QtWidgets>
#include <xlsx/xlsxdocument.h>
#include <layer2.h>
#include <config/config.h>

class StreamDock;
class DebugDock;

class Develop : public QMainWindow
{
    Q_OBJECT
public:
    explicit Develop(QWidget *parent = 0);
    ~Develop();
    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();
    void createTableViewHeader();
    void initExcelHeader();
    void appendRow(QStringList &row);
    void writeRow(QStringList &row);    
signals:
    void signalStreamEvent(int event);
    void signalStreamData(uint8_t *data_,
                         int data_len_);
    void signalDebugEvent(int event);
    void signalDebugData(uint8_t *data,
                         int data_len);
private slots:
    void openDebug();
    void closeDebug();
    void openStream();
    void closeStream();
    void showDebug();
    void showStream();
    void clearMessage();
    void saveMessage();
    void sendDebugData(QString);
    void recvStreamDataEvent(uint8_t *data_,
                       int data_len_);

    void recvStreamEvent(int event);
    void recvDebugDataEvent(uint8_t *data_,
                       int data_len_);

    void recvDebugEvent(int event);

private:

    QToolBar *toolBar;
    QHBoxLayout *mainLayout;
    QWidget *centerWidget;

    QTableView *tableView;
    QStandardItemModel *model;


    QToolButton *btnOpenDebug, *btnCloseDebug, *btnShowDebug,
    *btnOpenStream, *btnCloseStream, *btnShowStream, *btnClearStream,
    *btnSaveStream;

    DebugDock *debug;
    StreamDock *stream;

    std::shared_ptr<ibc::layer2> ibc_stream_;
    std::shared_ptr<ibc::layer2> ibc_debug_;

    QXlsx::Document *msgFile;
    uint32_t        current_row;
};



class DebugDock : public QDockWidget {
    Q_OBJECT

    QCheckBox *checkProDebug;
    QToolButton *btnClear, *btnSave;
    QPlainTextEdit *peditScreen;
    QTextEdit   *editScreen;

    std::shared_ptr<ibc::layer2> ibc_debug_;

private slots:
    void clearDebugScreen();
    void saveDebugScreen();

public slots:
    void recvUpdateDebug(uint8_t *data, int len);
public:
    explicit DebugDock(const QString &name, QWidget *parent = 0);

    void createElement();
    void createConnection();
};


class StreamDock : public QDockWidget {
    Q_OBJECT

    QComboBox *comSrc, *comDst;
    QLineEdit *editCtrl, *editData;
    QCheckBox *checkRepeat;
    QSpinBox *spinDelay;

    QPushButton *btnSend;
    QTimer      *timer;

    std::shared_ptr<ibc::layer2> ibc_stream_;

private slots:
    void updateEditData();
    void onSendRepeat();
    void onSendOneTime();
    void sendData();
    void updateDelay();


public:
    explicit StreamDock(const QString &name, QWidget *parent = 0);
    ~StreamDock() {
        timer->stop();
    }

    void createElement();
    void setPanelEnable(bool en);
    void createConnection();

    void set_ibc_object(const std::shared_ptr<ibc::layer2> &ibc) {
        ibc_stream_ = ibc;
    }
};


#endif // DEVELOP_H
