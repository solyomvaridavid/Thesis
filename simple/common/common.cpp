#include "common.h"

Common::Common(){}

int Common::validateToken(QByteArray token, QSqlDatabase db)
{
    QSqlQuery q(db);
    q.prepare("SELECT id, expire_at FROM users WHERE token LIKE :token");
    q.bindValue(":token", QString::fromStdString(token.toStdString()));
    q.exec();
    if(!q.first())
        return 0;
    if(q.value(1).toInt() < QDateTime::currentSecsSinceEpoch())
        return 0;
    return q.value(0).toInt();
}

std::optional<QJsonObject> Common::byteArrayToJsonObject(const QByteArray &arr)
{
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isObject())
        return std::nullopt;
    return json.object();
}

QString Common::hashString(const QString& stringToHash)
{
    QByteArray stringData = stringToHash.toUtf8();
    QByteArray hashData = QCryptographicHash::hash(stringData, QCryptographicHash::Sha256);
    QString hash = QString(hashData.toHex());
    return hash;
}
