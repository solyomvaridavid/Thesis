#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include <condition_variable>
#include <mutex>

class Model : public QObject
{
    Q_OBJECT

public:
    static std::shared_ptr<Model> getInstance();

private:
    Model(){}

signals:
    void eventAdded(std::optional<int>);
    void eventEdited(std::optional<int>);
    void eventDeleted(std::optional<int>);
    void loginAttempt(std::optional<int>);
    void userCreated(std::optional<int>);
    void userDeleted(std::optional<int>);
    void populatedListModel(std::optional<QJsonArray>);
    void populatedUsersListModel(std::optional<QJsonArray>);
    void populatedAllEventsListModel(std::optional<QJsonArray>);
    void fetchedEditable(std::optional<QJsonArray>);
    void settingsPut(std::optional<int>);
    void settingsApplied(QString theme);
    void tokenSetFinished();
    void findFinished();

public:
    QString getToken() { return token; }
    QString getEvent() { return foundEvent; }
    void addEvent(const QString name, const QString start_date, const QString start_time,
                  const QString end_time, const QString label, const QString occurrence,
                  const QJsonArray jsonArray);
    void editEvent(const int eventId, const QString name, const QString start_date,
                   const QString start_time, const QString end_time, const QString label,
                   const QJsonArray jsonArray);
    void deleteEvent(const int eventId);
    void logIntoCalendar(const QString username, const QString password);
    void createUser(const QString username, const QString password);
    void deleteUser(const QString current_item);
    void usersListModelPopulate();
    void listModelPopulate(QDate date);
    void allEventsPopulate();
    void fetchEvent(int id);
    void putSettings(const QString theme);
    void applySettings();

    QString findTime(const int duration, const QString from_time, const QString til_time,
                  const QString invitees);

private:
    QNetworkAccessManager manager;
    QString token;
    QString foundEvent;

private:
    static std::shared_ptr<Model> instance;
};

