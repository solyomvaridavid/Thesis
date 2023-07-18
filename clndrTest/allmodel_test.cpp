#include "model.h"
#include "listmodel.h"
#include "userslistmodel.h"
#include "eventapi.h"

#include <QtTest>

class AllModelTest : public QObject
{
    Q_OBJECT

private slots:
    void testUsersConvertJsonArrayToList();
    void testEventsConvertJsonArrayToList();
    void testFindTime();
    void testFindNoTime();
};

void AllModelTest::testEventsConvertJsonArrayToList()
{
    ListModel listModel(Model::getInstance());

    QJsonArray jsonArray;
    QJsonObject jsonObject;
    jsonObject["end_time"] = "21:00";
    jsonObject["id"] = 342;
    jsonObject["invitees"] = "";
    jsonObject["label"] = "Personal";
    jsonObject["name"] = "Thesis writing";
    jsonObject["start_date"] = "2023-05-28";
    jsonObject["start_time"] = "17:00";
    jsonObject["user"] = 7;
    jsonArray.append(jsonObject);

    Event e;
    e.id = 342;
    e.name = "Thesis writing ";
    QString startdate = "2023-05-28";
    e.start_date = QDateTime::fromString(startdate, "yyyy-MM-dd");
    e.start_time = "17:00";
    e.end_time = "21:00";
    e.label =  "Personal";
    e.user = 7;
    QList<Event> testList;
    testList.append(e);


    QList<Event>events;
    events = listModel.convertJsonArrayToList(jsonArray);

    QCOMPARE(events, testList);
}

void AllModelTest::testUsersConvertJsonArrayToList()
{
    UsersListModel usersListModel(Model::getInstance());

    QJsonArray jsonArray;
    QJsonObject jsonObject;
    jsonObject["id"] = 8;
    jsonObject["username"] = "mike";
    jsonObject["last_login"] = "";
    jsonArray.append(jsonObject);

    User u;
    u.id = 8;
    u.username = "mike           ";
    u.last_login = "";
    QList<User> testList;
    testList.append(u);

    QList<User>users;
    users = usersListModel.convertJsonArrayToList(jsonArray);

    QCOMPARE(users, testList);
}

void AllModelTest::testFindTime()
{
    EventAPI eventapi;
    int duration = 3600;
    int from = 10800;
    int to = 21600;

    QList<Interval> intervals;
    Interval i1;
    i1.start_time = 25200;
    i1.end_time = 61200;
    Interval i2;
    i2.start_time = 72000;
    i2.end_time = 79200;
    intervals.append(i1);
    intervals.append(i2);

    QString res = eventapi.find(from, to, duration, intervals);

    QString str = "03:00 - 04:00";
    QCOMPARE(res, str);
}

void AllModelTest::testFindNoTime()
{
    EventAPI eventapi;
    int duration = 3600;
    int from = 10800;
    int to = 21600;

    QList<Interval> intervals;
    Interval i1;
    i1.start_time = 7200;
    i1.end_time = 72000;
    intervals.append(i1);

    QString res = eventapi.find(from, to, duration, intervals);

    QString str = "";
    QCOMPARE(res, str);
}

QTEST_APPLESS_MAIN(AllModelTest)

#include "allmodel_test.moc"
