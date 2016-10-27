#ifndef TASKFORM_H
#define TASKFORM_H

#include <QWidget>
class SettingForm;

namespace Ui {
    class TaskForm;
}

class TaskForm : public QWidget {
    Q_OBJECT
public:
    enum UseHeader{NOHEADER, LSBHEADER, MSBHEADER};
    TaskForm(SettingForm* settingForm, QWidget *parent = 0);
    ~TaskForm();

protected:
    bool isRelief()const; //�Ƿ������ģ�͹��  intaglio(��������
    bool isMsbSave()const; //�Ƿ�MSB��ʽ�洢
    bool isHorMirrored()const; //�Ƿ�ˮƽ����
    bool isVerMirrored()const; //�Ƿ���ֱ����
    void changeEvent(QEvent *e);
    bool compileRange(ushort& begin, ushort& end);
    const QFont& fontInTask(ushort ch, const struct FontTask& task) const;
    bool taskCompile(const struct FontTask& task, QIODevice& ioDevice);

private:
    const SettingForm* _settingForm;
    UseHeader _useHeader;
    Ui::TaskForm *ui;

private slots:
    void setUseHeader(int);
    void on_pushButton_compiler_clicked();
};

#endif // TASKFORM_H
