#ifndef UTILS_STYLESHEETLOADER_H
#define UTILS_STYLESHEETLOADER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QMap>
#include <QSet>

#include "utils_global.h"

namespace Utils {

class FileSystemWatcher;

class UTILS_EXPORT StyleSheetLoader : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(StyleSheetLoader)

    explicit StyleSheetLoader();
    ~StyleSheetLoader();

public:
    static StyleSheetLoader *instance();
    static QString fixButtonText(const QString &text);

public:
    bool isStyleSheetSet() const;

    void setDefaultName(const QString &name);
    QString defaultName() const;

    void setPaths(const QStringList &paths);
    QStringList paths() const;

    QStringList names() const;
    QString activeName() const;

public slots:
    void load(const QString &name);
    void unload();
    void reload();

private:
    void restoreSettings();
    void saveSettings();

    void findFiles();

private slots:
    void startWatcherOnActiveFile();
    void stopWatcher();

private:
    const QString m_oldStyleSheet;
    const QPointer<FileSystemWatcher> m_fileWatcher;
    QSet<QString> m_paths;
    QString m_activeName;
    QString m_defaultName;
    QMap<QString, QString> m_namePathMap; // <name, path>
};

} // namespace Utils

#endif // UTILS_STYLESHEETLOADER_H
