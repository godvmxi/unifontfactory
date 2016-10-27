#include "TaskTreeWidget.h"
#include "main.h"
#include <QTreeWidgetItem>
#include <QDebug>
#include <QInputDialog>
#include <QStringList>
#include <QFontDialog>
#include "exceptiondialog.h"
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

QString TaskTreeWidget::_this_config_file = __THIS_PROGRAM__".xcfg";

TaskTreeWidget::TaskTreeWidget(QWidget *parent)
        :QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Task Manager") << "Values";
    setHeaderLabels(labels);
    this->setColumnWidth(0, 200);

    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(on_itemDoubleClicked(QTreeWidgetItem*,int)));

    createActions();

    _validTaskCount = 0;

    if(readXcfgFile())
        loadFontTasks(_tasks);
}

TaskTreeWidget::~TaskTreeWidget()
{
    saveFontTask(_tasks);
    saveXcfgFile();
}

void TaskTreeWidget::on_itemDoubleClicked(QTreeWidgetItem* item, int col)
{
    int type = item->data(col, Qt::UserRole).toInt();
    switch(type)
    {
        //ITEM_TASK
        case ITEM_TASK:
        {
            if(col == 1)
            {
                bool ok;
                QString rename = QInputDialog::getText(this, tr("Rename Task"), tr("caption"),
                                                       QLineEdit::Normal, item->text(0), &ok);
                if(ok)
                    item->setText(0, rename);
            }
        }
        break;
        //ITEM_CANVAS
        case ITEM_CANVAS:
        {
            bool ok;
            int va = QInputDialog::getInt(this, tr("Canvas Size"), tr("Pixels"),
                                          item->text(1).toInt(), 8, 32, 1, &ok);
            if(ok)
                item->setText(1, QString::number(va));
        }
        break;
        //ITEM_BASEFONT
        case ITEM_BASEFONT:
        {
            bool ok;
            QString txt = item->text(1);
            QStringList ll = txt.split(',');
            QFont initFont(ll.value(0));
            initFont.setPointSize(ll.value(1).toInt());
            QFont ft = QFontDialog::getFont(&ok, initFont,this, tr("Base Font"));
            if(ok)
            {
                item->setText(1, QString("%1,%2").arg(ft.family())
                              .arg(ft.pointSize()));
            }
        }
        break;
        //ITEM_EXCEPTION_DIR
        case ITEM_EXCEPTION_DIR:
        {
        }
        break;
        //ITEM_EXCEPTION
        case ITEM_EXCEPTION:
        {
            if(col==0)
            {
                QStringList lss = item->text(0).split('-');
                ushort initStart = lss.value(0).toUShort(NULL, 16);
                ushort initEnd = lss.value(1).toUShort(NULL, 16);
                ExceptionDialog expDlg;
                expDlg.initValue(initStart, initEnd);
                if(expDlg.exec())
                {
                    ushort start = expDlg.startValue();
                    ushort end   = expDlg.endValue();
                    item->setText(0, QString("0x%1-0x%2")
                                     .arg(start, 1, 16)
                                     .arg(end, 1, 16));

                }
            }
            else if(col==1)
            {
                bool ok;
                QString txt = item->text(1);
                QStringList ll = txt.split(',');
                QFont initFont(ll.value(0));
                initFont.setPointSize(ll.value(1).toInt());
                QFont ft = QFontDialog::getFont(&ok, initFont,this, tr("Exception Font"));
                if(ok)
                {
                    item->setText(1, QString("%1,%2").arg(ft.family())
                                  .arg(ft.pointSize()));
                }
            }
        }
        break;
        //default
        default:
        break;
    }
}

int TaskTreeWidget::loadFontTasks(const QList<struct FontTask>& tasks)
{
    this->clear();
    foreach(const struct FontTask& task, tasks)
    {
        QTreeWidgetItem* taskItem = createItemTask(task.taskName, task.checked);
        createItemCanvas(taskItem, task.canvasSize);
        createItemBaseFont(taskItem, task.baseFont);
        QTreeWidgetItem* exps = createItemExceptionDir(taskItem, task.exceptions.count());
        //exps
        foreach(const struct FontInfo& fontInfo, task.exceptions)
        {
            createItemException(exps, fontInfo);
        }

    }
    return 0;
}

void TaskTreeWidget::saveFontTask(QList<struct FontTask>& tasks)
{
    tasks.clear();
    for(int top=0; top<topLevelItemCount(); top++)
    {
        struct FontTask task;

        QTreeWidgetItem* taskItem = topLevelItem(top);
        if(taskItem == NULL || taskItem->isHidden())
            continue;

        task.taskName = taskItem->text(0);
        task.checked  = (taskItem->checkState(0) == Qt::Checked);
        for(int sub=0; sub<taskItem->childCount(); sub++)
        {
            QTreeWidgetItem* subItem = taskItem->child(sub);
            int subItemType = subItem->data(0, Qt::UserRole).toInt();
            //enum{ITEM_TASK, ITEM_CANVAS, ITEM_BASEFONT, ITEM_EXCEPTION_DIR, ITEM_EXCEPTION};
            switch(subItemType)
            {
                case ITEM_CANVAS:
                task.canvasSize = subItem->text(1).toInt(NULL, 10);
                break;
                case ITEM_BASEFONT:
                {
                    QString text = subItem->text(1);
                    QStringList strList = text.split(',');
                    QFont ft(strList.value(0));
                    ft.setPointSize(strList.value(1, QString::number(task.canvasSize, 10)).toInt());
                    task.baseFont = ft;
                }
                break;
                case ITEM_EXCEPTION_DIR:
                {
                    for(int i=0; i<subItem->childCount(); i++)
                    {
                        QTreeWidgetItem* expItem = subItem->child(i);
                        if(expItem != NULL)
                        {
                            struct FontInfo ftInfo;
                            QString t1 = expItem->text(0);
                            QString t2 = expItem->text(1);

                            QStringList l1 = t1.split('-');
                            QStringList l2 = t2.split(',');

                            ftInfo.start = l1.value(0).toInt(NULL, 16);
                            ftInfo.end = l1.value(1).toInt(NULL, 16);
                            ftInfo.font = QFont(l2.value(0));
                            ftInfo.font.setPointSize(l2.value(1).toInt(NULL, 10));
                            task.exceptions.append(ftInfo);
                        }
                    }
                }
                break;
                default:
                break;
            }

        }//end of 'for sub'
        tasks.append(task);
    }//end of 'for top'
}

//创建任务条目
QTreeWidgetItem* TaskTreeWidget::createItemTask(const QString& taskName, bool checked)
{
    QTreeWidgetItem* task = new QTreeWidgetItem(this);
    task->setData(0, Qt::UserRole, ITEM_TASK); // col 0
    task->setData(1, Qt::UserRole, ITEM_TASK); // col 1
    task->setText(0, taskName);
    task->setText(1, tr("<Check to Rename>"));
    task->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
    return task;
}
//创建画布条目
QTreeWidgetItem* TaskTreeWidget::createItemCanvas(QTreeWidgetItem* task, int canvasSize)
{
    QTreeWidgetItem* canvas = new QTreeWidgetItem(task);
    canvas->setData(0, Qt::UserRole, ITEM_CANVAS); // col 0
    canvas->setData(1, Qt::UserRole, ITEM_CANVAS); // col 1
    canvas->setText(0, tr("Canvas Size"));
    canvas->setText(1, QString::number(canvasSize));
    return canvas;
}
//创建基础字体条目
QTreeWidgetItem* TaskTreeWidget::createItemBaseFont(QTreeWidgetItem* task, const QFont& font)
{
    QTreeWidgetItem* baseFont = new QTreeWidgetItem(task);
    baseFont->setData(0, Qt::UserRole, ITEM_BASEFONT);// col 0
    baseFont->setData(1, Qt::UserRole, ITEM_BASEFONT);// col 1
    baseFont->setText(0, tr("Base Font"));
    baseFont->setText(1, QString("%1,%2").arg(font.family()).arg(font.pointSize()));
    return baseFont;
}
//创建例外文件夹条目
QTreeWidgetItem* TaskTreeWidget::createItemExceptionDir(QTreeWidgetItem* task, int count)
{
    QTreeWidgetItem* exps = new QTreeWidgetItem(task);
    exps->setData(0, Qt::UserRole, ITEM_EXCEPTION_DIR); // col 0
    exps->setData(1, Qt::UserRole, ITEM_EXCEPTION_DIR); // col 1
    exps->setText(0, tr("Exceptions"));
    exps->setText(1, QString::number(count));
    return exps;
}
//创建例外条目
QTreeWidgetItem* TaskTreeWidget::createItemException(QTreeWidgetItem* exps, const struct FontInfo& fontInfo)
{
    QTreeWidgetItem* exp = new QTreeWidgetItem(exps);
    exp->setData(0, Qt::UserRole, ITEM_EXCEPTION); // col 0
    exp->setData(1, Qt::UserRole, ITEM_EXCEPTION); // col 1
    exp->setText(0, QString("0x%1-0x%2").arg(QString::number(fontInfo.start, 16))
                 .arg(QString::number(fontInfo.end, 16)));
    exp->setText(1, QString("%1,%2").arg(fontInfo.font.family()).arg(fontInfo.font.pointSize()));
    exps->setText(1, QString::number(exps->childCount(), 10));
    return exp;
}

//响应右键菜单
void TaskTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QTreeWidgetItem* item = this->itemAt(event->pos());
    QMenu* popMenu = new QMenu(this);    
    if(item != NULL)
    {
        int itemType = item->data(0, Qt::UserRole).toInt();
        //enum{ITEM_TASK, ITEM_CANVAS, ITEM_BASEFONT, ITEM_EXCEPTION_DIR, ITEM_EXCEPTION};
        switch(itemType)
        {
            case ITEM_TASK:
                popMenu->addAction(_newTaskAction);
                popMenu->addAction(_deleteTaskAction);
            break;
            case ITEM_EXCEPTION_DIR:
                popMenu->addAction(_newExceptionAction);
            break;
            case ITEM_EXCEPTION:
                popMenu->addAction(_newExceptionAction);
                popMenu->addAction(_deleteExceptionAction);
                popMenu->addAction(_moveUpAction);
                popMenu->addAction(_moveDownAction);
            break;
            default:
            break;

        }
    }
    else
    {
        popMenu->addAction(_newTaskAction);
    }
    popMenu->exec(QCursor::pos());
}

//创建动作
void TaskTreeWidget::createActions()
{
    _newTaskAction = new QAction(tr("New Task"), this);
    connect(_newTaskAction, SIGNAL(triggered()), this, SLOT(on_newTaskAction()));

    _deleteTaskAction = new QAction(tr("Delete Task"), this);
    connect(_deleteTaskAction, SIGNAL(triggered()), this, SLOT(on_deleteTaskAction()));

    _newExceptionAction = new QAction(tr("New Exception"), this);
    connect(_newExceptionAction, SIGNAL(triggered()), this, SLOT(on_newExceptionAction()));

    _deleteExceptionAction = new QAction(tr("Delete Exception"), this);
    connect(_deleteExceptionAction, SIGNAL(triggered()), this, SLOT(on_deleteExceptionAction()));

    _moveUpAction = new QAction(tr("Move up"), this);
    connect(_moveUpAction, SIGNAL(triggered()), this, SLOT(on_moveUpAction()));

    _moveDownAction = new QAction(tr("Move down"), this);
    connect(_moveDownAction, SIGNAL(triggered()), this, SLOT(on_moveDownAction()));
}

//enum{ITEM_TASK, ITEM_CANVAS, ITEM_BASEFONT, ITEM_EXCEPTION_DIR, ITEM_EXCEPTION};
void TaskTreeWidget::on_newTaskAction()
{
    bool ok;
    QString taskName = QInputDialog::getText(this, tr("New Task"), tr("Task Name"),
                                             QLineEdit::Normal, "Task unname", &ok);
    if(!ok)
        return;

    QTreeWidgetItem* task = createItemTask(taskName, false);
    createItemCanvas(task, 12);
    createItemBaseFont(task, QFont());
    createItemExceptionDir(task, 0);
}

void TaskTreeWidget::on_deleteTaskAction()
{
    QTreeWidgetItem* item = this->currentItem();
    if(item == NULL)
        return;

    int itemType = item->data(0, Qt::UserRole).toInt();

    if(itemType == ITEM_TASK)
    {
        if(QMessageBox::Yes == QMessageBox::warning(this, tr("Delete"), tr("Are you sure?"),
                                                    QMessageBox::Yes, QMessageBox::No))
        {
            item->setHidden(true);
        }
    }
}
void TaskTreeWidget::on_newExceptionAction()
{
    QTreeWidgetItem* item = this->currentItem();
    if(item == NULL)
        return;

    QTreeWidgetItem* exps = item;
    struct FontInfo fontInfo;
    fontInfo.start = 0;
    fontInfo.end = 0;
    fontInfo.font.setPointSize(12);
    int itemType = item->data(0, Qt::UserRole).toInt();
    if(itemType == ITEM_EXCEPTION_DIR)
    {
        createItemException(exps, fontInfo);        
    }
    else if(itemType == ITEM_EXCEPTION)
    {
        exps = item->parent();
        int index = exps->indexOfChild(item);
        fontInfo.font = baseFont(exps->parent());
        QTreeWidgetItem* expItem = createItemException(exps, fontInfo);
        expItem = exps->takeChild(exps->indexOfChild(expItem));
        exps->insertChild(index, expItem);
    }
    else
        return;
    exps->setText(1, QString::number(exps->childCount(), 10));
}

void TaskTreeWidget::on_deleteExceptionAction()
{
    QTreeWidgetItem* item = this->currentItem();
    if(item == NULL)
        return;

    int itemType = item->data(0, Qt::UserRole).toInt();

    if(itemType == ITEM_EXCEPTION)
    {
        QTreeWidgetItem* exps = item->parent();
        if(exps == NULL)
            return;

        if(QMessageBox::Yes == QMessageBox::warning(this, tr("Delete"), tr("Are you sure?"),
                                                    QMessageBox::Yes, QMessageBox::No))
        {
            exps->removeChild(item);
        }
        exps->setText(1, QString::number(exps->childCount(), 10));
    }
}

void TaskTreeWidget::on_moveUpAction()
{
    QTreeWidgetItem* item = this->currentItem();
    if(item == NULL)
        return;

    int itemType = item->data(0, Qt::UserRole).toInt();
    if(itemType == ITEM_EXCEPTION)
    {
        QTreeWidgetItem* exps = item->parent();
        int index = exps->indexOfChild(item);
        if(index > 0)
        {
            item = exps->takeChild(index);
            exps->insertChild(index-1, item);
        }
    }
}

void TaskTreeWidget::on_moveDownAction()
{
    QTreeWidgetItem* item = this->currentItem();
    if(item == NULL)
        return;
    int itemType = item->data(0, Qt::UserRole).toInt();
    if(itemType == ITEM_EXCEPTION)
    {
        QTreeWidgetItem* exps = item->parent();
        int index = exps->indexOfChild(item);
        if(index < exps->childCount()-1)
        {
            item = exps->takeChild(index);
            exps->insertChild(index+1, item);
        }
    }
}


QFont TaskTreeWidget::baseFont(QTreeWidgetItem* task)
{
    int itemType = task->data(0, Qt::UserRole).toInt();
    if(itemType == ITEM_TASK)
    {
        for(int i=0; i<task->childCount(); i++)
        {
            QTreeWidgetItem* item = task->child(i);
            if(item->data(0, Qt::UserRole).toInt() == ITEM_BASEFONT)
            {
                QString text = item->text(1);
                QStringList strList = text.split(',');
                QFont ft(strList.value(0));
                ft.setPointSize(strList.value(1, "12").toInt());
                return ft;
            }
        }
    }

    return QFont();
}

//检查设置
int TaskTreeWidget::checkTaskSettings()
{
    saveFontTask(_tasks);

    int validTaskCount = 0;
    foreach(const struct FontTask& task, _tasks)
    {
        if(task.checked)
            ++validTaskCount;
    }
    _validTaskCount = validTaskCount;

    return _validTaskCount;
}

//保存配置文件
bool TaskTreeWidget::saveXcfgFile()
{
    //checkTaskSettings();

    QFile file(TaskTreeWidget::_this_config_file);
    if(!file.open(QFile::WriteOnly))
        return false;

    QXmlStreamWriter* writer = new QXmlStreamWriter(&file);
    writer->setCodec("utf-8");

    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE xcfg>");
    writer->writeStartElement(__THIS_PROGRAM__);
    writer->writeAttribute("version", __THIS_VERSION__);

    foreach(const struct FontTask& task, _tasks)
    {
        writer->writeStartElement("task");
        writer->writeAttribute("name", task.taskName);
        writer->writeAttribute("checked", task.checked ? "yes" : "no");
        writer->writeTextElement("canvas", QString::number(task.canvasSize));
        writer->writeTextElement("basefont", task.baseFont.toString());

        writer->writeStartElement("exceptions");
        writer->writeAttribute("count", QString::number(task.exceptions.count()));
        foreach(const struct FontInfo& info, task.exceptions)
        {
            writer->writeTextElement("exception", QString("%1;%2;%3").arg(info.start).arg(info.end).arg(info.font.toString()));
        }
        writer->writeEndElement();

        writer->writeEndElement();
    }

    writer->writeEndElement();
    writer->writeEndDocument();

    return true;
}

//读取配置文件
bool TaskTreeWidget::readXcfgFile()
{
    _tasks.clear();
    QFile file(TaskTreeWidget::_this_config_file);
    if(!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader* reader = new QXmlStreamReader(&file);
    while(!reader->atEnd())
    {
        reader->readNext();
        if(reader->isStartElement())
        {
            if(reader->name() == __THIS_PROGRAM__
               && reader->attributes().value("version") == __THIS_VERSION__)
            {
                while(!reader->atEnd())
                {
                    reader->readNext();
                    if(reader->isEndElement())
                        break;

                    if(reader->isStartElement())
                    {
                        if(reader->name() == "task")
                        {
                            struct FontTask task;
                            task.taskName = reader->attributes().value("name").toString();
                            task.checked  = (reader->attributes().value("checked").toString() == "yes");
                            while(!reader->atEnd())
                            {
                                reader->readNext();
                                if(reader->isEndElement())
                                    break;
                                if(reader->isStartElement())
                                {
                                    if(reader->name() == "canvas")
                                    {
                                        task.canvasSize = reader->readElementText().toInt();
                                    }
                                    else if(reader->name() == "basefont")
                                    {
                                        task.baseFont.fromString(reader->readElementText());
                                    }
                                    else if(reader->name() == "exceptions")
                                    {
                                        while(!reader->atEnd())
                                        {
                                            reader->readNext();
                                            if(reader->isEndElement())
                                                break;

                                            if(reader->name() == "exception")
                                            {
                                                struct FontInfo exception;
                                                QString text = reader->readElementText();
                                                QStringList ls  = text.split(';');
                                                exception.start = ls.value(0).toUShort();
                                                exception.end = ls.value(1).toUShort();
                                                exception.font.fromString(ls.value(2));
                                                task.exceptions.append(exception);
                                            }
                                        }

                                    }
                                }
                            }
                            _tasks.append(task);
                        }
                    }
                }
            }
            else
                reader->raiseError(QObject::tr("The file is not an %1 verion %2 file.")
                                   .arg(__THIS_PROGRAM__)
                                   .arg(__THIS_VERSION__));
        }
    }

    return true;
}
