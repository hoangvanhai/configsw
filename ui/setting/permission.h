#ifndef PERMISSION_H
#define PERMISSION_H

#include <QtWidgets>

class Permission : public QWidget
{
    Q_OBJECT
public:
    explicit Permission(QWidget *parent = 0);
    void createUiElem();
    void createConnection();
signals:

public slots:
    void updateAppTheme();

private:
    QComboBox *listTheme;
};

#endif // PERMISSION_H
