#include "displayarea.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include "serial.h"
#include "widget.h"
#include "ui_widget.h"
#include "complex.h"
#include <QMessageBox>

DisplayArea::DisplayArea(QWidget *parent) : QLabel(parent)
{
    // 设置鼠标的追踪状态
    setMouseTracking(true);
}

// 鼠标进入事件
void DisplayArea::enterEvent(QEvent *)
{
    qDebug() << "鼠标进入";
    mouseIn = true;
}

// 鼠标离开事件
void DisplayArea::leaveEvent(QEvent *)
{
    qDebug() << "鼠标离开";
    mouseIn = false;
}

// 鼠标按下
void DisplayArea::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        QString str = QString("鼠标按下, x = %1, y = %2").arg(ev->x()).arg(ev->y());
        mouseXStart = ev->x();
        mouseYStart = ev->y();
        qDebug() << str;
        if (mouseState > 0)
        {
            update();
        }
    }
}

// 鼠标释放
void DisplayArea::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        QString str = QString("鼠标释放, globalX = %1, globalY = %2").arg(ev->globalX()).arg(ev->globalY());
        qDebug() << str;
    }
}

// 鼠标移动
void DisplayArea::mouseMoveEvent(QMouseEvent *ev)
{
    QString str = QString("鼠标移动, globalX = %1, globalY = %2").arg(ev->globalX()).arg(ev->globalY());
    if (ev->buttons() & Qt::LeftButton)
    {
        imgOffsetX += ev->x() - mouseXStart;
        imgOffsetY += ev->y() - mouseYStart;
        mouseXStart = ev->x();
        mouseYStart = ev->y();
        update();
    }
    mousePosX = ev->x();
    mousePosY = ev->y();
}

// 设置图片路径
void DisplayArea::SetImgPath(QString path)
{
    imgPath = path;
    pathFlag = true;
    LoadImageW();
}

// 绘图事件
void DisplayArea::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    int width = this->width();
    int height = this->height();
    if (mouseState == 0)    // 直接放大/缩小
    {
        if (sizeFlag)
        {
            if (pathFlag) { // 加载进地图后才对地图进行操作
                map.load(imgPath);
                map = map.scaled(mapOriWidth * scale, mapOriHeight * scale, Qt::KeepAspectRatio);
                mapPosX = (width - map.width()) / 2;
                mapPosY = (height - map.height()) / 2;
            }
            sizeFlag = false;
            emit winPosition((int)log2l((long) scale), width * (1.0 - 1.0 / scale) / 2, height * (1.0 - 1.0 / scale) / 2);
        }
    }
    else                    // 以鼠标点击位置为中心放大/缩小
    {
        if (mouseState == 1) // 放大
        {
            scale *= 2;
        }
        else                // 缩小
        {
            scale /= 2;
        }
        float nX = ((float)mouseXStart - imgOffsetX - mapPosX) / map.width();
        float nY = ((float)mouseYStart - imgOffsetY - mapPosY) / map.height();


        if (pathFlag) { // 加载进地图后才对地图进行操作
            map.load(imgPath);
            map = map.scaled(mapOriWidth * scale, mapOriHeight * scale, Qt::KeepAspectRatio);
        }
        mapPosX = mouseXStart - nX * map.width();
        mapPosY = mouseYStart - nY * map.height();

        imgOffsetX = 0;
        imgOffsetY = 0;
        mouseState = 0;
        painter.drawImage(QRectF(mapPosX + imgOffsetX, mapPosY + imgOffsetY, map.width(), map.height()), map);
        this->setCursor(Qt::CustomCursor);
        emit winPosition((int)log2l((long) scale), mouseXStart - nX * width, mousePosY - nY * height);
    }
    PaintPpi(&painter, this->width() / 2 + imgOffsetX, this->height() / 2 + imgOffsetY,  70 * scale, 6);
}

// 按中心位置放大
void DisplayArea::Enlarge(int ratio)
{
    if (scale >=64)
    {
        QMessageBox::information(this, "提示", QString("当前放大倍数为%1，不能继续放大！").arg(scale));
    }
    else {
        scale *= ratio;
        imgOffsetX = 0;
        imgOffsetY = 0;
        sizeFlag = true;
        update();
    }
}

// 按中心位置缩小
void DisplayArea::Reduce(int ratio)
{
    if (scale <=1)
    {
        QMessageBox::information(this, "提示", QString("当前放大倍数为%1，不能继续缩小！").arg(scale));
    }
    else {
        scale /= ratio;
        imgOffsetX = 0;
        imgOffsetY = 0;
        sizeFlag = true;
        update();
    }
}

// 加载地图
void DisplayArea::LoadImageW()
{
    if (pathFlag)
    {
        map.load(imgPath);
        mapOriWidth = map.width();
        mapOriHeight = map.height();
        update();
    }
}

// 局部放大
void DisplayArea::PartialEnlarge()
{
    if (scale >=64)
    {
        QMessageBox::information(this, "提示", QString("当前放大倍数为%1，不能继续放大！").arg(scale));
    }
    else {
        QPixmap pix(":/icon/enlargement.png");
        this->setCursor(QCursor(pix));
        if (mouseState == 1)
        {
            mouseState = 0;
            this->setCursor(Qt::CustomCursor);
        }
        else
        {
            mouseState = 1;
        }
    }
}

// 局部缩小
void DisplayArea::PartialReduce()
{
    if (scale <=1)
    {
        QMessageBox::information(this, "提示", QString("当前放大倍数为%1，不能继续缩小！").arg(scale));
    }
    else
    {
        QPixmap pix(":/icon/reduction.png");
        this->setCursor(QCursor(pix));
        if (mouseState == 2)
        {
            mouseState = 0;
            this->setCursor(Qt::CustomCursor);
        }
        else
        {
            mouseState = 2;
        }
    }
}

// 绘制ppi仪表盘
void DisplayArea::PaintPpi(QPainter* painter, int centerX, int centerY, int radius, int n)
{
    QPen pen;
    if (pathFlag)
    {
        pen.setColor(Qt::black);
    }
    else
    {
        pen.setColor(Qt::white);
    }
    pen.setWidth(2);
    painter->setPen(pen);

    // 绘制n个圆形
    for (int i = 1; i <= n; i++)
    {
        painter->drawEllipse(centerX - radius * i / 2, centerY - radius * i / 2, radius * i, radius * i);
    }

    // 绘制八条直线
    int length = n * radius / 2;
    double p = 3.1415926 / 4;
    int x0, y0, x1, y1;
    int gap = 2;
    for (int i = 0; i < 4; i++)
    {
        x0 = centerX - length * cos(p * i);
        y0 = centerY + length * sin(p * i);
        x1 = centerX + length * cos(p * i);
        y1 = centerY - length * sin(p * i);
        painter->drawLine(x0, y0, x1, y1);
        QString str = QString::number(180 + 45 * i);
        painter->drawText(x0 - gap * sizeof(str) * cos(p * i) - 10, y0 + gap * sizeof(str) * sin((p * i)) + 8, str);
        str = QString::number(45 * i);
        painter->drawText(x1 + gap * sizeof(str) * cos(p * i) - 10, y1 - gap * sizeof(str) * sin((p * i)) + 8, str);
    }

    // 绘制比例尺
    painter->drawLine(0.9 * this->width(), 0.9 * this->height(), 0.9 * this->width(), 0.9 * this->height() - 8);
    painter->drawLine(0.9 * this->width() + radius / 2, 0.9 * this->height(), 0.9 * this->width() + radius / 2, 0.9 * this->height() - 8);
    painter->drawLine(0.9 * this->width(), 0.9 * this->height(), 0.9 * this->width() + radius / 2, 0.9 * this->height());
    painter->drawText(0.9 * this->width(), 0.9 * this->height() + 15, QString("100m"));
}

// 获取比例参数
int DisplayArea::GetRatioValue()
{
    return (int) log2l((long) scale);
}
