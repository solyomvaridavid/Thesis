#pragma once

#include "common/common.h"

class UserAPI : public Common
{
public:
    UserAPI();
    QHttpServerResponse get(QSqlDatabase db, const QHttpServerRequest &request) override;
    QHttpServerResponse post(QSqlDatabase db, const QHttpServerRequest &request) override;
    QHttpServerResponse postUsersLogin(QSqlDatabase db, const QHttpServerRequest &request);
    QHttpServerResponse getOtherUsers(QSqlDatabase db, const QHttpServerRequest &request);
    QHttpServerResponse getUsersID(QSqlDatabase db, const QHttpServerRequest &request);
    QHttpServerResponse deleteUser(QSqlDatabase db, const QString username, const QHttpServerRequest &request);
    QHttpServerResponse getSettings(QSqlDatabase db, const QHttpServerRequest &request);
    QHttpServerResponse putSettings(QSqlDatabase db, const QHttpServerRequest &request);

    QString generateRandomString(QString username);
};
