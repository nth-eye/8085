#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QStringListModel>
#include <QQmlContext>

#include "doc_handler.h"
#include "settings.h"
#include "simulator.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    app.setOrganizationName("nth-eye");
    app.setOrganizationDomain("nth-eye");

    Highlighter::init();

    auto settings = std::make_unique<Settings>();
    auto simulator = std::make_unique<Simulator>();

    qmlRegisterType<DocHandler>("backend.doc_handler", 1, 0, "DocHandler");
    qmlRegisterSingletonInstance("backend.settings", 1, 0, "Settings", settings.get());
    qmlRegisterSingletonInstance("backend.simulator", 1, 0, "Simulator", simulator.get());

    QStringListModel list_model;
    list_model.setStringList(Settings::settings_list);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("list_model", &list_model);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    // settings.get()->reset();

    return app.exec();
}
