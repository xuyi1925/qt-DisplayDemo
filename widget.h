#pragma once

#include <QWidget>
#include "serial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class DisplayArea;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Ui::Widget *ui;
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QString imgPath;        // 图片路径
    PORT COM1;              // 串口


    void InitComboFormSelect();
    void InitTableLayerSeq();
    void SwapTableRow();
    void InitHorizontalSlider();

    // 获取分辨率
    void GetResolution();

    // 移动事件
    void moveEvent(QMoveEvent *event);

private:

private slots:
    void sendWinPos(int, int, int);

};
