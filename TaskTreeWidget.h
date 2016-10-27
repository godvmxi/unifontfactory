#ifndef TASKTREEWIDGET_H
#define TASKTREEWIDGET_H

#include <QTreeWidget>
#include <QContextMenuEvent>

struct FontInfo
{
    ushort start;
    ushort end;
    QFont  font;
};

struct FontTask
{
    QString taskName;
    int     canvasSize;
    QFont   baseFont;
    bool    checked;
    QList<struct FontInfo> exceptions;
};

class TaskTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TaskTreeWidget(QWidget *parent = 0);
    ~TaskTreeWidget();
    int loadFontTasks(const QList<struct FontTask>& tasks);
    void saveFontTask(QList<struct FontTask>& tasks);
    enum{ITEM_TASK, ITEM_CANVAS, ITEM_BASEFONT, ITEM_EXCEPTION_DIR, ITEM_EXCEPTION};
    bool saveXcfgFile();
    bool readXcfgFile();
    int checkTaskSettings();

protected:
    QTreeWidgetItem* createItemTask(const QString& taskName, bool checked);
    QTreeWidgetItem* createItemCanvas(QTreeWidgetItem* task, int canvasSize);
    QTreeWidgetItem* createItemBaseFont(QTreeWidgetItem* task, const QFont& font);
    QTreeWidgetItem* createItemExceptionDir(QTreeWidgetItem* task, int count);
    QTreeWidgetItem* createItemException(QTreeWidgetItem* exps, const struct FontInfo& fontInfo);
    void contextMenuEvent(QContextMenuEvent* );
    QFont baseFont(QTreeWidgetItem* task);

private slots:
    void on_itemDoubleClicked(QTreeWidgetItem*,int);
    void on_newTaskAction();
    void on_deleteTaskAction();
    void on_newExceptionAction();
    void on_deleteExceptionAction();
    void on_moveUpAction();
    void on_moveDownAction();

private:
    int _validTaskCount;
    QList<struct FontTask> _tasks;
    static QString _this_config_file;
    void createActions();
    QAction* _newTaskAction;
    QAction* _deleteTaskAction;
    QAction* _newExceptionAction;
    QAction* _deleteExceptionAction;
    QAction* _moveUpAction;
    QAction* _moveDownAction;
};

#endif // TASKTREEWIDGET_H
