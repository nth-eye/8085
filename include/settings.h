#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
#include <QHash>
#include <QSettings>

class Settings : public QSettings {
    Q_OBJECT
public:
    Settings(QObject *parent = 0);
    virtual ~Settings();

    Q_INVOKABLE void set_value(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &default_val = QVariant());
    Q_INVOKABLE QColor color_value(const QString &key, const QColor &default_val = QColor());
    Q_INVOKABLE void reset();
    Q_INVOKABLE void update();
    Q_INVOKABLE void sync();

    const static QStringList settings_list;
    const static QStringList settings_defaults;
signals:
    void settingChanged(const QString& key);
    void settingsChanged();
};

#endif // SETTINGS_H
