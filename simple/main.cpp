#include "common/common.h"
#include "user/userapi.h"
#include "event/eventapi.h"

static inline QString host(const QHttpServerRequest &request)
{
    return QString::fromLatin1(request.value("Host"));
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QHttpServer httpServer;

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../simple/calendardb.db");

    db.open();

    if(!db.isOpen())
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Failed to connect to database.");
    else
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Database connected.");

    //Common common;
    UserAPI userapi;
    EventAPI eventapi;

    //users get
    httpServer.route("/api/users", QHttpServerRequest::Method::Get,
                     [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.get(db,request);
    });

    //otherusers get
    httpServer.route("/api/otherusers", QHttpServerRequest::Method::Get,
                     [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.getOtherUsers(db,request);
    });

    //users get <id>
    httpServer.route("/api/users/<arg>", QHttpServerRequest::Method::Get,
                     [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.getUsersID(db,request);
    });

    //users post
    httpServer.route("/api/users", QHttpServerRequest::Method::Post,
                [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.post(db,request);
    });

    //users post login
    httpServer.route("/api/login", QHttpServerRequest::Method::Post,
                [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.postUsersLogin(db, request);
    });

    //users delete <username>
    httpServer.route("/api/users/<arg>", QHttpServerRequest::Method::Delete,
                [&db, &userapi](const QString username, const QHttpServerRequest &request) {
        return userapi.deleteUser(db, username, request);
    });

    //settings get
    httpServer.route("/api/settings", QHttpServerRequest::Method::Get,
                [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.getSettings(db, request);
    });

    //settings put
    httpServer.route("/api/settings", QHttpServerRequest::Method::Put,
                [&db, &userapi](const QHttpServerRequest &request) {
        return userapi.putSettings(db, request);
    });

    //events get
    httpServer.route("/api/events", QHttpServerRequest::Method::Get,
                     [&db, &eventapi](const QHttpServerRequest &request) {
        return eventapi.get(db,request);
    });

    //events get <id>
    httpServer.route("/api/events/<arg>", QHttpServerRequest::Method::Get,
                     [&db, &eventapi](int event_id, const QHttpServerRequest &request) {
        return eventapi.getEventsID(db, event_id, request);
    });

    //events post
    httpServer.route("/api/events", QHttpServerRequest::Method::Post,
                [&db, &eventapi](const QHttpServerRequest &request) {
        return eventapi.post(db,request);
    });

    //events put
    httpServer.route("/api/events/<arg>", QHttpServerRequest::Method::Put,
            [&db, &eventapi](int event_id, const QHttpServerRequest &request) {
        return eventapi.put(db, event_id, request);
    });

    //events delete <id>
    httpServer.route("/api/events/<arg>", QHttpServerRequest::Method::Delete,
            [&db, &eventapi](int event_id, const QHttpServerRequest &request) {
        return eventapi.deleteEventID(db, event_id, request);
    });

    //events get sparetime
    httpServer.route("/api/sparetime", QHttpServerRequest::Method::Get,
                     [&db, &eventapi](const QHttpServerRequest &request) {
        return eventapi.getSpareTime(db, request);
    });

    QHostAddress host("127.0.0.1");
    const auto port = httpServer.listen(host, 8080);
    if (!port) {
    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Server failed to listen on a port.");
    return 0;
    }

    const auto sslCertificateChain =
    QSslCertificate::fromPath(QStringLiteral(":/assets/certificate.crt"));
    if (sslCertificateChain.empty()) {
    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Couldn't retrive SSL certificate from file.");
    return 0;
    }

    QFile privateKeyFile(QStringLiteral(":/assets/private.key"));
    if (!privateKeyFile.open(QIODevice::ReadOnly)) {
    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Couldn't open file for reading.");
    return 0;
    }

    httpServer.sslSetup(sslCertificateChain.front(), QSslKey(&privateKeyFile, QSsl::Rsa));
    privateKeyFile.close();

    const auto sslPort = httpServer.listen(host, 8081);
    if (!sslPort) {
    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Server failed to listen on a port.");
    return 0;
    }

    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Running on http://127.0.0.1:%1/ and "
                                            "https://127.0.0.1:%2/ (Press CTRL+C to quit)")
    .arg(port)
    .arg(sslPort);

    return app.exec();
    }
