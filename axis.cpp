#include "axis.h"
#include <QPainter>
#include <QDebug>

const float GRAPH_SCALE = 1.2;                // 图的放大倍数
const int COORDINATE_CALIBRATION_LENGTH = 4;    // 坐标刻度的长度

Axis::Axis(QWidget *parent) : QLabel(parent)
{
    m_ptStartPos.setX(30);
    m_ptStartPos.setY(30);
    m_axisWidth = 320 * GRAPH_SCALE;
    m_axisHeight = 120 * GRAPH_SCALE;
}

void Axis::SetMathCoordinate(QPainter* painter)
{
    // 转换为数学坐标系，原点在左下角
    painter->setWindow(0,height(),width(),-height());
}

void Axis::ResetDefaultCoordinate(QPainter* painter)
{
    // 恢复默认坐标系，原点在左上角
    painter->setWindow(0, 0, this->width(), this->height());
}

void Axis::PaintCoordinateAxis(QPainter* painter)
{
    // 绘制 x 轴，y轴
    QPen pen;
    pen.setColor(Qt::white);
    painter->setPen(pen);
    SetMathCoordinate(painter);
    QPoint axisStartPoint;
    QPoint axisXEndPoint; // x 轴终点
    QPoint axisYEndPoint; // y 轴终点

    axisStartPoint.setX(m_ptStartPos.x());
    axisStartPoint.setY(m_ptStartPos.y());

    axisXEndPoint.setX(m_ptStartPos.x() + m_axisWidth);
    axisXEndPoint.setY(m_ptStartPos.y());

    axisYEndPoint.setX(m_ptStartPos.x());
    axisYEndPoint.setY(m_ptStartPos.y() + m_axisHeight);

    painter->drawLine(axisStartPoint, axisXEndPoint);
    painter->drawLine(axisStartPoint, axisYEndPoint);
}

void Axis::PaintCoordinateCalibration(QPainter* painter)
{
    // 绘制坐标刻度
    int deltaX = m_axisWidth / countCalibrationX;  // X 轴坐标刻度宽度
    int deltaY = m_axisHeight / countCalibrationY; // Y 轴坐标刻度宽度

    for (int i = m_ptStartPos.x(); i <= m_axisWidth + m_ptStartPos.x(); i = i + deltaX)
    {
       // 坐标刻度起始点
       QPoint calibrationStartPoint;
       calibrationStartPoint.setX(i);
       calibrationStartPoint.setY(m_ptStartPos.y());

       // 坐标刻度结束点
       QPoint calibrationEndPoint;
       calibrationEndPoint.setX(i);
       calibrationEndPoint.setY(m_ptStartPos.y() + COORDINATE_CALIBRATION_LENGTH);

       painter->drawLine(calibrationStartPoint, calibrationEndPoint);
     }

    for (int i = m_ptStartPos.y() + deltaY; i <= m_axisHeight + m_ptStartPos.y(); i = i + deltaY)
    {
       // 坐标刻度起始点
       QPoint calibrationStartPoint;
       calibrationStartPoint.setX(m_ptStartPos.x()); // x 轴不变，y 轴变
       calibrationStartPoint.setY(i);

       // 坐标刻度结束点
       QPoint calibrationEndPoint;
       calibrationEndPoint.setX(m_ptStartPos.x() - COORDINATE_CALIBRATION_LENGTH);
       calibrationEndPoint.setY(i);

       painter->drawLine(calibrationStartPoint, calibrationEndPoint);
     }
}

void Axis::PaintCoordinateValue(QPainter* painter)
{
    // 绘制坐标值
    int axisXValue = 0;
    int axisYValue = 0;

    int deltaX = m_axisWidth / countCalibrationX;  // X 轴坐标刻度宽度
    int deltaY = m_axisHeight / countCalibrationY; // Y 轴坐标刻度宽度

    ResetDefaultCoordinate(painter); // 必须恢复原来的坐标系，不然文字会镜像

    for (int i = m_ptStartPos.x(); i <= m_axisWidth + m_ptStartPos.x(); i = i + deltaX)
    {
        QString strAxisXValue = QString::number(axisXValue);
        QPoint temp;

        temp.setX(i - 10); // 左边移动的偏移量
        temp.setY(height() + 18 - m_ptStartPos.y());
        painter->drawText(temp, strAxisXValue);
        axisXValue = axisXValue + maxX / countCalibrationX;
    }

    for (int i = 0; i <= m_axisHeight; i = i + deltaY)
    {
        QString strAxisYValue = QString::number(axisYValue);

        QPoint temp;
        temp.setX(m_ptStartPos.x() - 23);         // 左边移动的偏移量
        temp.setY(height() - m_ptStartPos.y() - i + 3);

        painter->drawText(temp, strAxisYValue);
        axisYValue = axisYValue + maxY / countCalibrationY;
    }
    painter->drawText(QPoint(m_axisWidth + m_ptStartPos.x()+15, height() + 18 - m_ptStartPos.y()), strXAxis);
    painter->drawText(QPoint(m_ptStartPos.x(), height() - m_axisHeight - m_ptStartPos.y()), strYAxis);
}

void Axis::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制坐标轴两条直线
    PaintCoordinateAxis(&painter);

    // 绘制坐标刻度
    PaintCoordinateCalibration(&painter);

    // 绘制坐标值
    PaintCoordinateValue(&painter);
}

void Axis::SwitchDisplayMode(int maxX, int maxY, int countCalibrationX, int countCalibrationY, QString strX, QString strY)
{
    this->maxX = maxX;
    this->maxY = maxY;
    this->countCalibrationX = countCalibrationX;
    this->countCalibrationY = countCalibrationY;
    strXAxis = strX;
    strYAxis = strY;
    update();
}

