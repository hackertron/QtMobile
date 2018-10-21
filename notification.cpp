#include "notification.h"
#include <QDebug>
#include <QAndroidJniObject>
notification::notification()
{

}

void notification::schedule(int time, int id)
{
    qDebug() << id;
            QAndroidJniObject::callStaticMethod<void>
                                    ("in/rightanglemedia/mantrachetna/QtAndroidNotifications" // class name
                                    , "scheduleNotification" // method name
                                    , "(I)V" // signature
                                    ,time);

}
