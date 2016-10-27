#include "settingform.h"
#include "ui_settingform.h"
#include <QDebug>

SettingForm::SettingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingForm)
{
    ui->setupUi(this);
    initForm();
    connect(ui->charWidget, SIGNAL(ucharValues(int,QByteArray)),
            this, SLOT(ucharValues(int,QByteArray)));
}

SettingForm::~SettingForm()
{
    delete ui;
}

void SettingForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingForm::initForm()
{
    QButtonGroup* btnGroup1 = new QButtonGroup(this);
    btnGroup1->addButton(ui->radioButton_LSB);
    btnGroup1->addButton(ui->radioButton_MSB);
    QButtonGroup* btnGroup2 = new QButtonGroup(this);
    btnGroup2->addButton(ui->radioButton_CharT0);
    btnGroup2->addButton(ui->radioButton_CharT1);

    ui->checkBox_HorFirst->setChecked(false);
    ui->checkBox_VerFirst->setChecked(false);

    ui->radioButton_CharT1->setChecked(ui->charWidget->charBitFace() == CharWidget::LightOnHigh);
    ui->radioButton_CharT0->setChecked(ui->charWidget->charBitFace() == CharWidget::LightOnLow);

    ui->griddingNumber->setValue(ui->charWidget->gridNumber());
    ui->griddingNumber->setMaximum(64);
    ui->griddingNumber->setMinimum(8);

    ui->checkBox_ShowChar->setChecked(ui->charWidget->charShow());
    ui->fontComboBox->setCurrentFont(QFont(ui->charWidget->baseFontFamily()));

    ui->radioButton_MSB->setChecked(!ui->charWidget->byteLSB());
    ui->radioButton_LSB->setChecked(ui->charWidget->byteLSB());
}


void SettingForm::on_radioButton_MSB_clicked(bool checked)
{
    ui->charWidget->setByteLSB(!checked);
}

void SettingForm::on_radioButton_LSB_clicked(bool checked)
{
    ui->charWidget->setByteLSB(checked);
}

void SettingForm::on_radioButton_CharT1_clicked(bool checked)
{
    if(checked)
        ui->charWidget->setCharBitFace(CharWidget::LightOnHigh);
}

void SettingForm::on_radioButton_CharT0_clicked(bool checked)
{
    if(checked)
        ui->charWidget->setCharBitFace(CharWidget::LightOnLow);
}

void SettingForm::on_griddingNumber_valueChanged(int v)
{
    ui->charWidget->setGridNumber(v);
}

void SettingForm::on_mainChar_textChanged(QString  s)
{
    s.append(QChar());
    ui->charWidget->setMainChar(s.at(0));
}

void SettingForm::on_checkBox_ShowChar_clicked(bool checked)
{
    ui->charWidget->setCharShow(checked);
}

void SettingForm::on_fontComboBox_currentFontChanged(QFont f)
{
    ui->charWidget->setBaseFontFamily(f.family());
}

void SettingForm::ucharValues(int lineByte, QByteArray bytes)
{
    QChar ch = ui->charWidget->mainChar();
    //生成注释
    QString text = QString("//*Uni-font *- %1(0x%2) -*\n")
                   .arg(ch).arg(QString::number(ch.unicode(),16));

    //生成语句
    text.append("static unsigned char uniCh_0x");
    text.append(QString::number(ch.unicode(),16));
    text.append("[] = {");
    for(int i=0; i<bytes.count(); i++)
    {
        if(i % lineByte == 0)
        {
            text.append("\n");
        }
        text.append("0x");
        text.append(QString::number((quint8)bytes.at(i), 16));
        text.append(",");
    }
    text.append("\n};");

    ui->textEdit->clear();
    ui->textEdit->setPlainText(text);
}

void SettingForm::on_checkBox_HorFirst_clicked(bool checked)
{
    ui->charWidget->setHorMirrored(checked);
}

void SettingForm::on_checkBox_VerFirst_clicked(bool checked)
{
    ui->charWidget->setVerMirrored(checked);
}


//是否是阳文（凸雕）  intaglio(阴，凹雕）
bool SettingForm::isRelief()const
{
    return ui->radioButton_CharT1->isChecked();
}
//是否按MSB方式存储
bool SettingForm::isMsbSave()const
{
    return ui->radioButton_MSB->isChecked();
}
//是否水平镜像
bool SettingForm::isHorMirrored()const
{
    return ui->checkBox_HorFirst->isChecked();
}
//是否竖直镜像
bool SettingForm::isVerMirrored()const
{
    return ui->checkBox_VerFirst->isChecked();
}
