#include "dbmanager.h"
#include <QDebug>
#include <QEventLoop>
#include <QStandardPaths>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStandardPaths>




dbmanager::dbmanager(QObject *parent) : QObject(parent), webCtrl(new QNetworkAccessManager(this))
{

}

dbmanager::~dbmanager()
{
    delete webCtrl;
}


void dbmanager::subscribe_db(QString server_id, QString email)
{
    QString db_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    qDebug() << db_path;
    QDir dir(db_path);
    dir.cd("MANTRA_appdata");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//not dbConnection
       db.setDatabaseName(dir.absoluteFilePath("MANTRA.db"));
       if(!db.open())
       {
           qDebug() <<"error in opening DB";
       }
       else
       {
           qDebug() <<"connected to DB" ;

       }
       QSqlQuery query;

       qDebug() << server_id;
       qDebug() << email;

          query.prepare("INSERT INTO User (server_id,email) "
                        "VALUES (:server_id , :email )");



          query.bindValue(":server_id", server_id);
          query.bindValue(":email", email);




          if(query.exec())
          {
              qDebug() << "done";
              db.close(); //  user is added now

          }
          else
          {
              qDebug() << query.lastError();
              db.close();

          }
}

QString dbmanager::getImage_url()
{
    QString db_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString email = "";

    qDebug() << db_path;
    QDir dir(db_path);
    dir.cd("MANTRA_appdata");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//not dbConnection
       db.setDatabaseName(dir.absoluteFilePath("MANTRA.db"));
       if(!db.open())
       {
           qDebug() <<"error in opening DB";
       }
       else
       {
           qDebug() <<"connected to DB" ;

       }
       QSqlQuery query;

       query.prepare("SELECT email from User where ID = 1");

       if(query.exec())
       {
          qDebug() << "done";

       }
       query.prepare("SELECT mantra_type FROM Mantra WHERE enabled = true AND ");

       return email;

}

bool dbmanager::check_connection()
{
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply *reply = webCtrl->get(req);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if(reply->bytesAvailable())
    {
        qDebug() << "You are connected to the internet :)";
        return true;
    }
    else
    {
        qDebug() << "No internet connection ";
        return false;
    }


}


void dbmanager::downloadFile(QUrl url, QString id, QString dir_absolute_path, QString filename)
{
    qDebug() << url;

    QString path = dir_absolute_path + "/MANTRA_appdata/" + filename;
    qDebug() << path;
    bool connection = check_connection();
    if(connection)
    {
        QFile *file = new QFile(path, this);
        if(!file->open(QIODevice::WriteOnly))
        {
            return;
        }

        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", userAgent);

    //    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    //    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    //    sslConfiguration.setProtocol(QSsl::AnyProtocol);
    //    request.setSslConfiguration(sslConfiguration);

        QNetworkReply *reply = webCtrl->get(request);
        replytofile.insert(reply, file);
        replytopathid.insert(reply, QPair<QString, QString>(path, id));

        QObject::connect(reply, &QNetworkReply::finished, this, &dbmanager::fileDownloaded);
        QObject::connect(reply, &QNetworkReply::readyRead, this, &dbmanager::onReadyRead);
    }
    else
    {
        qDebug() << "No internet connection ";
    }


}

void dbmanager::fileDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (replytofile[reply]->isOpen())
    {
        replytofile[reply]->close();
        replytofile[reply]->deleteLater();
    }

    switch(reply->error())
    {
    case QNetworkReply::NoError:
        break;

    default:
        emit error(reply->errorString().toLatin1());
        break;
    }

    emit downloaded(replytopathid[reply].first, replytopathid[reply].second);

    replytofile.remove(reply);
    replytopathid.remove(reply);
    delete reply;
}

void dbmanager::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    replytofile[reply]->write(reply->readAll());
}
