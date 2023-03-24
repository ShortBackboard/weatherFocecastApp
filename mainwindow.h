#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>//http
#include <QNetworkReply>

#include <QList>
#include <QLabel>
#include <QMap>

#include "weatherdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

protected:
    //实现右键菜单事件，重写父类的虚函数
    void contextMenuEvent(QContextMenuEvent *event);

    //获取鼠标按下的位置
    void mousePressEvent(QMouseEvent *event);

    //窗口移动
    void mouseMoveEvent(QMouseEvent *event);

    //获取天气信息,用于发送http请求
    void getWeaherInfo(QString cityCode);

    //解析天气数据
    void parseJson(QByteArray &byteArray);

    //更新ui内的数据
    void updateUI();

    //重写父类的eventfilter方法,事件过滤捕获
    bool eventFilter(QObject *watched, QEvent *event);

    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();

private slots:
    void on_btnSearch_clicked();

private:
    void onReplied(QNetworkReply *reply);//请求数据成功调用槽

private:
    QMenu *mExitMenu;  //右键退出的菜单
    QAction *mExitAct;  //退出事件的菜单项

    QPoint mOffset;    //窗口移动时，鼠标点击点和App窗口左上角的偏移

    //http请求
    QNetworkAccessManager *mNetAccessManager;//请求数据

    //保存今天和六天的数据
    Today mToday;
    Day mDay[6];


    //星期和日期List
    QList<QLabel *> mWeekList;
    QList<QLabel *> mDateList;

    //天气和天气图标List
    QList<QLabel *> mTypeList;
    QList<QLabel *> mTypeIconList;

    //天气污染指数List
    QList<QLabel *> mAqiList;

    //风力和风向List
    QList<QLabel *> mFlList;
    QList<QLabel *> mFxList;


    //天气类型与图片资源相对应
    //键值对
    QMap<QString,QString > mTypeMap;

};
#endif // MAINWINDOW_H
