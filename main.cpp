#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "contactmodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QGuiApplication::setApplicationDisplayName("Contact Book");
    QGuiApplication::setApplicationName("ContactBook");
    QGuiApplication::setApplicationVersion("1.0");
    QGuiApplication::setOrganizationName("ContactBook");
    QGuiApplication::setOrganizationDomain("contactbook@example.com");

    ContactModel model;

    QQmlApplicationEngine engine;

    qmlRegisterUncreatableType<ContactModel>("data.model", 1, 0, "ContactModel", "Not creatable as it is an enum type");

    engine.rootContext()->setContextProperty("contactModel", &model);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
