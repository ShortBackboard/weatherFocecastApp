/*
 * 将城市名转为citycode
 * 先读取包含所有城市对应的citycode的.json文件，保存键值对方便查找
 */


#ifndef CITYCODETOOL_H
#define CITYCODETOOL_H
#include <QMap>
#include <QFile>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QDebug>



class CitycodeTool{
private:

    static void initCityMap(){
        //1.读取文件
        QString filePath = "../weatherForecast/citycode.json";


        QFile file(filePath);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray citycodes =  file.readAll();
        file.close();

        //2.解析并且写入map
        QJsonParseError err;
        QJsonDocument doc =  QJsonDocument::fromJson(citycodes,&err);

        if(err.error != QJsonParseError::NoError){
            return;
        }

        if(!doc.isArray()){
            return;
        }

        QJsonArray cities = doc.array();
        for(int i = 0; i < cities.size(); i++){
            QString city = cities[i].toObject().value("city_name").toString();
            QString code = cities[i].toObject().value("city_code").toString();

            //判断，省不可查
            if(code.size() > 0){
            mCityMap.insert(city,code);
            }
        }

    }


public:
    static QString getCitycode(QString cityName){

        if(mCityMap.isEmpty()){
            initCityMap();
        }

        //查找
        QMap<QString,QString>::iterator it = mCityMap.find(cityName);

        //北京或者北京市
        if(it == mCityMap.end()){
            it = mCityMap.find(cityName + "市");
        }

        if(it != mCityMap.end()){

            return it.value();

        }

        return " ";
    }

private:
    static QMap<QString,QString> mCityMap;
};


//初始化
QMap<QString,QString> CitycodeTool::mCityMap = {};

#endif // CITYCODETOOL_H








































