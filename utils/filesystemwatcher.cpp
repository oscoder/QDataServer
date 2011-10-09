#include "filesystemwatcher.h"

#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtGui/QApplication>

using namespace Utils;

/*! \cond false */
class FileSystemWatcher::Watcher : public QObject
{
    Q_OBJECT

private:
    Watcher();

public:
    ~Watcher();

public:
    static Watcher *instance();

public:
    void registerPaths(const QSet<QString> &paths, const FileSystemWatcher
            *pathsOwner);
    void unregisterPaths(const QSet<QString> &paths, const FileSystemWatcher
            *pathsOwner);

    QSet<QString> paths(const FileSystemWatcher *pathsOwner) const;
    QSet<QString> activePaths(const FileSystemWatcher *pathsOwner) const;

    bool active() const;

private:
    QSet<const FileSystemWatcher *> pathOwners(const QString &path) const;
    void synchronize();

private slots:
    void onPathChanged(const QString &path);
    void onFocusChanged(QWidget *old, QWidget *now);
    void onPathOwnerDestroyed(QObject *object);

private:
    static QPointer<Watcher> m_instance;
    QPointer<QFileSystemWatcher> m_watcher;
    //   path     path owners
    QMap<QString, QSet<const FileSystemWatcher *> > m_map;
    bool m_active;
    QSet<QString> m_pathsChanged;
};
/*! \endcond */

/*!
 * \class Utils::FileSystemWatcher
 * \brief Provides an interface for monitoring file system for modifications
 *
 * The FileSystemWatcher provides an interface for monitoring files and
 * directories for modifications similary to Qt's QFileSystemWatcher. It uses
 * single instance of QFileSystemWatcher to save system resources. The biggest
 * benefit is that FileSystemWatcher is active only when its application is
 * focused.
 *
 * That mechanism is useful for instance in cases where editor has opened
 * document, user changes that document from another editor and then activates
 * first editor. Editor asks user whether to reload document modified outside or
 * not only after editor is focused, not immediately after the change is made.
 *
 * Delayed notification when the application is not active helps a lot in cases
 * where opened document which is modified outside the application doesn't store
 * changed directly to the file but stores data to backup file, deletes original
 * file and moves temporary file to regular one (e.g. <em>vim</em> editor).
 */

//! Creates watcher with given \a parent
FileSystemWatcher::FileSystemWatcher(QObject *parent)
    : QObject(parent)
{
}

//! Created watched with given \a parent and adds \a paths
/*!
 * \sa addPaths
 */
FileSystemWatcher::FileSystemWatcher(const QStringList &paths, QObject *parent)
    : QObject(parent)
{
    addPaths(paths);
}

//! Destroys the file system watcher
/*!
 * All registered paths that were not removed are automatically unregistered.
 * Compared to QFileSystemWatcher provided by Qt it is not necessary to take
 * care about possible deadlocks on shutdown. It's handled internally.
 *
 * \sa ~QFileSystemWatcher
 */
FileSystemWatcher::~FileSystemWatcher()
{
}

//! Register new \a path to be watched
/*!
 * Simplified method for single \a path given.
 *
 * \sa addPaths
 * \sa removePath
 * \sa removePaths
 */
void FileSystemWatcher::addPath(const QString &path)
{
    if (path.isEmpty()) {
        qWarning("%s: path is empty", Q_FUNC_INFO);
        return;
    }
    addPaths(QStringList(path));
}

//! Register new \a paths to be watched
/*!
 * Given \a paths are added to the list of paths that should be watched.
 * For paths that do exist watching is started. If file or directory disappears
 * active watching is stopped but path is not removed from internal list. Once
 * the file/directory is recreated and internal synchronization is done watching
 * is started again.
 *
 * Internal synchronization is done when some path is added/removed or
 * application get back lost focus.
 *
 * Empty list or empty path strings are ignored (warning sent to output).
 *
 * \sa addPath
 * \sa removePath
 * \sa removePaths
 */
void FileSystemWatcher::addPaths(const QStringList &paths)
{
    if (paths.isEmpty()) {
        qWarning("%s: list with paths is empty", Q_FUNC_INFO);
        return;
    }

    QSet<QString> _paths_ = paths.toSet();
    if (_paths_.contains("")) {
        qWarning("%s: list with paths contains empty paths", Q_FUNC_INFO);
        _paths_.remove("");
    }
    Watcher::instance()->registerPaths(_paths_, this);
}

//! Removes \a path from watcher
/*!
 * Simplified method for single \a path given.
 *
 * \sa addPath
 * \sa addPaths
 * \sa removePaths
 */
void FileSystemWatcher::removePath(const QString &path)
{
    if (path.isEmpty()) {
        qWarning("%s: path is empty", Q_FUNC_INFO);
        return;
    }
    removePaths(QStringList(path));
}

//! Removes \a paths from watcher
/*!
 * Given \a paths are removed from the list of paths that should be watched.
 *
 * Empty list or empty path strings are ignored (warning sent to output).
 *
 * \sa addPath
 * \sa addPaths
 * \sa removePath
 */
void FileSystemWatcher::removePaths(const QStringList &paths)
{
    if (paths.isEmpty()) {
        qWarning("%s: list with paths is empty", Q_FUNC_INFO);
        return;
    }

    QSet<QString> _paths_ = paths.toSet();
    if (_paths_.contains("")) {
        qWarning("%s: list with paths contains empty paths", Q_FUNC_INFO);
        _paths_.remove("");
    }
    Watcher::instance()->unregisterPaths(_paths_, this);
}

//! Returns all registered paths for this watcher
/*!
 * Returned list contains all registered paths that should be watched. Those
 * paths may or may not exist in file system.
 *
 * \sa activePaths
 */
QStringList FileSystemWatcher::paths() const
{
    return Watcher::instance()->paths(this).toList();
}

//! Returns only active registered paths for this watcher
/*!
 * Returned list contains only registered paths that are actively watched. Those
 * paths always exists in file system.
 *
 * \sa paths
 */
QStringList FileSystemWatcher::activePaths() const
{
    return Watcher::instance()->activePaths(this).toList();
}

//! Returns current state of the watcher
/*!
 * File system watcher is active if one of application windows is active (has
 * focus).
 */
bool FileSystemWatcher::active() const
{
    return Watcher::instance()->active();
}

/*!
 * \fn FileSystemWatcher::pathChanged(const QString &path) const
 *
 * Signal is emitted only in active state whenever any path returned by
 * activePath method is modified, renamed or deleted.
 *
 * \sa active
 * \sa activePaths
 */

/*
 * \class Utils::FileSystemWatcher::Watcher
 */

/*! \cond false */

QPointer<FileSystemWatcher::Watcher> FileSystemWatcher::Watcher::m_instance = 0;

FileSystemWatcher::Watcher::Watcher()
    : QObject(),
    m_active(false)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(onFocusChanged(QWidget*,QWidget*)));

    connect(qApp, SIGNAL(aboutToQuit()),
            this, SLOT(deleteLater()));

    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(onPathChanged(QString)));
    connect(m_watcher, SIGNAL(fileChanged(QString)),
            this, SLOT(onPathChanged(QString)));
}

FileSystemWatcher::Watcher::~Watcher()
{
    m_instance = 0;
    delete m_watcher;
    m_watcher = 0;
}

FileSystemWatcher::Watcher *FileSystemWatcher::Watcher::instance()
{
    if (m_instance == 0)
        m_instance = new Watcher;
    return m_instance;
}

void FileSystemWatcher::Watcher::registerPaths(const QSet<QString> &paths, const
        FileSystemWatcher *pathsOwner)
{
    if (this->paths(pathsOwner).isEmpty()) // Register new owner
        connect(pathsOwner, SIGNAL(destroyed(QObject*)),
                this, SLOT(onPathOwnerDestroyed(QObject*)));

    foreach (const QString &path, paths) {
        QSet<const FileSystemWatcher *> pathOwners = this->pathOwners(path);
        pathOwners.insert(pathsOwner);
        m_map[path] = pathOwners;
    }

    synchronize();
}

void FileSystemWatcher::Watcher::unregisterPaths(const QSet<QString> &paths,
        const FileSystemWatcher *pathsOwner)
{
    foreach (const QString &path, paths) {
        QSet<const FileSystemWatcher *> pathOwners = this->pathOwners(path);
        pathOwners.remove(pathsOwner);
        if (pathOwners.isEmpty()) {
            m_map.remove(path);
            m_pathsChanged.remove(path);
        }
        else {
            m_map[path] = pathOwners;
        }
    }

    if (this->paths(pathsOwner).isEmpty()) // Unregister owner
        disconnect(pathsOwner, SIGNAL(destroyed(QObject*)),
                this, SLOT(onPathOwnerDestroyed(QObject*)));

    synchronize();
}

QSet<QString> FileSystemWatcher::Watcher::paths(const FileSystemWatcher
        *pathsOwner) const
{
    QSet<QString> paths;
    const QStringList keys = m_map.keys();
    foreach (const QString &path, keys) {
        if (m_map.value(path).contains(pathsOwner))
            paths.insert(path);
    }
    return paths;
}

QSet<QString> FileSystemWatcher::Watcher::activePaths(const FileSystemWatcher
        *pathsOwner) const
{
    const QSet<QString> paths = this->paths(pathsOwner);

    const QSet<QString> activeFiles =
        paths & m_watcher->files().toSet();
    const QSet<QString> activeDirectories =
        paths & m_watcher->directories().toSet();

    return activeFiles | activeDirectories;
}

bool FileSystemWatcher::Watcher::active() const
{
    return m_active;
}

QSet<const FileSystemWatcher *> FileSystemWatcher::Watcher::pathOwners(const
        QString &path) const
{
    if (m_map.contains(path))
        return m_map.value(path);
    return QSet<const FileSystemWatcher *>();
}

void FileSystemWatcher::Watcher::synchronize()
{
    const QSet<QString> allActivePaths =
        m_watcher->files().toSet() | m_watcher->directories().toSet();

    const QSet<QString> paths = m_map.keys().toSet();
    const QSet<QString> pathsToRemove = allActivePaths - paths;
    QSet<QString> pathsToAdd;
    foreach (const QString &path, paths) {
        if (!allActivePaths.contains(path) && QFileInfo(path).exists())
            pathsToAdd.insert(path);
    }
    if (!pathsToRemove.isEmpty())
        m_watcher->removePaths(pathsToRemove.toList());
    if (!pathsToAdd.isEmpty())
        m_watcher->addPaths(pathsToAdd.toList());
}

void FileSystemWatcher::Watcher::onPathChanged(const QString &path)
{
    /*! \todo Qt doesn't remove paths immediately in some cases.
            Change back to assert once it is fixed. */
    if (!m_map.keys().contains(path))
        return;
    //Q_ASSERT(m_map.keys().contains(path));

    /*! \todo Remove and add again otherwise it doesn't work properly in some
            cases. That's a Qt bug. Remove those lines once it is fixed. */
    m_watcher->removePath(path);
    if (QFileInfo(path).exists())
        m_watcher->addPath(path);

    if (m_active) {
        const QSet<const FileSystemWatcher *> pathOwners =
            this->pathOwners(path);
        foreach (const FileSystemWatcher *pathOwner, pathOwners) {
            emit pathOwner->pathChanged(path);
        }
    }
    else {
        m_pathsChanged.insert(path);
    }
}

void FileSystemWatcher::Watcher::onFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);
    Q_UNUSED(now);

    if (m_active == (qApp->activeWindow() != 0))
        return;

    m_active = qApp->activeWindow() != 0;

    synchronize();

    if (!m_active)
        return;

    foreach (const QString &path, m_pathsChanged) {
        onPathChanged(path);
    }
    m_pathsChanged.clear();
}

void FileSystemWatcher::Watcher::onPathOwnerDestroyed(QObject *object)
{
    const FileSystemWatcher *pathsOwner =
        static_cast<const FileSystemWatcher *>(object);
    Q_ASSERT(pathsOwner != 0);

    const QStringList keys = m_map.keys();
    QSet<QString> paths;
    foreach (const QString &path, keys) {
        QSet<const FileSystemWatcher *> pathOwners = m_map.value(path);
        if (pathOwners.contains(pathsOwner))
            paths.insert(path);
    }

    if (!paths.isEmpty())
        unregisterPaths(paths, pathsOwner);

    synchronize();
}

/*! \endcond */

#include "filesystemwatcher.moc"
