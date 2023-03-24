/*
 *
 * A program : weatherForecastApp
 *
 * anthor:  ShortBackboard
 * date:    2023-3-24
 *
 *
 * 大致步骤：
 * 1.添加资源文件
 * 新建文件Qt Resource File
 *
 * 2.设置窗口无边框，固定大小；界面布局
 *
 * 3.设置鼠标右键事件
 *
 * 4.设置鼠标移动事件
 *
 * 5.获取天气数据
 *
 * 6.解析天气数据
 * 获取到服务端的数据之后，就可以使用Qt提供的JSON相关的类来解析数据
 *
 * 7.更新ui界面的数据
 *
 * 8.获取城市编号
 * 
 * 9.绘制温度曲线
 * 流程：
 * 调用标签的update方法
 * 框架发送QEvent::Paint事件给标签
 * 事件被MainWindow拦截，进而调用其eventFilter方法
 * 在eventFilter中，调用paintHighCurve和paintLowCurve来真正绘制曲线
 *
*/

#include "mainwindow.h"

#include <QApplication>
#include <memory>

using std::make_unique;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = make_unique<MainWindow>();
    w->show();
    return a.exec();
}
