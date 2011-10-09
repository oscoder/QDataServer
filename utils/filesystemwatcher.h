#ifndef UTILS_FILESYSTEMWATCHER_H
#define UTILS_FILESYSTEMWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT FileSystemWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemWatcher(QObject *parent = 0);
    explicit FileSystemWatcher(const QStringList &paths, QObject *parent = 0);
    ~FileSystemWatcher();

    void addPath(const QString &path);
    void addPaths(const QStringList &paths);
    void removePath(const QString &path);
    void removePaths(const QStringList &paths);

    QStringList paths() const;
    QStringList activePaths() const;

    bool active() const;

signals:
    void pathChanged(const QString &path) const;

private:
    class Watcher;
};

} // namespace Utils

#endif // UTILS_FILESYSTEMWATCHER_H
