/*
 * 定义两个类来保存接收到的数据
 * class Today:保存今天的数据
 *
 * class Day:保存六天的数据
 *
 * 因为从服务器返回的天气数据类型是拼音，所以设置成员变量为拼音
*/


#ifndef WEATHERDATA_H
#define WEATHERDATA_H


class Today{
public:
    Today(){
        //默认值
        date = "2022-10-20";
        city = "广州";

        ganmao = "感冒指数";

        wendu = 0;
        shidu = "0%";
        pm25 = 0;
        quality = "无数据";

        type = "多云";

        fl = "2级";
        fx = "南风";

        high = 30;
        low = 18;
    }

    QString date;//日期
    QString city;//城市

    QString ganmao;//感冒指数

    int wendu;//温度
    QString shidu;//湿度
    int pm25;//pm2.5
    QString quality;//空气质量

    QString type;//天气类型

    QString fx;//风向
    QString fl;//风力

    int high;//最高温度
    int low;//最低温度

};


class Day{
public:
    Day(){
        date = "2022-10-20";
        week = "周五";

        type = "多云";

        high = 0;
        low = 0;

        fx = "南风";
        fl = "2级";

        aqi = 0;
    }

    QString date;
    QString week;

    QString type;

    int high;
    int low;

    QString fx;
    QString fl;

    int aqi;//天气质量指数
};


#endif // WEATHERDATA_H
