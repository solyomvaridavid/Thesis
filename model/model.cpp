#include "model.h"

std::shared_ptr<Model> Model::instance;

std::shared_ptr<Model> Model::getInstance()
{
    if(instance == nullptr)
        instance = std::shared_ptr<Model>(new Model());

    return instance;
}

void Model::addEvent(const QString name, const QString start_date, const QString start_time,
                     const QString end_time, const QString label, const QString occurrence,
                     const QJsonArray jsonArray)
{
    QJsonObject obj;
    obj["name"] = name;
    obj["start_date"] = start_date;
    obj["start_time"] = start_time;
    obj["end_time"] = end_time;
    obj["label"] = label;
    obj["invitees"] = jsonArray;
    obj["occurrence"] = occurrence;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    const QUrl url(QStringLiteral("http://127.0.0.1:8080/api/events"));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.post(request, data);

    QObject::connect(reply,
                     &QNetworkReply::finished,
                     [this, reply] {
        if(reply->error() == QNetworkReply::NoError){
            emit eventAdded(std::nullopt);
        }
        else {
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            emit eventAdded(value);
        }
        reply->deleteLater();
    });
}

void Model::editEvent(const int eventId, const QString name, const QString start_date, const QString start_time, const QString end_time, const QString label, const QJsonArray jsonArray)
{
    const QUrl url("http://127.0.0.1:8080/api/events/" + QString::number(eventId));
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QJsonObject obj;
    obj["name"] = name;
    obj["start_date"] = start_date;
    obj["start_time"] = start_time;
    obj["end_time"] = end_time;
    obj["label"] = label;
    obj["invitees"] = jsonArray;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager.put(request, data);

    QObject::connect(reply,
                     &QNetworkReply::finished,
                     [=] {
        if(reply->error() == QNetworkReply::NoError){
            emit eventEdited(std::nullopt);
        }
        else{
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            emit eventEdited(value);
        }
        reply->deleteLater();
    });
}

void Model::logIntoCalendar(const QString username, const QString password)
{
    QJsonObject obj;
    obj["username"] = username;
    obj["password"] = password;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    const QUrl url(QStringLiteral("http://127.0.0.1:8080/api/login"));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.post(request, data);

    QObject::connect(reply,
                     &QNetworkReply::finished,
                     [=] {
        if(reply->error() == QNetworkReply::NoError){
            token = QString::fromUtf8(reply->readAll());
            emit tokenSetFinished();
            emit loginAttempt(std::nullopt);
        }
        else {
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            emit loginAttempt(value);
        }
        reply->deleteLater();
    });

}

void Model::createUser(const QString username, const QString password)
{
    const QUrl url(QStringLiteral("http://127.0.0.1:8080/api/users"));
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "users/json");

            QJsonObject obj;
            obj["username"] = username;
            obj["password"] = password;
            QJsonDocument doc(obj);
            QByteArray data = doc.toJson();

            QNetworkReply *reply = manager.post(request, data);

            QObject::connect(reply,
                             &QNetworkReply::finished,
                             [=] {
                if(reply->error() != QNetworkReply::NoError){
                    auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
                    int value = statusCode.toInt();
                    emit userCreated(value);
                    return;
                }
                emit userCreated(std::nullopt);
                reply->deleteLater();
            });

}

void Model::deleteUser(const QString current_item)
{
    const QUrl url("http://127.0.0.1:8080/api/users/" + current_item);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.deleteResource(request);

    QObject::connect(reply,
            &QNetworkReply::finished,
            [=] {
        if(reply->error() != QNetworkReply::NoError){
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            emit userDeleted(value);
        } else {
            emit userDeleted(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::usersListModelPopulate()
{
    const QUrl url(QStringLiteral("http://127.0.0.1:8080/api/otherusers"));
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply] {
        if(reply->error() != QNetworkReply::NoError){
            emit populatedUsersListModel(std::nullopt);
        } else {
            const QString jsonArrayStr = QString::fromUtf8(reply->readAll());
            QJsonArray jsonArray = QJsonDocument::fromJson(jsonArrayStr.toUtf8()).array();
            emit populatedUsersListModel(jsonArray);
        }
        reply->deleteLater();
    });
}

void Model::listModelPopulate(QDate date)
{
    QString d = date.toString("yyyy-MM-dd");

    QUrl url("http://127.0.0.1:8080/api/events?date=" + d);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply] {
        if(reply->error() != QNetworkReply::NoError){
            emit populatedListModel(std::nullopt);
        } else {
            const QString jsonArrayStr = QString::fromUtf8(reply->readAll());
            QJsonArray jsonArray = QJsonDocument::fromJson(jsonArrayStr.toUtf8()).array();
            emit populatedListModel(jsonArray);
        }
        reply->deleteLater();
    });
}

void Model::allEventsPopulate()
{
    QUrl url("http://127.0.0.1:8080/api/events");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [=] {
        if(reply->error() != QNetworkReply::NoError){
            emit populatedAllEventsListModel(std::nullopt);
        } else {
            const QString jsonArrayStr = QString::fromUtf8(reply->readAll());
            QJsonArray jsonArray = QJsonDocument::fromJson(jsonArrayStr.toUtf8()).array();
            emit populatedAllEventsListModel(jsonArray);
        }
        reply->deleteLater();
    });
}

void Model::fetchEvent(int id)
{
    QString event_id = QString::number(id);
    QUrl url("http://127.0.0.1:8080/api/events/" + event_id);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [=] {
        if(reply->error() != QNetworkReply::NoError){
            emit fetchedEditable(std::nullopt);
        }
        else{
            const QString jsonArrayStr = QString::fromUtf8(reply->readAll());
            QJsonArray jsonArray = QJsonDocument::fromJson(jsonArrayStr.toUtf8()).array();
            emit fetchedEditable(jsonArray);
        }
        reply->deleteLater();
    });
}

void Model::putSettings(const QString theme)
{
    QUrl url("http://127.0.0.1:8080/api/settings");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QJsonObject obj;
    obj["theme"] = theme;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager.put(request, data);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [=] {
        if(reply->error() == QNetworkReply::NoError){
            emit settingsPut(std::nullopt);
        }
        else{
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            emit settingsPut(value);
        }
        reply->deleteLater();
    });
}

void Model::applySettings()
{
    QUrl url("http://127.0.0.1:8080/api/settings");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply] {
        QString theme;
        if(reply->error() != QNetworkReply::NoError){
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            int value = statusCode.toInt();
            theme = "error";
        }
        else{
            theme = QString(reply->readAll());
        }
        emit settingsApplied(theme);
        reply->deleteLater();
    });
}

void Model::findTime(const int duration, const QString from_time, const QString til_time,
                     const QString invitees)
{
    //qDebug() << duration << from_time << til_time, invitees;
    const QUrl url("http://127.0.0.1:8080/api/sparetime?" + invitees + "&duration=" + QString::number(duration)  + "&from_time=" + from_time + "&til_time=" + til_time);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("token", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply] {
        if(reply->error() == QNetworkReply::NoError){
            auto event = QString::fromUtf8(reply->readAll());
            //qDebug() << event;
            foundEvent = event;
            emit findFinished(event);
        }
        else{
            auto event = "notime";
            emit findFinished(event);
        }
        reply->deleteLater();
    });

}

void Model::deleteEvent(const int eventId)
{
    const QUrl url("http://127.0.0.1:8080/api/events/" + QString::number(eventId));
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("token", token.toUtf8());

        QNetworkReply *reply = manager.deleteResource(request);

        connect(reply,
                &QNetworkReply::finished,
                this,
                [=] {
            if(reply->error() == QNetworkReply::NoError){
                emit eventDeleted(std::nullopt);
            }
            else{
                auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
                int value = statusCode.toInt();
                emit eventDeleted(value);
            }
            reply->deleteLater();
        });
}
