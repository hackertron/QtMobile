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

#include <QJsonObject>
#include <QJsonDocument>

#include "notification.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("Mantra Chetna");
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
                   "`email` VARCHAR(100) NOT NULL , "
                   "`revision_id` INT NOT NULL );"))
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



    // insert into Mantra table
    QString id;
    int rev_id = 0;

    QSqlQuery quer("SELECT server_id, revision_id FROM User WHERE ID = 1");
    quer.exec();
    while (quer.next()) {
        id = quer.value(0).toString();
        rev_id = quer.value(1).toInt();
    }



    qDebug() << "got id : " << id;
    if(id.isEmpty())
    {
        qDebug() << "no insertions in User yet !!";
    }
    else
    {
        // create custom temporary event loop on stack
           QEventLoop eventLoop;

           // "quit()" the event-loop, when the network request "finished()"
           QNetworkAccessManager mgr;
           QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

           // the HTTP request
           const QUrl req_url = "http://vkguptamantra.herokuapp.com/api/users/" + id;
           QNetworkRequest req(req_url);
           QNetworkReply *reply = mgr.get(req);
           eventLoop.exec();


           if (reply->error() == QNetworkReply::NoError) {
               //success
               //qDebug() << "Success" <<reply->readAll();
               QString   result = (QString)reply->readAll();
               QJsonDocument jsonResponse = QJsonDocument::fromJson(result.toUtf8());

               QJsonObject jsonObj = jsonResponse.object();
               qDebug() << "server sent revision id : " <<  jsonObj.value("revision_id").toInt();
               qDebug() << "local rev id " << rev_id;

               if(rev_id == jsonObj.value("revision_id").toInt())
               {
                   qDebug() << "no updation needed";
               }
               else
               {
                   query.exec("DELETE FROM Mantra");
                   foreach (const QString& key, jsonObj.keys()) {

                       qDebug() << key << " : " << jsonObj.value(key);


                       QString email = jsonObj.value("email").toString();
                       if(key == "mantra1" || key == "mantra2" || key == "mantra3" || key == "mantra4" || key == "mantra5" || key == "mantra6"
                               || key == "mantra7" || key == "mantra8" || key == "mantra9")
                       {
                            bool enabled = jsonObj[key].toObject()["enabled"].toBool();
                            QString time = jsonObj[key].toObject()["time"].toString();

                            query.prepare("INSERT INTO Mantra (email, mantra_type, enabled, time) VALUES(:email, :mantra_type, :enabled, :time)");
                            query.bindValue(":email", email);
                            query.bindValue(":mantra_type", key);
                            query.bindValue(":enabled", enabled);
                            query.bindValue(":time", time);
                            if(query.exec())
                            {
                                qDebug() << "Successfully added in Mantra";
                            }
                       }




                   }

                   query.prepare("UPDATE User SET revision_id = :revid WHERE ID = 1");
                   int update_revision = jsonObj.value("revision_id").toInt();
                   query.bindValue(":revid", update_revision);

                   if(query.exec())
                   {
                       qDebug() << "Update complete !!";
                   }
                   else
                   {
                       qDebug() << "Errors" << query.lastError();
                   }

               }


               delete reply;
           }

           else {
               //failure
               qDebug() << "Failure" <<reply->errorString();
               delete reply;
           }



    }


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


           /* call notifications here */
           notification notify  ;
           notify.schedule(5,1);
           notify.schedule(10,2);




    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
