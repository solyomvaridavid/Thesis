#pragma once

#include "common/common.h"

class EventAPI : public Common
{
public:
    EventAPI();
    QHttpServerResponse get(QSqlDatabase db, const QHttpServerRequest &request) override;
    QHttpServerResponse post(QSqlDatabase db, const QHttpServerRequest &request) override;
    QHttpServerResponse put(QSqlDatabase db, int event_id, const QHttpServerRequest &request);
    QHttpServerResponse getEventsID(QSqlDatabase db, int event_id, const QHttpServerRequest &request);
    QHttpServerResponse deleteEventID(QSqlDatabase db, int event_id, const QHttpServerRequest &request);
    QHttpServerResponse getSpareTime(QSqlDatabase db, const QHttpServerRequest &request);

    int timeToSec(QString str);
    QString find(int min_hour, int max_hour, int duration, QList<Interval>& intervals);
};
