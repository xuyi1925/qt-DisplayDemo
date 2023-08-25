#ifndef DISPLAYAREA_H
#define DISPLAYAREA_H

#include <QLabel>
#include <QPaintEvent>
#include <QImage>

class Widget;

class DisplayArea : public QLabel
{
    Q_OBJECT
public:
    explicit DisplayArea(QWidget *parent = nullptr);

    int scale = 1;

    // 鼠标进入事件
    void enterEvent(QEvent *);

    // 鼠标离开事件
    void leaveEvent(QEvent *);

    // 鼠标按下
    void mousePressEvent(QMouseEvent *ev);

    // 鼠标释放
    void mouseReleaseEvent(QMouseEvent *);

    // 鼠标移动
    void mouseMoveEvent(QMouseEvent *);

    // 设置文件路径
    void SetImgPath(QString);

    // 加载地图
    void LoadImageW();

    // 绘图事件
    void paintEvent(QPaintEvent *);

    // 放大
    void Enlarge(int);
    // 缩小
    void Reduce(int);

    // 局部放大
    void PartialEnlarge();

    // 局部缩小
    void PartialReduce();

    // 绘制ppi仪表盘
    void PaintPpi(QPainter* painter,int, int, int, int);

    // 获取比例参数
    int GetRatioValue();

    // 鼠标位置
    int mousePosX = 0;
    int mousePosY = 0;
    bool mouseIn = false;

private:
    Widget *widget;
    QString imgPath;
    QImage dashBoard;
    QImage map;
    QImage mapScaled;

    bool pathFlag = false;
    bool sizeFlag = false;

    int mouseState = 0;

    int mouseXStart;
    int mouseYStart;
    int imgOffsetX = 0;
    int imgOffsetY = 0;
    int mapPosX = 0;
    int mapPosY = 0;
    int dashBoardPosX = 0;
    int dashBoardPosY = 0;
    int mapOriWidth = 0;
    int mapOriHeight = 0;

signals:
    void winPosition(int, int, int);

};

#endif // DISPLAYAREA_H
