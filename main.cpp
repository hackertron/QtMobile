// ekke (Ekkehard Gentz) @ekkescorner
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QTranslator>
#include <QQmlContext>

#include "applicationui.hpp"

#include "dbmanager.h"

#include <QUrl>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>


int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("Mantra");
    QGuiApplication::setOrganizationName("rightanglemedia.in");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_QUICK_CONTROLS_STYLE", "material");
    QGuiApplication app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("bottom_nav_x"), QLatin1String("_"), QLatin1String(":/translations"))) {
        app.installTranslator(&translator);
    } else {
        qDebug() << "cannot load translator " << QLocale::system().name() << " check content of translations.qrc";
    }

    ApplicationUI appui;
    QQmlApplicationEngine engine;

    // from QML we have access to ApplicationUI as myApp
    QQmlContext* context = engine.rootContext();
    context->setContextProperty("myApp", &appui);


    // dbmanager object
    dbmanager dbman;
    context->setContextProperty("dbman", &dbman);

    //standard path to store all the application data ( varies from OS to OS )
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    qDebug() << path;

    context->setContextProperty("path", path);


    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(path);
    if (!dir.exists("MANTRA_appdata"))
        dir.mkdir("MANTRA_appdata");

    dir.cd("MANTRA_appdata");


            /*
             * creates sqlite DB to keep track of all the info offline
             * and also it's dependencies
             *
            */
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//not dbConnection
    db.setDatabaseName(dir.absoluteFilePath("MANTRA.db"));
    if(!db.open())
    {
        qDebug() <<"error";
    }
    else
    {
        qDebug() <<"connected" ;
    }

    QSqlQuery query;

    if( query.exec("CREATE TABLE IF NOT EXISTS `User`"
                   "(  `ID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT ,"
                   "`server_id` VARCHAR(32)  NOT NULL ,"
                   "`email` VARCHAR(100) NOT NULL);"))
    {
        qDebug() << "User table created";
    }
    else
    {
        qDebug() <<query.lastError();
    }

    if(query.exec("CREATE TABLE IF NOT EXISTS `Mantra` ("
                  "`mantra_ID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                  " `email` VARCHAR(100) NOT NULL,"
                  "`mantra_type` VARCHAR(30) NOT NULL,"
                  "`enabled` BOOLEAN NOT NULL,"
                  "`time` VARHCAR(10) NOT NULL);"))
    {
        qDebug() << "Mantra table created";

    }
    else
    {
        qDebug() <<query.lastError();
    }

    query.clear();
    db.close();

    /* *** Images download **/



           QVector<QString>images = {"Mantra1.png", "Mantra2.png", "Mantra3.png", "Mantra4.png", "Mantra5.png", "Mantra6.png",
                                            "Mantra7.png", "Mantra8.png", "Mantra9.png"};

           for(int i = 0; i < 9; i++)
           {
               if(QFile::exists(dir.absoluteFilePath(images[i]))) // checks if the image already exist
               {
                   qDebug() << " already downloaded " << images[i];
               }

               else{
                   QString url = "http://vkguptamantra.herokuapp.com/images/" + images[i];

                   QString id = (QString)i;
                   dbman.downloadFile(url, id, path, images[i]);

               }
           }




    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
