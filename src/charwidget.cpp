#include "charwidget.h"
#include <QPainter>
#include <QImage>
#include <QDebug>

CharWidget::CharWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setMinimumSize(16,16);
    setMainChar();
    setScanOrder();
    setHorizontalScanDirection();
    setVerticalScanDirection();
    setCharBitFace();
    setGridNumber();
    setCharShow();
    setHorMirrored();
    setVerMirrored();
    setByteLSB();

    this->setMinimumSize(128, 128);
}

void CharWidget::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    qreal canvasWH = qMin(rect().width(), rect().height());
    canvasWH -= 0.5;
    QRectF drawRectF(rect().left(), rect().top(), canvasWH, canvasWH);

    //设置字体
    QFont font = painter.font();
    font.setFamily("Arial");
    painter.setFont(font);

    //绘制扫描的顺序
    qreal orderWH = canvasWH * 0.1;
    QRectF hOrderRect(drawRectF.left()+orderWH, drawRectF.top(), orderWH, orderWH);
    QRectF vOrderRect(drawRectF.left(), drawRectF.top()+orderWH, orderWH, orderWH);
    drawScanOrder(painter, hOrderRect, vOrderRect);

    //绘制水平方向的扫描方向
    QRectF hdirRectF(hOrderRect.topRight(), QPointF(drawRectF.right(), hOrderRect.bottom()));
    drawHorizontalScanDirection(painter, hdirRectF);

    //绘制竖直方向的扫描方向
    QRectF vdirRectF(vOrderRect.bottomLeft(), QPointF(vOrderRect.right(), drawRectF.bottom()));
    drawVerticalScanDirection(painter, vdirRectF);

    //绘制示例文字
    QRectF charRectF(hOrderRect.bottomLeft(), drawRectF.bottomRight());
    if(_charShow)
    {        
        drawMainChar(painter, charRectF);
    }

    //绘制网格
    drawMainGrid(painter, charRectF, _gridNumber);
}


void CharWidget::drawScanOrder(QPainter& painter, const QRectF& hRectF, const QRectF& vRectF)
{
    painter.save();
    //① 0x2460   ② 0x2461
    ushort num1 = 0x2460;
    ushort num2 = 0x2461;
    int charH = qMin(hRectF.width(), hRectF.height());
    int charV = qMin(vRectF.width(), vRectF.height());
    painter.setPen(Qt::blue);
    QFont font = painter.font();
    if(_scanOrder == HorizontalFirst)
    {
        font.setPixelSize(charH);
        painter.setFont(font);
        painter.drawText(hRectF, Qt::AlignCenter, QChar(num1));
        font.setPixelSize(charV);
        painter.setFont(font);
        painter.drawText(vRectF, Qt::AlignCenter, QChar(num2));
    }
    else
    {
        font.setPixelSize(charH);
        painter.setFont(font);
        painter.drawText(hRectF, Qt::AlignCenter, QChar(num2));
        font.setPixelSize(charV);
        painter.setFont(font);
        painter.drawText(vRectF, Qt::AlignCenter, QChar(num1));
    }
    QPolygonF hPolygon;
    painter.setPen(Qt::blue);
    painter.setBrush(QBrush(QColor(128,128,128, 128)));
    hPolygon << QPointF(0, 0) << hRectF.topLeft() << vRectF.topLeft();
    painter.drawPolygon(hPolygon);
    painter.restore();
}

void CharWidget::drawHorizontalScanDirection(QPainter& painter, const QRectF& drawRectF)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(QBrush(QColor(128, 255,128, 128)));
    QPolygonF hPolygonF;
    QString  title;
    if(_horizontalDirection == LeftToRight)
    {
        hPolygonF << QPointF(drawRectF.left(), drawRectF.top()+drawRectF.height()/2)
                  << QPointF(drawRectF.right()-drawRectF.width()*0.05, drawRectF.top()+drawRectF.height()*0.2)
                  << QPointF(drawRectF.right(), drawRectF.top()+drawRectF.height()/2)
                  << QPointF(drawRectF.right()-drawRectF.width()*0.05, drawRectF.bottom()-drawRectF.height()*0.2);
        title = "----------------->";
    }
    else
    {
        hPolygonF << QPointF(drawRectF.left(), drawRectF.top()+drawRectF.height()/2)
                  << QPointF(drawRectF.left()+drawRectF.width()*0.05, drawRectF.top()+drawRectF.height()*0.2)
                  << QPointF(drawRectF.right(), drawRectF.top()+drawRectF.height()/2)
                  << QPointF(drawRectF.left()+drawRectF.width()*0.05, drawRectF.bottom()-drawRectF.height()*0.2);
        title = "<-----------------";
    }
    painter.drawPolygon(hPolygonF);
    painter.setPen(Qt::black);
    painter.drawText(drawRectF, Qt::AlignCenter, title);
    painter.restore();
}

void CharWidget::drawVerticalScanDirection(QPainter& painter, const QRectF& drawRectF)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(QBrush(QColor(128, 255, 128, 128)));
    QPolygonF hPolygonF;
    QString  title;
    if(_verticalDirection == BottomToTop)
    {
        hPolygonF << QPointF(drawRectF.left()+drawRectF.width()/2, drawRectF.top())
                  << QPointF(drawRectF.right()-drawRectF.width()*0.2, drawRectF.top()+drawRectF.height()*0.05)
                  << QPointF(drawRectF.left()+drawRectF.width()/2, drawRectF.bottom())
                  << QPointF(drawRectF.left()+drawRectF.width()*0.2, drawRectF.top()+drawRectF.height()*0.05);
        title = "<-----------------";
    }
    else
    {
        hPolygonF << QPointF(drawRectF.left()+drawRectF.width()/2, drawRectF.top())
                  << QPointF(drawRectF.right()-drawRectF.width()*0.2, drawRectF.bottom()-drawRectF.height()*0.05)
                  << QPointF(drawRectF.left()+drawRectF.width()/2, drawRectF.bottom())
                  << QPointF(drawRectF.left()+drawRectF.width()*0.2, drawRectF.bottom()-drawRectF.height()*0.05);
        title = " ----------------->";
    }
    painter.drawPolygon(hPolygonF);
    painter.setPen(Qt::black);
    painter.rotate(90);
    QRectF hRectF(drawRectF.top(), -drawRectF.right(), drawRectF.height(), drawRectF.width());
    painter.drawText(hRectF, Qt::AlignCenter, title);
    painter.restore();
}

void CharWidget::drawMainChar(QPainter& painter, const QRectF& drawRectF)
{
    //char "W"
    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setFamily(_baseFontFamily);
    int charWH = qMin(drawRectF.width(), drawRectF.height());
    font.setPixelSize(charWH);

    QRect rc = drawRectF.toRect();
    QImage image(rc.size(), QImage::Format_ARGB32);
    QPainter pImage(&image);
    pImage.setFont(font);
    pImage.fillRect(image.rect(), Qt::white);
    pImage.drawText(image.rect(), Qt::AlignCenter, QString(_ch));

    QImage imageM = image.mirrored(_horizontalMirrored, _verticalMirrored);
    painter.drawImage(rc, imageM);
    painter.restore();
}

void CharWidget::drawMainGrid(QPainter& painter, const QRectF& drawRectF, int grid)
{
    Q_ASSERT(grid > 0);
    const qreal dx = drawRectF.width() / grid;
    const qreal dy = drawRectF.height() / grid;
    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor(128,128,128,128));
    painter.drawRect(drawRectF);
    int i = 0;
    for(qreal x=drawRectF.left(); x<drawRectF.right(); x+=dx)
    {
        if(i++ % 4 == 0)
        {
            painter.setPen(QColor(0, 0, 128, 128));
        }
        else
        {
            painter.setPen(QColor(128,128,128,128));
        }
        painter.drawLine(x, drawRectF.top(), x, drawRectF.bottom());
    }
    i = 0;
    for(qreal y=drawRectF.top(); y<drawRectF.bottom(); y+=dy)
    {
        if(i++ % 4 == 0)
        {
            painter.setPen(QColor(0, 0, 128, 128));
        }
        else
        {
            painter.setPen(QColor(128,128,128,128));
        }
        painter.drawLine(drawRectF.left(), y, drawRectF.right(), y);
    }

    painter.setPen(QColor(200,100,100,200));
    QFont font = painter.font();
    font.setPixelSize(qMin(dx, dy));
    painter.setFont(font);    
    //------------------------------------------------------------
    QImage canvas;
    if(_saveLSB)
        canvas = QImage(_gridNumber, _gridNumber, QImage::Format_MonoLSB);
    else
        canvas = QImage(_gridNumber, _gridNumber, QImage::Format_Mono);

    QPainter paintCanvas(&canvas);
    paintCanvas.setPen(Qt::white);//设置白色画笔
    font.setPixelSize(_gridNumber);
    font.setFamily(_baseFontFamily);
    paintCanvas.setFont(font);
    paintCanvas.fillRect(canvas.rect(), Qt::black);//设置黑色背景
    paintCanvas.drawText(canvas.rect(), Qt::AlignCenter, _ch);

    QImage canvasM = canvas.mirrored(_horizontalMirrored, _verticalMirrored);

    int useBytes = (canvasM.width() + 7) / 8;  // 计算存储用到的字节数
    QByteArray bytes;
    for(int col=0; col<canvasM.height(); ++col)
    {
        const uchar* pLineBits = canvasM.scanLine(col);
        for(int i=0; i<useBytes; i++)
        {
            uchar ch = *(pLineBits+i);
            if(_charBitFace == LightOnLow)
            {
                ch ^= 0xFF;
            }
            bytes.append(ch);
        }

        for(int row=0; row<canvasM.width(); row++)
        {
            bool bitOn = false;
            if(_saveLSB)
                bitOn = (*(pLineBits+row/8) & (0x01 << row%8)) != 0;
            else
                bitOn = (*(pLineBits+row/8) & (0x80 >> row%8)) != 0;

            qreal x = drawRectF.left() + row * dx;
            qreal y = drawRectF.top() + col * dy;
            QRectF cellRectF(x, y, dx, dy);

            if(_charBitFace == LightOnHigh)
            {
                painter.drawText(cellRectF, Qt::AlignCenter, bitOn ? QChar('1') : QChar('0'));
            }
            else
            {
                painter.drawText(cellRectF, Qt::AlignCenter, bitOn ? QChar('0') : QChar('1'));
            }
        }
    }
    emit ucharValues(useBytes, bytes);
    //------------------------------------------------------------
    painter.restore();
}
