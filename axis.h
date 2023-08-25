#ifndef AXIS_H
#define AXIS_H

#include <QLabel>

class Axis : public QLabel
{
    Q_OBJECT
public:
    explicit Axis(QWidget *parent = nullptr);

    // 绘图事件
    void paintEvent(QPaintEvent *);

    // 设置为数学坐标系
    void SetMathCoordinate(QPainter* painter);

    // 恢复默认坐标系
    void ResetDefaultCoordinate(QPainter* painter);

    // 绘制坐标刻度
    void PaintCoordinateCalibration(QPainter* painter);

    // 绘制坐标值
    void PaintCoordinateValue(QPainter* painter);

    // 绘制两条坐标轴直线
    void PaintCoordinateAxis(QPainter* painter);

    // 切换显示方式显示
    void SwitchDisplayMode(int, int, int, int, QString, QString);

private:
    QPoint m_ptStartPos; // 起始点
    int m_axisWidth;    // 坐标轴的宽度
    int m_axisHeight;   // 坐标轴的高度
    int maxX = 360;     // x坐标最大值
    int maxY = 120;     // y坐标最大值
    int countCalibrationX = 12; // 横坐标尺度个数
    int countCalibrationY = 6; // 纵坐标尺度个数
    QString strXAxis = "方位角/°";
    QString strYAxis = "距离/米";
signals:
};

#endif // AXIS_H
