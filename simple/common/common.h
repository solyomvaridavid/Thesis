#pragma once

#include <QtCore>
#include <QtHttpServer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMap>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>
#include <QSqlError>
#include <QSqlRecord>

struct User
{
    int id;
    QString username;
    QString password;
    QString email;
    QString token;
    QString created_at;
    QString expire_at;
    QString last_login;

    User(const int &id, const QString &username, const QString &password, const QString &email, const QString &last_login)
        : id(id), username(username), password(password), email(email), token(), created_at(), expire_at(), last_login(last_login)
    {}

    QJsonObject userToJson() const
    {
        return QJsonObject{ { "id", id },
                            { "username", username },
                            { "password", password },
                            { "email", email },
                            { "token", token },
                            { "created_at", created_at },
                            { "expire_at", expire_at },
                            { "last_login", last_login }};
    }
};

struct Event
{
    int id;
    QString name;
    QString start_date;
    QString start_time;
    QString end_time;
    QString label;
    int user;

    Event(const int &id, const QString &name, const QString &start_date, const QString &start_time, const QString &end_time, const QString &label, const int &user)
        :id(id), name(name), start_date(start_date), start_time(start_time), end_time(end_time), label(label), user(user)
    {}

    QJsonObject eventToJson() const
    {
        return QJsonObject{ { "id", id },
                            { "name", name },
                            { "start_date", start_date },
                            { "start_time", start_time },
                            { "end_time", end_time },
                            { "label", label },
                            { "user", user}};
    }
};

struct Interval
{
    int start_time;
    int end_time;
};

class Common
{
public:
    Common();
    virtual QHttpServerResponse get(QSqlDatabase db, const QHttpServerRequest &request) = 0;
    virtual QHttpServerResponse post(QSqlDatabase db, const QHttpServerRequest &request) = 0;
    int validateToken(QByteArray token, QSqlDatabase db);
    static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &arr);
    QString hashString(const QString& stringToHash);
};
