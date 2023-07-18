#pragma once

#include "model.h"
#include <QAbstractListModel>

#include <QString>
#include <QDate>
#include <QTime>

struct Event
{
    int id;
    QString name;
    QDateTime start_date;
    QString start_time;
    QString end_time;
    QString label;
    int user;

    bool operator==(const Event& other) const
    {
        return id == other.id &&
               name == other.name &&
               start_date == other.start_date &&
               start_time == other.start_time &&
               end_time == other.end_time &&
               label == other.label &&
               user == other.user;
    }
};

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ListModel (std::shared_ptr<Model> model,
                        QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QList<Event> convertJsonArrayToList(QJsonArray jsonArray);
    void populate(QDate date = QDate());
    int getEventToBe() const {return eventToBe;}
    int getEventId(int row) const;
    void setEventToBe(int id) {eventToBe = id;}
    void setCurrentDate(QDate date);
    void fetchEditableEvent(int id);
    void setListenAllEvents(bool value) { listenAllEvents = value; }

signals:
    void queryFinished(Event e, QString inv);

public slots:
    void onRefreshEvent();

private:
    std::shared_ptr<Model> model;
    QList<Event> events;
    QDate currentDate;
    int eventToBe;
    bool listenAllEvents = false;
    QString inv = "";
};
