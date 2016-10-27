#ifndef CHARWIDGET_H
#define CHARWIDGET_H

#include <QWidget>

class CharWidget : public QWidget
{
Q_OBJECT
public:
    explicit CharWidget(QWidget *parent = 0);

    enum ScanOrder{HorizontalFirst, VerticalFirst};
    enum HorizontalDirection{LeftToRight, RightToLeft};
    enum VerticalDirection{TopToBottom, BottomToTop};
    enum CharBitFace{LightOnHigh, LightOnLow};

    //整体扫描顺序
    void setScanOrder(ScanOrder order = HorizontalFirst)
    {
        _scanOrder = order;
        update();
    }
    ScanOrder scanOrder() const
    {
        return _scanOrder;
    }

    //水平方面的扫描顺序
    void setHorizontalScanDirection(HorizontalDirection horizontalDirection = LeftToRight)
    {
        _horizontalDirection = horizontalDirection;
        update();
    }
    HorizontalDirection horizontalScanDirection()const
    {
        return _horizontalDirection;
    }

    //竖直方向的扫描顺序
    void setVerticalScanDirection(VerticalDirection verticalDirection = TopToBottom)
    {
        _verticalDirection = verticalDirection;
        update();
    }
    VerticalDirection verticalScanDirection()const
    {
        return _verticalDirection;
    }

    //设置字体每个点的点亮表示方式 1?0
    void setCharBitFace(CharBitFace charBitFace = LightOnHigh)
    {
        _charBitFace = charBitFace;
        update();
    }
    CharBitFace charBitFace() const
    {
        return _charBitFace;
    }

    //设置显示的文字
    void setMainChar(const QChar& ch = QChar())
    {
        if(ch.isNull())
        {
            _ch = 'W';
        }
        else
        {
            _ch = ch;
        }
        update();
    }
    QChar mainChar()const
    {
        return _ch;
    }
    //设置网格数量
    void setGridNumber(int number = 48)
    {
        if(number <8)
            _gridNumber = 8;
        else
            _gridNumber = number;
        update();
    }
    int gridNumber()const
    {
        return _gridNumber;
    }
    //设置是否可以显示文字
    void setCharShow(bool bShow = true)
    {
        _charShow = bShow;
        update();
    }
    bool charShow()const
    {
        return _charShow;
    }
    //设置基础字体
    void setBaseFontFamily(QString fntFamily = "Arial")
    {
        if(!fntFamily.isEmpty())
            _baseFontFamily = fntFamily;
        update();
    }
    QString baseFontFamily()const
    {
        return _baseFontFamily;
    }

    //设置镜像
    void setHorMirrored(bool bHorMirrored = false)
    {
        _horizontalMirrored = bHorMirrored;
        update();
    }
    bool horMirrored()const
    {
        return _horizontalMirrored;
    }
    void setVerMirrored(bool bVerMirrored = false)
    {
        _verticalMirrored = bVerMirrored;
        update();
    }
    bool verMirrored()const
    {
        return _verticalMirrored;
    }

    //设置字节序
    void setByteLSB(bool bLSB = false)
    {
        _saveLSB =  bLSB;
        update();
    }
    bool byteLSB()const
    {
        return _saveLSB;
    }

protected:
    void paintEvent(QPaintEvent *);
    void drawScanOrder(QPainter& painter, const QRectF& hRectF, const QRectF& vRectF);
    void drawHorizontalScanDirection(QPainter& painter, const QRectF& drawRectF);
    void drawVerticalScanDirection(QPainter& painter, const QRectF& drawRectF);
    void drawMainChar(QPainter& painter, const QRectF& drawRectF);
    void drawMainGrid(QPainter& painter, const QRectF& drawRectF, int grid);

signals:
    void ucharValues(int lineByte, QByteArray bytes);

public slots:

private:
    bool _saveLSB;
    bool _horizontalMirrored;
    bool _verticalMirrored;
    bool _charShow;
    int _gridNumber;
    QChar _ch;
    QString _baseFontFamily;
    ScanOrder _scanOrder;
    HorizontalDirection _horizontalDirection;
    VerticalDirection _verticalDirection;
    CharBitFace _charBitFace;

};

#endif // CHARWIDGET_H
