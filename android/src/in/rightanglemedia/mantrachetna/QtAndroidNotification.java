package in.rightanglemedia.mantrachetna;
// Qt

import org.qtproject.qt5.android.QtNative;

// android
import android.app.AlarmManager;
import android.content.Intent;
import android.content.Context;
import android.app.PendingIntent;
import android.app.Notification;
import android.app.NotificationManager;

// java
import java.lang.String;

class QtAndroidNotifications {


    //TODO : Send the EPOCH time in milliseconds to this method at which the alarm is to be scheduled
    public static void scheduleNotification(int time) {
        Context context = QtNative.activity();
        long timer = time * 1000;
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);

        Intent intent = new Intent(context, NotificationReceiver.class);

        PendingIntent pi = PendingIntent.getBroadcast(context, 12345, intent, PendingIntent
                .FLAG_UPDATE_CURRENT);

        alarmManager.setExact(AlarmManager.RTC_WAKEUP, timer, pi);

    }


}
