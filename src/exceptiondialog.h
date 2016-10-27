#ifndef EXCEPTIONDIALOG_H
#define EXCEPTIONDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class ExceptionDialog;
}

class ExceptionDialog : public QDialog {
    Q_OBJECT
public:
    ExceptionDialog(QWidget *parent = 0);
    ~ExceptionDialog();

    void   initValue(ushort start, ushort end);
    ushort startValue() const;
    ushort endValue()const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ExceptionDialog *m_ui;
};

#endif // EXCEPTIONDIALOG_H
