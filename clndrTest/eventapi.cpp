#include "eventapi.h"

EventAPI::EventAPI(QObject *parent)
    : QObject{parent}
{}

QString EventAPI::find(int min_hour, int max_hour, int duration, QList<Interval>& intervals)
{
    for (int current_min = min_hour; current_min <= max_hour - duration; current_min+=3600) {
        bool available = true;
        int current_max = current_min + duration;
        for (auto&& current_event : intervals) {
            available &= !(current_min >= current_event.start_time && current_max <=  current_event.end_time)
                    && !(current_event.start_time >= current_min  && current_event.end_time <= current_max);
        }
        if (available) {
            int hour = current_min / 3600;
            int min = (current_min - hour * 3600) / 60;
            int endh = (current_min + duration) / 3600;
            int endm = ((current_min + duration) - endh * 3600) / 60;
            QString h;
            QString m;
            QString eh;
            QString em;
            if (hour < 10)
                h = "0" + QString::number(hour) + ":";
            else
                h = QString::number(hour) + ":";
            if (min < 10)
                m = "0" + QString::number(min);
            else
                m = QString::number(min);

            if (endh < 10)
                eh = "0" + QString::number(endh) + ":";
            else
                eh = QString::number(endh) + ":";
            if (endm < 10)
                em = "0" + QString::number(endm);
            else
                em = QString::number(endm);
            return h+m + " - " + eh+em;
        }
        return "";
    }
}
