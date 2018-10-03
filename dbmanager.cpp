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


void dbmanager::subscribe_db(QString server_id, QString email, int revision_id)
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

    query.prepare("INSERT INTO User (server_id,email, revision_id) "
                  "VALUES (:server_id , :email, :revision_id )");



    query.bindValue(":server_id", server_id);
    query.bindValue(":email", email);
    query.bindValue(":revision_id", revision_id);




    if(query.exec())
    {
        qDebug() << "done";
        //  user is added now

    }
    else
    {
        qDebug() << query.lastError();


    }
    db.close();
}



QString return_diff(QHash<QString, QString> mantra)
{
    qDebug() << mantra;
    QTime sys_time;
    QHash<QString, QTime>result;
    sys_time =  QTime::currentTime();
    qDebug() << sys_time;
    QHashIterator<QString, QString> i(mantra);

    while(i.hasNext())
    {
        i.next();
        QString server_timeID = i.key();
        qDebug() << server_timeID;
        QString server_time = i.value();
        qDebug() << server_time;
        QStringList splitted_time = server_time.split(":");

        QString hrs = splitted_time.at(0);
        QString min = splitted_time.at(1);
        int int_hrs =hrs.toInt();
        int int_min = min.toInt();
        QTime notify_time(int_hrs,int_min,0,0);
        qDebug() << "created time : " << notify_time;
        if(notify_time >= sys_time)
        {
            result.insert(server_timeID,notify_time);
        }
        qDebug() << "result : " << result;


    }
    QString image;
    QHashIterator<QString, QTime> j(result);
    QTime min(0,0,0,0);
    while(j.hasNext())
    {
        if(j.value() < min)
        {
            image = j.key();
        }
    }


    return image;


}

QString dbmanager::getImage_url()
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
        qDebug() <<"connected to DB Mantra" ;

    }
    QString image;
    QHash<QString, QString> mantra;
    QSqlQuery query1("SELECT mantra_type, time FROM Mantra WHERE enabled = 1");
    while (query1.next()) {
        QString type = query1.value(0).toString();
        QString time  = query1.value(1).toString();

        qDebug() << type << " : " << time;
        mantra.insert(type,time);
    }

    image = return_diff(mantra);


    return image;

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
