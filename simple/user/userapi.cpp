#include "userapi.h"

UserAPI::UserAPI(){}

QString UserAPI::generateRandomString(QString username)
{
    const QString timestamp = QString::number(QDateTime::currentSecsSinceEpoch());
    return hashString(username + timestamp);
}

QHttpServerResponse UserAPI::get(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QJsonArray array;
    QSqlQuery query(db);
    query.exec("SELECT id, username, password, email, last_login FROM users");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString username = query.value(1).toString();
        QString password = query.value(2).toString();
        QString email = query.value(3).toString();
        QString last_login = query.value(5).toString();
        User user(id, username, password, email, last_login);
        QJsonObject obj;
        obj = user.userToJson();
        array.push_back(obj);
    }
    return QHttpServerResponse(array);
}

QHttpServerResponse UserAPI::post(QSqlDatabase db, const QHttpServerRequest &request)
{
    const auto json = byteArrayToJsonObject(request.body());
    if (!json|| !json->contains("username") || !json->contains("password"))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
    QSqlQuery q(db);
    q.prepare("SELECT username FROM users WHERE username LIKE :username");
    q.bindValue(":username", json->value("username").toString());
    if (q.exec() && q.first())
    {
        QString u = q.value(0).toString();
        if (!(u.isEmpty()))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, role) "
                          "VALUES (:username, :password, :role)");
    query.bindValue(":username", json->value("username").toString());
    query.bindValue(":password", hashString(json->value("password").toString()));
    query.bindValue(":role", "user");
    query.exec();
    return QHttpServerResponse(QHttpServerResponder::StatusCode::Created);
}

QHttpServerResponse UserAPI::postUsersLogin(QSqlDatabase db, const QHttpServerRequest &request)
{
    const auto json = byteArrayToJsonObject(request.body());
    if (!json || !json->contains("username")|| !json->contains("password"))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

    QSqlQuery query(db);
    query.prepare("SELECT id, password FROM users WHERE username LIKE :username");
    query.bindValue(":username", json->value("username").toString());
    query.exec();
    if(query.next())
    {
        query.first();
        QString pwd = query.value(1).toString();
        QString p = hashString(json->value("password").toString());
        int x = QString::compare(pwd, p, Qt::CaseInsensitive);
        if (x != 0)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        QString token = generateRandomString(json->value("username").toString());
        QString expire_at = QString::number(QDateTime::currentSecsSinceEpoch() + 3600);

        QSqlQuery token_query(db);
        token_query.prepare("UPDATE users SET token = :token, expire_at = :expire_at, last_login = :last_login WHERE id = :id");
        token_query.bindValue(":id", query.value(0));
        token_query.bindValue(":token", token);
        token_query.bindValue(":expire_at", expire_at);
        token_query.bindValue(":last_login", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        token_query.exec();
        return QHttpServerResponse(token);
    } else {
        return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
    }
}

QHttpServerResponse UserAPI::getOtherUsers(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QJsonArray array;
    QSqlQuery query(db);
    query.prepare("SELECT id, username, password, email, last_login "
                  "FROM users "
                  "WHERE id <> :id AND role LIKE :role");
    query.bindValue(":id", user_id);
    query.bindValue(":role", "user");
    query.exec();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString username = query.value(1).toString();
        QString password = query.value(2).toString();
        QString email = query.value(3).toString();
        QString last_login = query.value(4).toString();
        User user(id, username, password, email, last_login);
        QJsonObject obj;
        obj = user.userToJson();
        array.push_back(obj);
    }
    return QHttpServerResponse(array);
}

QHttpServerResponse UserAPI::getUsersID(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QJsonArray array;
    QSqlQuery query(db);
    query.prepare("SELECT id, username, password, email, last_login FROM users WHERE id = :id");
    query.bindValue(":id", user_id);
    if(query.exec() != false)
    {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString username = query.value(1).toString();
            QString password = query.value(2).toString();
            QString email = query.value(3).toString();
            QString last_login = query.value(4).toString();
            User user(id, username, password, email, last_login);
            QJsonObject obj;
            obj = user.userToJson();
            array.push_back(obj);
        }
        return QHttpServerResponse(array);
    }
    return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
}

QHttpServerResponse UserAPI::deleteUser(QSqlDatabase db, const QString username, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT username FROM users WHERE username LIKE :username");
    checkQuery.bindValue(":username",username);
    checkQuery.exec();
    if (checkQuery.next()) {
        QSqlQuery query(db);
        query.prepare("DELETE FROM users WHERE username LIKE :username");
        query.bindValue(":username",username);
        if (query.exec() != true)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);

        return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
    }
    else {
        return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
    }
}

QHttpServerResponse UserAPI::getSettings(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QString theme = "";
    QSqlQuery query(db);
    query.prepare("SELECT theme "
               "FROM users "
               "WHERE id = :id");
    query.bindValue(":id", user_id);
    query.exec();
    while (query.next()) {
        theme = query.value(0).toString();
    }
    return QHttpServerResponse(theme);
}

QHttpServerResponse UserAPI::putSettings(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    const auto json = byteArrayToJsonObject(request.body());
    if (!json || !json->contains("theme"))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

    QString theme = json->value("theme").toString();

    QSqlQuery query(db);
    query.prepare("UPDATE users "
                  "SET theme = :theme "
                  "WHERE id = :user_id");
    query.bindValue(":theme", theme);
    query.bindValue(":user_id", user_id);
    if(query.exec())
    {
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
    }
    else
    {
        return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
    }
}
