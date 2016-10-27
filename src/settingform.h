#ifndef SETTINGFORM_H
#define SETTINGFORM_H

#include <QWidget>

namespace Ui {
    class SettingForm;
}

class SettingForm : public QWidget {
    Q_OBJECT
public:
    SettingForm(QWidget *parent = 0);
    ~SettingForm();

    bool isRelief()const; //是否是阳文（凸雕）  intaglio(阴，凹雕）
    bool isMsbSave()const; //是否按MSB方式存储
    bool isHorMirrored()const; //是否水平镜像
    bool isVerMirrored()const; //是否竖直镜像

protected:
    void changeEvent(QEvent *e);
    void initForm();

private:
    Ui::SettingForm *ui;

private slots:
    void on_checkBox_VerFirst_clicked(bool checked);
    void on_checkBox_HorFirst_clicked(bool checked);
    void ucharValues(int lineByte, QByteArray bytes);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_checkBox_ShowChar_clicked(bool checked);
    void on_mainChar_textChanged(QString );
    void on_griddingNumber_valueChanged(int );
    void on_radioButton_CharT0_clicked(bool checked);
    void on_radioButton_CharT1_clicked(bool checked);
    void on_radioButton_LSB_clicked(bool checked);
    void on_radioButton_MSB_clicked(bool checked);
};

#endif // SETTINGFORM_H
