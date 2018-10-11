#ifndef JSONFUNCS_H
#define JSONFUNCS_H
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

inline bool missKey(const QJsonObject &json, QString key) {
    if (!json.contains(key)) {
        qWarning() << "QJsonObject не содержит " << key;
        return true;
    }
    return false;
}

inline bool missKeys(const QJsonObject &json, QStringList keys) {
    foreach (auto k, keys) {
        if (missKey(json, k)) {
            return true;
        }
    }
    return false;
}

#endif // JSONFUNCS_H
