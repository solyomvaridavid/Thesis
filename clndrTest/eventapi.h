#pragma once
#include <QObject>

struct Interval
{
    int start_time;
    int end_time;
};

class EventAPI : public QObject
{
    Q_OBJECT
public:
    EventAPI(QObject *parent = nullptr);
    QString find(int min_hour, int max_hour, int duration, QList<Interval>& intervals);
};
