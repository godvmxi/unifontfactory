#include "unifontfactory.h"
#include "ui_unifontfactory.h"


UniFontFactory::UniFontFactory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UniFontFactory)
{
    ui->setupUi(this);
    ui->mainTab->clear();

    _settingForm = new SettingForm(this);
    ui->mainTab->addTab(_settingForm, tr("Setting"));

    _taskForm = new TaskForm(_settingForm, this);
    ui->mainTab->addTab(_taskForm, tr("Task"));

    statusBar()->showMessage("wangxinus<wangxinus@gmail.com> 2010-01");
}

UniFontFactory::~UniFontFactory()
{
    delete ui;
}

void UniFontFactory::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
