#include "listmodel.h"

ListModel::ListModel(std::shared_ptr<Model> model,
                     QObject *parent)
    : QAbstractListModel(parent)
    , model(model)
{}

QList<Event> ListModel::convertJsonArrayToList(QJsonArray jsonArray)
{
    QList<Event> e;

    for (const auto& value : jsonArray) {
        if (value.isObject()) {
            const auto& obj = value.toObject();

            Event event;
            event.id = obj["id"].toInt();
            event.name = obj["name"].toString();
            event.user = obj["user"].toInt();
            const QString start_date_str = obj["start_date"].toString();
            event.start_date = QDateTime::fromString(start_date_str, "yyyy-MM-dd");
            event.start_time = obj["start_time"].toString();
            event.end_time = obj["end_time"].toString();
            event.label = obj["label"].toString();

            if (obj.contains("invitees")) {
                inv= obj["invitees"].toString();
            }

            if(event.name.length() < 15)
                for (int i = event.name.length(); i < 15; ++i) {
                    event.name = event.name + " ";
                }

            e.append(event);
        }
    }
    return e;
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return events.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (index.row() >= events.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const auto& event = events.at(index.row());
        return tr("%1|  %2 %3-%4 |%5").arg(event.name,
                                           event.start_date.toString("yyyy-MM-dd"),
                                           event.start_time,
                                           event.end_time,
                                           event.label);
    }
    return QVariant();
}

void ListModel::populate(QDate date)
{
    if (date.isValid()) {
        model->listModelPopulate(date);
    } else {
        model->allEventsPopulate();
    }
}

int ListModel::getEventId(int row) const
{
    return events[row].id;
}

void ListModel::setCurrentDate(QDate date)
{
    currentDate = date;
    populate(currentDate);
}

void ListModel::fetchEditableEvent(int id)
{
    model->fetchEvent(id);
}

void ListModel::onRefreshEvent()
{
    populate(currentDate);
}
