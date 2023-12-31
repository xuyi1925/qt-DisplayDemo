#include "widget.h"
#include "ui_widget.h"
#include "displayarea.h"
#include "serial.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <Windows.h>
#include <QMessageBox>
#include <stdio.h>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    COM1 = serial_init(3, 115200, 8, 1, 0);

    ui->setupUi(this);

    // 设置窗口位置
    move(640, 400);

    // 初始化形状选择框
    InitComboFormSelect();


    // 设置显示区域二背景颜色为黑色
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0, 0, 0));
    ui->labelDisplayArea2->setAutoFillBackground(true);
    ui->labelDisplayArea2->setPalette(palette);

    // 显示区域一背景颜色默认为黑色
    ui->labelDisplayArea1->setAutoFillBackground(true);
    ui->labelDisplayArea1->setPalette(palette);

    // 定时发送鼠标位置
    QTimer* timerMousePos = new QTimer(this);
    timerMousePos->start(50);       // 50毫秒触发一次
    connect(timerMousePos, &QTimer::timeout, this, [=](){
        if (ui->labelDisplayArea1->mouseIn)
        {
            char* command = new char[5];
            command[0] = '4' - '0';          // 指令为4表示发送鼠标位置指令
            command[1] = (byte) ((ui->labelDisplayArea1->mousePosX >> 8) & 0xFF);
            command[2] = (byte) ((ui->labelDisplayArea1->mousePosX) & 0xFF);
            command[3] = (byte) ((ui->labelDisplayArea1->mousePosY >> 8) & 0xFF);
            command[4] = (byte) ((ui->labelDisplayArea1->mousePosY) & 0xFF);
            Serial_SendData(COM1, command, 5);
        }
    });


    // 文件选择
    connect(ui->btnFileSelect, &QPushButton::clicked, [=](){
        imgPath = QFileDialog::getOpenFileName(this, "打开文件", "E:/workspace/qt/qt-DisplayDemo/resource/image");
        qDebug() << imgPath;
        ui->labelDisplayArea1->SetImgPath(imgPath);
    });

    // 地图加载
    connect(ui->btnLoadMap, &QPushButton::clicked, [=](){
        ui->labelDisplayArea1->LoadImageW();
    });

    // 放大按钮
    connect(ui->btnEnlargement, &QPushButton::clicked, [=](){
        ui->labelDisplayArea1->Enlarge(2);
        char* command = new char[5];
        command[0] = '5' - '0';          // 指令为5表示发送比例变更指令
        command[1] = 0x00;
        command[2] = 0x00;
        command[3] = 0x00;
        command[4] = (byte)ui->labelDisplayArea1->GetRatioValue();
//        Serial_SendData(COM1, command, 5);
    });

    // 缩小按钮
    connect(ui->btnReduction, &QPushButton::clicked, [=](){
        ui->labelDisplayArea1->Reduce(2);
        char* command = new char[5];
        command[0] = '5' - '0';          // 指令为5表示发送比例变更指令
        command[1] = 0x00;
        command[2] = 0x00;
        command[3] = 0x00;
        command[4] = (byte)ui->labelDisplayArea1->GetRatioValue();
//        Serial_SendData(COM1, command, 5);
    });

    // 局部放大按钮
    connect(ui->btnPartialEnlargement, &QPushButton::clicked, [=](){
        ui->labelDisplayArea1->PartialEnlarge();
    });

    // 局部缩小按钮
    connect(ui->btnPartialReduction, &QPushButton::clicked, [=](){
        ui->labelDisplayArea1->PartialReduce();
    });

    // 颜色设置
    connect(ui->btnColorSet, &QPushButton::clicked, [=](){
        QColor color = QColorDialog::getColor(QColor(255, 0, 0));
        char* command = new char[5];
        // signal
        command[0] = '1' - '0'; // 指令为1表示发送字符颜色指令
        // value
        command[1] = '0' - '0';
        command[2] = (byte)color.red();
        command[3] = (byte)color.green();
        command[4] = (byte)color.blue();
        Serial_SendData(COM1, command, 5);
    });

    // A显
    connect(ui->btnDisplayA, &QPushButton::clicked, [=](){
       ui->labelDisplayArea2->SwitchDisplayMode(1000, 100, 10, 10, "距离/米", "回波强度/分贝");
       char* command = new char[5];
       command[0] = '7' - '0';          // 指令为7表示发送多窗口指令
       command[1] = 0x00;
       command[2] = 0x00;
       command[3] = 0x00;
       command[4] = 0x04;
       Serial_SendData(COM1, command, 5);
    });

    // B显
    connect(ui->btnDisplayB, &QPushButton::clicked, [=](){
       ui->labelDisplayArea2->SwitchDisplayMode(360, 120, 12, 6, "方位角/°", "距离/米");
       char* command = new char[5];
       command[0] = '7' - '0';          // 指令为7表示发送多窗口指令
       command[1] = 0x00;
       command[2] = 0x00;
       command[3] = 0x00;
       command[4] = 0x02;
       Serial_SendData(COM1, command, 5);
    });

    // R显
    connect(ui->btnDisplayR, &QPushButton::clicked, [=](){
       ui->labelDisplayArea2->SwitchDisplayMode(1000, 500, 10, 10, "距离/米",  "高度/米");
       char* command = new char[5];
       command[0] = '7' - '0';          // 指令为7表示发送多窗口指令
       command[1] = 0x00;
       command[2] = 0x00;
       command[3] = 0x00;
       command[4] = 0x01;
       Serial_SendData(COM1, command, 5);
    });

    // 形状选择
    connect(ui->comboFormSelect, &QComboBox::currentTextChanged, [=](){
       int index = ui->comboFormSelect->currentIndex()+1;
       char* command = new char[5];
       command[0] = '2' - '0';          // 指令为2表示发送形状指令
       command[1] = 0x00;
       command[2] = 0x00;
       command[3] = 0x00;
       command[4] = (byte)index;
       Serial_SendData(COM1, command, 5);

    });

    // 危险等级过滤
    connect(ui->horizontalSlider, &QSlider::valueChanged, [=](){
        int dangerLevel = ui->horizontalSlider->value();
        qDebug() << "dangerLevel:" << dangerLevel << endl;
        char* command = new char[5];
        command[0] = '3' - '0';         // 指令为3表示发送危险等级过滤指令
        command[1] = 0x00;
        command[2] = 0x00;
        command[3] = 0x00;
        command[4] = (byte)dangerLevel;
        Serial_SendData(COM1, command, 5);
    });

    // 透明度默认为0
    ui->transparencyEdit->setText("0");
    // 透明度设置
    connect(ui->btnTransparencySet, &QPushButton::clicked, [=](){
        QString transparencyString = ui->transparencyEdit->text();
        int transparencyValue = transparencyString.toInt();
        if (transparencyValue < 0 || transparencyValue > 100)
        {
            QMessageBox::information(this, "提示", "透明度的值需在0-100之间！");
        }
        else
        {
            char* command = new char[5];
            command[0] = '6' - '0';         // 指令为6表示透明度设置
            command[1] = 0x00;
            command[2] = 0x00;
            command[3] = 0x00;
            command[4] = (byte)(int)(0.01f * transparencyValue * 255);
            Serial_SendData(COM1, command, 5);
        }
    });

    // 反转扫描
    ui->CheckScanningDirection->setText("反转扫描");
    ui->CheckScanningDirection->setFixedSize(30,30);
    connect(ui->CheckScanningDirection, &QCheckBox::clicked, [=](){
        char* command = new char[5];
        command[0] = 'l' - 'a';         // 指令为11表示扫描线反转
        command[1] = 0x00;
        command[2] = 0x00;
        command[3] = 0x00;
        command[4] = 0x00;
        Serial_SendData(COM1, command, 5);
    });

    // 获取屏幕分辨率
    GetResolution();

    // 绑定发送显示区域位置的信号和槽函数
    connect(ui->labelDisplayArea1, SIGNAL(winPosition(int, int, int)), this, SLOT(sendWinPos(int, int, int)));

    // 发送扫描区间
    connect(ui->btnSendAngles, &QPushButton::clicked, [=]() {
        QString angleStart = ui->ScanningAngleStart->text();
        int angleStartValue = angleStart.toInt();
        if (angleStartValue < 0 || angleStartValue > 360)
        {
            QMessageBox::information(this, "提示", "起始角度的值需在0-360之间！");
        }
        QString angleEnd = ui->ScanningAngleEnd->text();
        int angleEndValue = angleEnd.toInt();
        if (angleEndValue < 0 || angleEndValue > 360)
        {
            QMessageBox::information(this, "提示", "终点角度的值需在0-360之间！");
        }
        if (angleEndValue <= angleStartValue) {
            QMessageBox::information(this, "提示", "终点角度需大于起始角度！");
        }
        angleStartValue *= 10;
        angleEndValue *= 10;            // 角度乘以10再发送，FPGA端精度为0.1度
        char* command = new char[5];
        command[0] = 'm' - 'a';         // 指令为12发送起始和终止角度
        command[1] = (byte) ((angleStartValue >> 8) & 0xFF);
        command[2] = (byte) (angleStartValue & 0xFF);
        command[3] = (byte) ((angleEndValue >> 8) & 0xFF);
        command[4] = (byte) (angleEndValue & 0xFF);
        Serial_SendData(COM1, command, 5);
    });
}

void Widget::InitComboFormSelect()
{
    ui->comboFormSelect->addItem("▲三角形");
    ui->comboFormSelect->addItem("●圆形");
    ui->comboFormSelect->addItem("■正方形");
    ui->comboFormSelect->addItem("★五角星");
}

// 初始化危险等级滑动条
void Widget::InitHorizontalSlider()
{
    ui->horizontalSlider->setMaximum(4);
    ui->horizontalSlider->setMinimum(1);
    ui->horizontalSlider->setSingleStep(1);
}

// 获取屏幕分辨率
void Widget::GetResolution()
{
    int resolutionX=GetSystemMetrics(SM_CXSCREEN);
    int resolutionY=GetSystemMetrics(SM_CYSCREEN);
    qDebug() << resolutionX << resolutionY;
}

// 发送显示区域1的坐标信息
void Widget::sendWinPos(int scale, int x, int y)
{
    char* command = new char[5];
    command[0] = '9' - '0';          // 指令为9表示发送鼠标位置指令
    command[1] = (byte) scale;
    command[2] = (byte) ((x >> 4) & 0xFF);
    command[3] = (byte) (((y >> 8) & 0x0F) | ((x << 4) & 0xF0));
    command[4] = (byte) ((y) & 0xFF);
    qDebug() << scale;
    Serial_SendData(COM1, command, 5);
    qDebug() << "signal send success";
}

// 窗口移动 向FPGA发送显示区域位置信息
void Widget::moveEvent(QMoveEvent *ev)
{
    char* command = new char[5];
    command[0] = 'k' - 'a';          // 指令为10表示发送窗口位置
    command[1] = (byte) (((ev->pos().x() + ui->labelDisplayArea1->pos().x()) >> 8) & 0xFF);
    command[2] = (byte) ((ev->pos().x() + ui->labelDisplayArea1->pos().x()) & 0xFF);
    command[3] = (byte) (((ev->pos().y() + ui->labelDisplayArea1->pos().y()) >> 8) & 0xFF);
    command[4] = (byte) ((ev->pos().y() + ui->labelDisplayArea1->pos().y()) & 0xFF);
    Serial_SendData(COM1, command, 5);
}

Widget::~Widget()
{
    delete ui;
}

