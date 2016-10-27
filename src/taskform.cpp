#include "settingform.h"
#include "taskform.h"
#include "ui_taskform.h"
#include <QDir>
#include <QDebug>
#include <QButtonGroup>
#include <QRegExp>
#include <QFile>
#include <QPainter>

TaskForm::TaskForm(SettingForm* settingForm, QWidget *parent) :
    QWidget(parent), _settingForm(settingForm),
    ui(new Ui::TaskForm)
{
    ui->setupUi(this);

    QString inputMakeStr = "\\0\\xHHHH - \\0\\xHHHH";
    ui->lineEdit_wordRange->setInputMask(inputMakeStr);

    QButtonGroup * radioBox = new QButtonGroup(this);
    radioBox->addButton(ui->radioButton_noTitle,  0);
    radioBox->addButton(ui->radioButton_lsbTitle, 1);
    radioBox->addButton(ui->radioButton_msbTitle, 2);
    connect(radioBox, SIGNAL(buttonClicked(int)), this, SLOT(setUseHeader(int)));

    ui->radioButton_noTitle->click();
}

TaskForm::~TaskForm()
{
    delete ui;
}

void TaskForm::changeEvent(QEvent *e)
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

//�Ƿ������ģ�͹��  intaglio(��������
bool TaskForm::isRelief()const
{
    return _settingForm->isRelief();
}
//�Ƿ�MSB��ʽ�洢
bool TaskForm::isMsbSave()const
{
    return _settingForm->isMsbSave();
}
//�Ƿ�ˮƽ����
bool TaskForm::isHorMirrored()const
{
    return _settingForm->isHorMirrored();
}
//�Ƿ���ֱ����
bool TaskForm::isVerMirrored()const
{
    return _settingForm->isVerMirrored();
}

//�趨ͷ������
void TaskForm::setUseHeader(int header)
{
    _useHeader = (enum UseHeader)header;
}

//���ַ�Χ
bool TaskForm::compileRange(ushort& begin, ushort& end)
{
    QString text = ui->lineEdit_wordRange->text();
    QStringList range = text.split(QRegExp("\\s*-\\s*"), QString::SkipEmptyParts);
    bool ok;
    ushort begin_ = range.value(0, QString()).toUShort(&ok, 16);
    if(!ok)
        return ok;

    ushort end_ = range.value(1, QString()).toUShort(&ok, 16);
    if(!ok)
        return ok;

    begin = begin_;
    end = end_;

    return ok;
}

//���� ch �� task������ʹ�õ�����
const QFont& TaskForm::fontInTask(ushort ch, const struct FontTask& task) const
{
    const QList<struct FontInfo> & exceptions = task.exceptions;

    QListIterator<struct FontInfo> iter(exceptions);
    iter.toBack();

    //���������в��ң��ҵ��ͷ��ظ����壬û���ҵ��ͷ��ػ�������
    while(iter.hasPrevious())
    {
        const struct FontInfo& fontInfo = iter.previous();
        if(fontInfo.start <= ch && fontInfo.end >= ch)
        {
            return fontInfo.font;
        }
    }

    return task.baseFont;
}

//��ʼ����
void TaskForm::on_pushButton_compiler_clicked()
{
    if(!QDir::current().mkdir("usrfont"))
    {
        if(!QDir::current().exists("usrfont"))
        {
            qDebug() << "mkdir <./userfont> failed.";
            return;
        }
    }

    QList<struct FontTask> fontTasks;
    ui->taskTree->saveFontTask(fontTasks);

    foreach(const struct FontTask& task, fontTasks)
    {
        if(!task.checked)
            continue;

        QString fileName = QString("usrfont/%1.xft").arg(task.taskName);
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly))
        {
            qDebug() << "open file :" << fileName << " failed.";
        }

        if(!taskCompile(task, file))
        {
            qDebug() << "compile task:" << task.taskName << " failed.";
        }
    }
}

//�����񴴽�
bool TaskForm::taskCompile(const struct FontTask& task, QIODevice& ioDevice)
{
    ushort beginCh, endCh;
    if(!compileRange(beginCh, endCh))
    {
        qDebug() << "Error Range.";
        return false;
    }

    QDataStream ioStream(&ioDevice);
    if(ui->radioButton_msbTitle->isChecked()) // ���д��unicode�룬�������Ҫ¼��ʱ�����ᱻʹ��
        ioStream.setByteOrder(QDataStream::BigEndian);
    else
        ioStream.setByteOrder(QDataStream::LittleEndian);

    if(beginCh > endCh)
    {
        //��С��������
        qSwap(beginCh, endCh);
    }

    QImage canvas;
    if(isMsbSave())
        canvas = QImage(task.canvasSize, task.canvasSize, QImage::Format_Mono);
    else
        canvas = QImage(task.canvasSize, task.canvasSize, QImage::Format_MonoLSB);

    QPainter paintCanvas(&canvas);
    paintCanvas.setPen(Qt::white); //���ð�ɫ����

    ushort preCh = beginCh;
    bool horMirrored = isHorMirrored();
    bool verMirrored = isVerMirrored();
    for(ushort ch=beginCh; ch<=endCh; ch++)
    {
        if(ch < preCh)  // ��ֹ��ѭ�� 0xFFFF ++ --> 0x0000
            break;
        else
            preCh = ch;
        if(!ui->radioButton_noTitle->isChecked())
        {
            ioStream << ch;
        }

        const QFont& chFont_ = fontInTask(ch, task);
        QFont chFont(chFont_);
        chFont.setPixelSize(chFont_.pointSize());

        paintCanvas.setFont(chFont);
        paintCanvas.fillRect(canvas.rect(), Qt::black);//���ú�ɫ����
        paintCanvas.drawText(canvas.rect(), Qt::AlignCenter, QChar(ch));

        if(horMirrored || verMirrored)
        {
            canvas = canvas.mirrored(horMirrored, verMirrored);
        }

        int useBytes = (canvas.width() + 7) / 8;  // ����洢�õ����ֽ���
        QByteArray bytes;
        for(int line=0; line<canvas.height(); ++line)
        {
            const uchar* pLineBits = canvas.scanLine(line);
            for(int i=0; i<useBytes; i++)
            {
                uchar ch = *(pLineBits+i);
                if(!isRelief())
                {
                    ch ^= 0xFF;
                }
                bytes.append(ch);
            }
        }

        //����д���ļ�
        ioDevice.write(bytes);
    }

    return true;
}
