#include "eventapi.h"

EventAPI::EventAPI(){}

int EventAPI::timeToSec(QString str){
    int seconds;
    QStringList list = str.split(":");
    seconds = list[0].toInt() * 3600 + list[1].toInt() * 60;
    return seconds;
}

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

QHttpServerResponse EventAPI::get(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QString str = "SELECT * "
                  "FROM events "
                  "LEFT JOIN users_to_events ON (events.id = users_to_events.event_id OR events.root_id = users_to_events.event_id) "
                  "WHERE (users_to_events.user_id = :user_id OR events.user = :user_id)";
    if (request.query().hasQueryItem("date"))
    {
        str = str + " AND start_date LIKE :start_date";
    }
    str = str + " GROUP BY events.id";
    QJsonArray array;
    QSqlQuery query(db);
    query.prepare(str);
    query.bindValue(":user_id", user_id);
    if (request.query().hasQueryItem("date"))
    {
        query.bindValue(":start_date", request.query().queryItemValue("date"));
    }
    query.exec();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString start_date = query.value(2).toString();
        QString start_time = query.value(3).toString();
        QString end_time = query.value(4).toString();
        QString label = query.value(6).toString();
        int user = query.value(5).toInt();
        Event event(id, name, start_date, start_time, end_time, label, user);
        QJsonObject obj;
        obj = event.eventToJson();
        array.push_back(obj);
    }
    return QHttpServerResponse(array);
}

QHttpServerResponse EventAPI::post(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    const auto json = byteArrayToJsonObject(request.body());
    if (!json || !json->contains("name")|| !json->contains("start_date")|| !json->contains("start_time")||
                 !json->contains("end_time") || !json->contains("label") || !json->contains("invitees") ||
                 !json->contains("occurrence"))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

    int root_id;

    QString occ = json->value("occurrence").toString();

    if (occ == "None") {
        QSqlQuery query(db);
        query.prepare("INSERT INTO events (name, start_date, start_time, end_time, label, user) "
                              "VALUES (:name, :start_date, :start_time, :end_time, :label, :user)");
        query.bindValue(":name", json->value("name").toString());
        query.bindValue(":start_date", json->value("start_date").toString());
        query.bindValue(":start_time", json->value("start_time").toString());
        query.bindValue(":end_time", json->value("end_time").toString());
        query.bindValue(":label", json->value("label").toString());
        query.bindValue(":user", user_id);
        query.exec();

        root_id = query.lastInsertId().toInt();
    }
    else if (occ == "Daily") {
        QSqlQuery query(db);
        query.prepare("INSERT INTO events (name, start_date, start_time, end_time, label, user) "
                              "VALUES (:name, :start_date, :start_time, :end_time, :label, :user)");
        query.bindValue(":name", json->value("name").toString());
        query.bindValue(":start_date", json->value("start_date").toString());
        query.bindValue(":start_time", json->value("start_time").toString());
        query.bindValue(":end_time", json->value("end_time").toString());
        query.bindValue(":label", json->value("label").toString());
        query.bindValue(":user", user_id);
        query.exec();

        root_id = query.lastInsertId().toInt();

        QString start_date = json->value("start_date").toString();
        QDate sDate = QDate::fromString(start_date,"yyyy-MM-dd");
        QDate eDate = {sDate.year(), 12, 31};
        int days = sDate.daysTo(eDate);

        for (int i = 1; i < days + 1; ++i) {

            QSqlQuery reocc(db);
            reocc.prepare("INSERT INTO events (name, start_date, start_time, end_time, label, user, root_id) "
                                  "VALUES (:name, :start_date, :start_time, :end_time, :label, :user, :root_id)");
            reocc.bindValue(":name", json->value("name").toString());
            reocc.bindValue(":start_date", sDate.addDays(i).toString("yyyy-MM-dd"));
            reocc.bindValue(":start_time", json->value("start_time").toString());
            reocc.bindValue(":end_time", json->value("end_time").toString());
            reocc.bindValue(":label", json->value("label").toString());
            reocc.bindValue(":user", user_id);
            reocc.bindValue(":root_id", root_id);
            reocc.exec();
        }
    }
    else if (occ == "Weekly") {
        QSqlQuery query(db);
        query.prepare("INSERT INTO events (name, start_date, start_time, end_time, label, user) "
                              "VALUES (:name, :start_date, :start_time, :end_time, :label, :user)");
        query.bindValue(":name", json->value("name").toString());
        query.bindValue(":start_date", json->value("start_date").toString());
        query.bindValue(":start_time", json->value("start_time").toString());
        query.bindValue(":end_time", json->value("end_time").toString());
        query.bindValue(":label", json->value("label").toString());
        query.bindValue(":user", user_id);
        query.exec();

        root_id = query.lastInsertId().toInt();

        QString start_date = json->value("start_date").toString();
        QDate sDate = QDate::fromString(start_date,"yyyy-MM-dd");
        QDate eDate = {sDate.year(), 12, 31};
        int numOccurrences = 1 + ((eDate.toJulianDay() - sDate.toJulianDay()) / 7);

        for (int i = 1; i < numOccurrences; ++i) {
            QSqlQuery reocc(db);
            reocc.prepare("INSERT INTO events (name, start_date, start_time, end_time, label, user, root_id) "
                                  "VALUES (:name, :start_date, :start_time, :end_time, :label, :user, :root_id)");
            reocc.bindValue(":name", json->value("name").toString());
            reocc.bindValue(":start_date", sDate.addDays(i*7).toString("yyyy-MM-dd"));
            reocc.bindValue(":start_time", json->value("start_time").toString());
            reocc.bindValue(":end_time", json->value("end_time").toString());
            reocc.bindValue(":label", json->value("label").toString());
            reocc.bindValue(":user", user_id);
            reocc.bindValue(":root_id", root_id);
            reocc.exec();
        }
    }

    QJsonArray jsonArray = json->value("invitees").toArray();
    QList<QString> invitees;
    for (int i = 0; i < jsonArray.count(); ++i) {
        invitees.append(jsonArray.at(i).toString());
    }

    QList<int> ids;
    for (auto& invitee : invitees) {
        QSqlQuery q(db);
        q.prepare("SELECT id FROM users WHERE username LIKE :username");
        q.bindValue(":username", invitee);
        if(q.exec() == false)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
        q.first();
        int id = q.value(0).toInt();
        ids.append(id);
    }

    for (auto& id : ids) {
        QSqlQuery qq(db);
        qq.prepare("INSERT INTO users_to_events (user_id, event_id) VALUES (:user_id, :event_id)");
        qq.bindValue(":user_id", id);
        qq.bindValue(":event_id", root_id);
        if(qq.exec() == false)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
    }

    return QHttpServerResponse(QHttpServerResponder::StatusCode::Created);
}

QHttpServerResponse EventAPI::put(QSqlDatabase db, int event_id, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    const auto json = byteArrayToJsonObject(request.body());
    if (!json || !json->contains("name")|| !json->contains("start_date")|| !json->contains("start_time")|| !json->contains("end_time") || !json->contains("label") || !json->contains("invitees"))
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

    QSqlQuery correctUserQuery(db);
    correctUserQuery.prepare("SELECT user FROM events WHERE id = :event");
    correctUserQuery.bindValue(":event", event_id);
    correctUserQuery.exec();
    while (correctUserQuery.next()) {
        int user = correctUserQuery.value(0).toInt();
        if (user != user_id)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Forbidden);
    }
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM users_to_events WHERE event_id = :event");
    checkQuery.bindValue(":event", event_id);
    checkQuery.exec();

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QSqlQuery q(db);
        q.prepare("DELETE FROM users_to_events WHERE event_id = :event");
        q.bindValue(":event", event_id);
        if (q.exec()) {
            QSqlQuery query(db);
            query.prepare("UPDATE events SET name = :name, start_date = :start_date, start_time = :start_time, end_time = :end_time, label = :label, user = :user WHERE id = :id");
            query.bindValue(":id", event_id);
            query.bindValue(":name", json->value("name").toString());
            query.bindValue(":start_date", json->value("start_date").toString());
            query.bindValue(":start_time", json->value("start_time").toString());
            query.bindValue(":end_time", json->value("end_time").toString());
            query.bindValue(":label", json->value("label").toString());
            query.bindValue(":user", user_id);
            if(query.exec())
            {
                int event = event_id;

                QJsonArray jsonArray = json->value("invitees").toArray();
                QList<QString> invitees;
                for (int i = 0; i < jsonArray.count(); ++i) {
                    invitees.append(jsonArray.at(i).toString());
                }

                QList<int> ids;
                for (auto& invitee : invitees) {
                    QSqlQuery q(db);
                    q.prepare("SELECT id FROM users WHERE username LIKE :username");
                    q.bindValue(":username", invitee);
                    if(q.exec() == false)
                        return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
                    q.first();
                    int id = q.value(0).toInt();
                    ids.append(id);
                }

                for (auto& id : ids) {
                    QSqlQuery qq(db);
                    qq.prepare("INSERT INTO users_to_events (user_id, event_id) VALUES (:user_id, :event_id)");
                    qq.bindValue(":user_id", id);
                    qq.bindValue(":event_id", event);
                    if(qq.exec() == false)
                        return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
                }
                return QHttpServerResponse(QHttpServerResponder::StatusCode::Created);
            }
            else
            {
                return QHttpServerResponse(QHttpServerResponder::StatusCode::InternalServerError);
            }
        }
        else {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::InternalServerError);
        }
    } else {
        QSqlQuery query(db);
        query.prepare("UPDATE events SET name = :name, start_date = :start_date, start_time = :start_time, end_time = :end_time, label = :label, user = :user WHERE id = :id");
        query.bindValue(":id", event_id);
        query.bindValue(":name", json->value("name").toString());
        query.bindValue(":start_date", json->value("start_date").toString());
        query.bindValue(":start_time", json->value("start_time").toString());
        query.bindValue(":end_time", json->value("end_time").toString());
        query.bindValue(":label", json->value("label").toString());
        query.bindValue(":user", user_id);
        if(query.exec())
        {
            int event = event_id;

            QJsonArray jsonArray = json->value("invitees").toArray();
            QList<QString> invitees;
            for (int i = 0; i < jsonArray.count(); ++i) {
                invitees.append(jsonArray.at(i).toString());
            }

            QList<int> ids;
            for (auto& invitee : invitees) {
                QSqlQuery q(db);
                q.prepare("SELECT id FROM users WHERE username LIKE :username");
                q.bindValue(":username", invitee);
                if(q.exec() == false)
                    return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
                q.first();
                int id = q.value(0).toInt();
                ids.append(id);
            }

            for (auto& id : ids) {
                QSqlQuery qq(db);
                qq.prepare("INSERT INTO users_to_events (user_id, event_id) VALUES (:user_id, :event_id)");
                qq.bindValue(":user_id", id);
                qq.bindValue(":event_id", event);
                if(qq.exec() == false)
                    return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
            }
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Created);
        }
        else
        {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::InternalServerError);
        }
    }
}

QHttpServerResponse EventAPI::getEventsID(QSqlDatabase db, int event_id, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QJsonArray array;
    QSqlQuery query(db);
    query.prepare("SELECT events.id, name, start_date, start_time, end_time, label, user, "
                  "GROUP_CONCAT(users_to_events.user_id, ',') AS invitees "
                  "FROM events LEFT JOIN users_to_events ON events.id = users_to_events.event_id "
                  "WHERE events.id = :id GROUP BY events.id");
    query.bindValue(":id", event_id);
    query.exec();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString start_date = query.value(2).toString();
        QString start_time = query.value(3).toString();
        QString end_time = query.value(4).toString();
        QString label = query.value(5).toString();
        int user = query.value(6).toInt();
        QString invitees = query.value(7).toString();

        QSqlQuery i(db);
        i.prepare("SELECT username "
                  "FROM users "
                  "WHERE id IN ("+ invitees +")");
        i.exec();
        QString s;
        while (i.next()){
            QString name = i.value(0).toString();
            s += name + ',';
        }
        s.remove(s.length() - 1, 1);
        QJsonObject invObj;
        QString key = "invitees";
        QString value = s;
        invObj[key] = value;
        Event event(id, name, start_date, start_time, end_time, label, user);
        QJsonObject obj;
        obj = event.eventToJson();
        for (auto it = invObj.constBegin(); it != invObj.constEnd(); ++it) {
            obj.insert(it.key(), it.value());
        }
        array.push_back(obj);
    }
    return QHttpServerResponse(array);
}

QHttpServerResponse EventAPI::deleteEventID(QSqlDatabase db, int event_id, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id, name, user FROM events WHERE id = :id AND user = :user");
    checkQuery.bindValue(":id",event_id);
    checkQuery.bindValue(":user",user_id);
    checkQuery.exec();
    if (checkQuery.next()) {
        QSqlQuery query(db);
        query.prepare("DELETE FROM events WHERE (id = :id OR root_id = :id) AND user = :user");
        query.bindValue(":id",event_id);
        query.bindValue(":user",user_id);
        if (query.exec() != true)
            return QHttpServerResponse(QHttpServerResponder::StatusCode::NoContent);

        return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);
    }
    else {
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Forbidden);
    }
}

QHttpServerResponse EventAPI::getSpareTime(QSqlDatabase db, const QHttpServerRequest &request)
{
    int user_id = validateToken(request.value("token"), db);
    if(user_id == 0)
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);

    QDate from_date = QDate::currentDate().addDays(1);
    QDate til_date = from_date.addDays(14);
    QList<QString> dates;

    QDate current_date = from_date;
    while (current_date <= til_date) {
        dates.append(current_date.toString("yyyy-MM-dd"));
        current_date = current_date.addDays(1);
    }

    auto duration = request.query().queryItemValue("duration").toInt();
    auto from_time = timeToSec(request.query().queryItemValue("from_time"));
    auto til_time= timeToSec(request.query().queryItemValue("til_time"));

    QList<QString> invitees = request.query().allQueryItemValues("invitees[]");
    QString idstring = QString::number(user_id) + ",";
    for(auto&& i : invitees)
    {
        int id = 0;
        QSqlQuery idquery(db);
        idquery.prepare("SELECT id "
                      "FROM users "
                      "WHERE username LIKE :username");
        idquery.bindValue(":username", i.trimmed());
        if(idquery.exec() != false)
        {
            while (idquery.next()) {
               id = idquery.value(0).toInt();
            }
        }
        idstring += QString::number(id) + ",";
    }
    idstring = idstring.remove(idstring.size()-1, 1);
    QList<Interval> intervals;
    for (auto&& day : dates) {
        QSqlQuery query(db);
        query.prepare("SELECT events.start_time, events.end_time "
                      "FROM events "
                      "LEFT JOIN users_to_events ON (events.id = users_to_events.event_id OR events.root_id = users_to_events.event_id) "
                      "WHERE (users_to_events.user_id IN ("+ idstring +") OR events.user IN ("+ idstring +")) AND (date(start_date) LIKE date(:day))"
                      "GROUP BY events.id "
                      "ORDER BY start_date, start_time ASC");
        query.bindValue(":day", day);
        if(query.exec() != false)
        {
            while (query.next()) {
                Interval i;
                i.start_time = timeToSec(query.value(0).toString());
                i.end_time = timeToSec(query.value(1).toString());
                intervals.append(i);
            }
            QString s = find(from_time, til_time, duration, intervals);
            intervals.clear();
            if (s != "") {
                day += " " + s;
                return QHttpServerResponse(day);
            }
        }
    }
    return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
}
