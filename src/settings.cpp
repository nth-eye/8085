#include "settings.h"
#include "highlighter.h"

const QStringList Settings::settings_list = {
    "editor/number",
    "editor/unit",
    "editor/instruction",
    "editor/register",
    "editor/directive",
    "editor/comment",
    "editor/label",
    // "editor/label-reference"
};

const QStringList Settings::settings_defaults = {
    "#FFB71D",
    "#E16325",
    "#CA9B55",
    "#50B4DC",
    "#F99157",
    "#51546A",
    "#FF435B",
//    "#EB606B"
};

Settings::Settings(QObject* parent) :
    QSettings(parent)
{}

Settings::~Settings()
{}

QVariant Settings::value(const QString &key, const QVariant &default_val)
{
    return QSettings::value(key, default_val);
}

QColor Settings::color_value(const QString &key, const QColor &default_val)
{
    return QSettings::value(key, default_val).value<QColor>();
}

void Settings::set_value(const QString &key, const QVariant &value)
{
    QSettings::setValue(key, value);

    // emit settingChanged(key);
}

void Settings::reset()
{
    for (int i = 0; i < settings_list.size(); ++i)
        setValue(settings_list[i], settings_defaults[i]);

    update();
}

void Settings::update()
{
    Highlighter::update();

    emit settingsChanged();
}

void Settings::sync()
{
    QSettings::sync();
}
