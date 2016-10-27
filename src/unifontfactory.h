#ifndef UNIFONTFACTORY_H
#define UNIFONTFACTORY_H

#include <QMainWindow>
#include "settingform.h"
#include "taskform.h"

namespace Ui {
    class UniFontFactory;
}

class UniFontFactory : public QMainWindow {
    Q_OBJECT
public:
    UniFontFactory(QWidget *parent = 0);
    ~UniFontFactory();

protected:

    void changeEvent(QEvent *e);

private:
    TaskForm* _taskForm;
    SettingForm* _settingForm;
    Ui::UniFontFactory *ui;

private slots:
};

#endif // UNIFONTFACTORY_H
