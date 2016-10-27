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

//是否是阳文（凸雕）  intaglio(阴，凹雕）
bool TaskForm::isRelief()const
{
    return _settingForm->isRelief();
}
//是否按MSB方式存储
bool TaskForm::isMsbSave()const
{
    return _settingForm->isMsbSave();
}
//是否水平镜像
bool TaskForm::isHorMirrored()const
{
    return _settingForm->isHorMirrored();
}
//是否竖直镜像
bool TaskForm::isVerMirrored()const
{
    return _settingForm->isVerMirrored();
}

//设定头部类型
void TaskForm::setUseHeader(int header)
{
    _useHeader = (enum UseHeader)header;
}

//文字范围
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

//查找 ch 在 task任务中使用的字体
const QFont& TaskForm::fontInTask(ushort ch, const struct FontTask& task) const
{
    const QList<struct FontInfo> & exceptions = task.exceptions;

    QListIterator<struct FontInfo> iter(exceptions);
    iter.toBack();

    //先在例外中查找，找到就返回该字体，没有找到就返回基础字体
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

//开始创建
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

//按任务创建
bool TaskForm::taskCompile(const struct FontTask& task, QIODevice& ioDevice)
{
    ushort beginCh, endCh;
    if(!compileRange(beginCh, endCh))
    {
        qDebug() << "Error Range.";
        return false;
    }

    QDataStream ioStream(&ioDevice);
    if(ui->radioButton_msbTitle->isChecked()) // 大端写入unicode码，如果不需要录入时，不会被使用
        ioStream.setByteOrder(QDataStream::BigEndian);
    else
        ioStream.setByteOrder(QDataStream::LittleEndian);

    if(beginCh > endCh)
    {
        //从小到大排列
        qSwap(beginCh, endCh);
    }

    QImage canvas;
    if(isMsbSave())
        canvas = QImage(task.canvasSize, task.canvasSize, QImage::Format_Mono);
    else
        canvas = QImage(task.canvasSize, task.canvasSize, QImage::Format_MonoLSB);

    QPainter paintCanvas(&canvas);
    paintCanvas.setPen(Qt::white); //设置白色画笔

    ushort preCh = beginCh;
    bool horMirrored = isHorMirrored();
    bool verMirrored = isVerMirrored();
    for(ushort ch=beginCh; ch<=endCh; ch++)
    {
        if(ch < preCh)  // 防止死循环 0xFFFF ++ --> 0x0000
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
        paintCanvas.fillRect(canvas.rect(), Qt::black);//设置黑色背景
        paintCanvas.drawText(canvas.rect(), Qt::AlignCenter, QChar(ch));

        if(horMirrored || verMirrored)
        {
            canvas = canvas.mirrored(horMirrored, verMirrored);
        }

        int useBytes = (canvas.width() + 7) / 8;  // 计算存储用到的字节数
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

        //数据写入文件
        ioDevice.write(bytes);
    }

    return true;
}
