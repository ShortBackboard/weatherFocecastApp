#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QAction>
#include <QContextMenuEvent>//用于产生右键事件
#include <QCursor>
#include <QMenu>
#include <QMessageBox>//弹框

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QPainter>

#include "citycodeTool.h"

#define INCREMENT 3 //1摄氏度温差对于的px增量,绘制曲线
#define POINT_RADIUS 3 //曲线描点的大小
#define TEXT_OFFSET_X 12 //温度文本显示相当于点的x偏移量
#define TEXT_OFFSET_Y 12 //温度文本显示相当于点的y偏移量

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);//设置无边框
    setFixedSize(width(),height());//设置固定窗口大小


    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();

    mExitAct->setText("退出");//设置内容
    mExitAct->setIcon(QIcon(":/res/close.png"));//设置图片

    //把菜单项添加到菜单里面
    mExitMenu->addAction(mExitAct);

    //点击事件退出，信号与槽
    connect(mExitAct,&QAction::triggered,
            this,&QApplication::quit);


    //网络请求
    //请求数据成功，关联&MainWindow::onReplied
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,
            this,&MainWindow::onReplied);


    //默认初始值
    getWeaherInfo("重庆");


    //将ui控件添加到控件数组
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3
              << ui->lblWeek4 << ui->lblWeek5;

    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3
              << ui->lblDate4 << ui->lblDate5 ;

    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3
              << ui->lblType4 << ui->lblType5;

    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2
                  << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2
             << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3
            << ui->lblFx4 << ui->lblFx5;

    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3
            << ui->lblFl4 << ui->lblFl5;

    //天气类型与图片资源，添加到容器中
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoyYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/BaoTuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/res/type/undefined.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("中到大雪",":/res/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");


    //绘制曲线
    //给标签安装事件过滤器
    //任意对象都可以提前拦截其他任意对象的事件，拦截到的事件都会在本对象的eventFilter函数中被接收到，程序员可以决定拦截并处理后是否继续放行。

    //当lblHighCurve和lblLowCurve有事件发生，就会被this当前窗口拦截，调用其eventFilter方法
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

}



//重写父类的虚函数
//父类中的默认实现是默认忽略右键菜单事件
//重写后可处理右键菜单事件
void MainWindow::contextMenuEvent(QContextMenuEvent *event){

    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());//传递鼠标当前位置

    //调用accept表示：这个事件已经处理，不需要向上传递
    event->accept();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //event->globalPos(): 鼠标点相对于电脑桌面的偏移
    //this->pos(): 鼠标点相对于App窗口左上角的坐标
    mOffset = event->globalPos() - this->pos();
}

//移动窗口左上角的点,移动App窗口
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

//获取天气信息
void MainWindow::getWeaherInfo(QString cityName)
{

    QString citycode = CitycodeTool::getCitycode(cityName);

    if(citycode.isEmpty()){
        QMessageBox::warning(this,"天气","请检查输入是否正确",QMessageBox::Ok);
        return;
    }

    //发送http请求
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + citycode);
    qDebug() <<"url: "<<url;
    mNetAccessManager->get(QNetworkRequest(url));
}


//更新ui内的数据
void MainWindow::updateUI()
{
    //1.更新日期和城市
    //ui->lblDate->setText(mToday.date);
    ui->lblDate->setText(QDate::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);//格式化 20221010 -> 2022/10/10

    ui->lblCity->setText(mToday.city);

    //2.更新今天的数据
    ui->lblTemp->setText(QString::number(mToday.wendu) + "°c");//int转QString
    ui->lblType->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" +
                            QString::number(mToday.high) + "°c");
    ui->lblGanMao->setText("感冒指数：" + mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);

    ui->lblPM25->setText(QString::number(mToday.pm25));

    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    //更新图标
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);

    //3.更新6天的数据
    for(int i=0; i<6; i++){
        //更新日期和时间
        mWeekList[i]->setText("周" + mDay[i].week.right(1));//获取字符串右边第一个字符
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        //更新天气质量
        if(mDay[i].aqi >= 0 && mDay[i].aqi <= 50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121,184,0);");
        } else if(mDay[i].aqi >= 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,187,23);");
        } else if(mDay[i].aqi >= 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        } else if(mDay[i].aqi >= 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235,17,27);");
        } else if(mDay[i].aqi >= 200 && mDay[i].aqi <= 250){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        } else {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }

        //更新风力风向
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //watched:过滤对象
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }

    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }

    return QWidget::eventFilter(watched,event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1.获取x坐标
    int pointX[6] = {0};
    for(int i = 0; i < 6 ; i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    //2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;//6天的平均最高温度

    for(int i = 0; i < 6 ; i++){
        tempSum += mDay[i].high;
    }

    tempAverage = tempSum / 6;

    int pointY[6] = {0};

    int yCenter = ui->lblHighCurve->height()/2;

    for(int i = 0; i < 6 ; i++){
        pointY[i] = yCenter - ((mDay[i].high - tempAverage)* INCREMENT);
        //3px Qt坐标轴y向下增大
    }

    //3.开始绘制
    //3.1初始化画笔
    //比温度平均值高就向上移动px
    QPen pen = painter.pen();//初始化画笔
    pen.setWidth(1);//画笔宽度
    pen.setColor(QColor(255,170,0));//画笔颜色

    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));//设置画刷，填充颜色

    //3.2 画点、写文本
    for(int i = 0; i < 6 ; i++){
        //显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);
        //Ellipse:椭圆

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,
                         QString::number(mDay[i].high) + "°c");
    }

    //3.3连接点画线
    for(int i = 0; i < 5 ; i++){
        if(i == 0){
            pen.setStyle(Qt::DotLine);  //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);  //实线
            painter.setPen(pen);
        }

        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }

}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1.获取x坐标
    int pointX[6] = {0};
    for(int i = 0; i < 6 ; i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    //2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;//6天的平均最高温度

    for(int i = 0; i < 6 ; i++){
        tempSum += mDay[i].low;
    }

    tempAverage = tempSum / 6;

    int pointY[6] = {0};

    int yCenter = ui->lblLowCurve->height()/2;

    for(int i = 0; i < 6 ; i++){
        pointY[i] = yCenter - ((mDay[i].low - tempAverage)* INCREMENT);
        //3px Qt坐标轴y向下增大
    }

    //3.开始绘制
    //3.1初始化画笔
    //比温度平均值高就向上移动px
    QPen pen = painter.pen();//初始化画笔
    pen.setWidth(1);//画笔宽度
    pen.setColor(QColor(0,255,255));//画笔颜色

    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));//设置画刷，填充颜色

    //3.2 画点、写文本
    for(int i = 0; i < 6 ; i++){
        //显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);
        //Ellipse:椭圆

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,
                         QString::number(mDay[i].low) + "°c");
    }

    //3.3连接点画线
    for(int i = 0; i < 5 ; i++){
        if(i == 0){
            pen.setStyle(Qt::DotLine);  //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);  //虚线
            painter.setPen(pen);
        }

        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }

}


//解析数据
void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&err);//如果出错，就把信息存入err中

    if(err.error != QJsonParseError::NoError){//如果出错
        return;
    }

    QJsonObject rootObj = doc.object();

    //1.解析日期和城市,通过键获得值
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    //2.解析昨天的信息
    //rootObj.value("data").toObject().value("yesterday").toObject().value("date").toString;
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();
    mDay[0].type = objYesterday.value("type").toString();

    //最高温
    QString highTem = objYesterday.value("high").toString().split(" ").at(1);//以空格分割，取第二部分  "高温 18^" ->"18^"
    highTem = highTem.left(highTem.length()-1);//18^ -> 18
    mDay[0].high = highTem.toInt();

    //最低温
    QString lowTem = objYesterday.value("low").toString().split(" ").at(1);
    lowTem = lowTem.left(lowTem.length()-1);//从左向右截取n个字符串
    mDay[0].low = lowTem.toInt();

    //风向风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();

    //天气质量指数
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //3.解析未来5天的信息
    QJsonArray forecastArr = objData.value("forecast").toArray();

    for( int i = 0; i < 5; i++){
        QJsonObject objForecast = forecastArr[i].toObject();
        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();

        //最高温、最低温
        QString highTem = objForecast.value("high").toString().split(" ").at(1);
        highTem = highTem.left(highTem.length()-1);
        mDay[i+1].high = highTem.toInt();

        QString lowTem = objForecast.value("low").toString().split(" ").at(1);
        lowTem = lowTem.left(lowTem.length()-1);
        mDay[i+1].low = lowTem.toInt();

        //风向风力
        mDay[i+1].fx = objForecast.value("fx").toString();
        mDay[i+1].fl = objForecast.value("fl").toString();

        //天气质量指数
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();
    }

    //4.解析今天的数据  mDay[0] 就是今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();

    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //5.更新ui内的数据
    updateUI();

    //6.绘画温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}




void MainWindow::onReplied(QNetworkReply *reply)
{
    //请求成功后所有数据都封装在reply中
    qDebug() << "onReplied successfully";

    //获取响应码
    int status_code =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "operation: " << reply->operation();
    qDebug() << "status code: " << status_code;//200表示成功
    qDebug() << "url: " << reply->url();//请求时的url
    qDebug() << "raw header: " << reply->rawHeaderList();

    if(reply->error() != QNetworkReply::NoError || status_code != 200){
        //如果请求失败
        qDebug() << reply->errorString().toLatin1().data();

        //弹框    (parent,title,text,button)
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        //请求成功,获取响应数据
        QByteArray byteArray = reply->readAll();

        //打印所有的响应数据
        //qDebug() << "read all: " << byteArray.data();

        //解析数据
        parseJson(byteArray);
    }

    reply->deleteLater();//释放内存
}


//搜索按钮点击事件
void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeaherInfo(cityName);
}



MainWindow::~MainWindow()
{
    delete ui;
}
