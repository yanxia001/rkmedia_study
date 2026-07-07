#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建一个vlc实例
    libvlc_instance_t * ch = libvlc_new(0,NULL);
    //创建vlc媒体 rtsp://192.168.100.75/9203
    libvlc_media_t * md =libvlc_media_new_location(ch,"rtsp://192.168.100.75/9203");
    //播放本地文件
    //libvlc_media_t * md = libvlc_media_new_path(ch,"");
    //创建媒体播放器
    libvlc_media_player_t * player = libvlc_media_player_new_from_media(md);

    //播放函数
    libvlc_media_player_play(player);

    //指定播放窗口
    libvlc_media_player_set_xwindow(player,ui->label->winId());
}

MainWindow::~MainWindow()
{
    delete ui;
}
