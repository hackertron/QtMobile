// ekke (Ekkehard Gentz) @ekkescorner
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QTranslator>
#include <QQmlContext>

#include "applicationui.hpp"

#include <QUrl>
#include <QStandardPaths>
#include <QFile>
#include <QDir>


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

    //standard path to store all the application data ( varies from OS to OS )
        QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        qDebug() << path;

        QDir dir(path);
           if (!dir.exists())
               dir.mkpath(path);
           if (!dir.exists("MANTRA_appdata"))
               dir.mkdir("MANTRA_appdata");

           dir.cd("MANTRA_appdata");



    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
