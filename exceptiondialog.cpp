#include "exceptiondialog.h"
#include "ui_exceptiondialog.h"
#include <QRegExp>
#include <QRegExpValidator>

ExceptionDialog::ExceptionDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ExceptionDialog)
{
    m_ui->setupUi(this);

    QRegExp exp("[0-9A-Fa-f]{0,4}");
    m_ui->lineEditFrom->setValidator(new QRegExpValidator(exp, this));
    m_ui->lineEditTo->setValidator(new QRegExpValidator(exp, this));

    setWindowTitle(tr("Exception Range"));
}

ExceptionDialog::~ExceptionDialog()
{
    delete m_ui;
}

void ExceptionDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

ushort ExceptionDialog::startValue() const
{
    QString str = m_ui->lineEditFrom->text();
    return str.toUShort(NULL, 16);
}

ushort ExceptionDialog::endValue()const
{
    QString str = m_ui->lineEditTo->text();
    return str.toUShort(NULL, 16);
}

void ExceptionDialog::initValue(ushort start, ushort end)
{
    m_ui->lineEditFrom->setText(QString::number(start, 16));
    m_ui->lineEditTo->setText(QString::number(end, 16));
}
