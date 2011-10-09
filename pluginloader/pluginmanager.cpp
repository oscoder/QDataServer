#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <QtCore/QDir>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QSettings>
#include <QtGui/QApplication>

#include <utils/iprogressmonitor.h>

#include "iplugin.h"
#include "pluginspec.h"

using namespace PluginLoader;

enum {
    debugPluginManager = 0
};

PluginManager::PluginManager()
    : d_ptr(new PluginManagerPrivate(this))
{
    Q_D(PluginManager);
    d->restoreSettings();
}

PluginManager::~PluginManager()
{
    Q_D(PluginManager);
    d->saveSettings();
    delete d;
}

/*!
    The PluginManager is a singleton. Use this method to get an instance.
    \return PluginManager's instance
 */
PluginManager *PluginManager::instance()
{
    static PluginManager instance;
    return &instance;
}

/*!
    Returs all paths where to search for plugins.
    \return plugin paths
 */
QStringList PluginManager::getPluginPaths()
{
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    const QString searchPath =
        rootDirPath + '/' + QString(UITOOLS_REL_PLUGINS_DIR);
    return QStringList() << searchPath;
}

/*!
    Searches all the given \a paths for valid application's plugins. Once the
    dependencies among plugins are resolved the plugins are loaded in found
    order.
    \param paths the list of paths where to search for plugins
 */
void PluginManager::loadPlugins(const QStringList &paths)
{
    Q_D(PluginManager);
    return d->loadPlugins(paths);
}

/*!
    Returns the list of successfully loaded plugins.
    \return the list of loaded plugins
 */
QList<IPlugin *> PluginManager::plugins() const
{
    Q_D(const PluginManager);
    return d->plugins();
}

/*!
    Tries to initialize all loaded plugins.
    \return true if all loaded plugins were successfully initialized
    \sa IPlugin::initialize()
 */
bool PluginManager::initializePlugins(Utils::IProgressMonitor *monitor)
{
    Q_D(PluginManager);
    return d->initializePlugins(monitor);
}

/*!
    In case some plugin initialization failed and the reason is too critical,
    plugin may request application shutdown.
    Optional argument \a pluginName is filled by plugin name that has requested
    shutdown and can be used in a message shown to user.
    \param pluginName if not null, filled by plugin name which needs shutdown
    \return true if any plugin requested shutdown
 */
bool PluginManager::isShutdownRequested(QString *pluginName)
{
    Q_D(PluginManager);

    if (pluginName != 0)
        *pluginName = d->pluginWhichRequestedShutdown;

    return !d->pluginWhichRequestedShutdown.isEmpty();
}

/*!
    Unloads all loaded plugins. Before each plugin is unloaded the method
    IPlugin::shutdown() is called.
    \sa IPlugin::shutdown()
 */
void PluginManager::unloadPlugins()
{
    Q_D(PluginManager);
    QList<PluginSpec *> queue = d->unloadQueue();
    return d->unloadPlugins(queue);
}

/*!
    Returns the list of plugin specifications for successfully loaded plugins.
    The specification is taken from plugin's description file.
    \return the list of plugin specifications
    \sa PluginSpec
 */
QList<PluginSpec *> PluginManager::pluginSpecs() const
{
    Q_D(const PluginManager);
    return d->pluginSpecs();
}

/*!
    Returns the plugin specification for given \a plugin.
    \param plugin the instance of loaded plugin
    \return the plugin specification
 */
PluginSpec *PluginManager::pluginSpec(IPlugin *plugin) const
{
    Q_D(const PluginManager);
    return d->pluginSpec(plugin);
}

bool PluginManager::isPluginLoaded(const QString &pluginName) const
{
    Q_D(const PluginManager);

    foreach (IPlugin *plugin, d->plugins()) {
        if (pluginSpec(plugin)->name() == pluginName)
            return true;
    }

    return false;
}

PluginManagerPrivate::PluginManagerPrivate(PluginManager *q)
    : q_ptr(q)
{
}

PluginManagerPrivate::~PluginManagerPrivate()
{
    QList<IPlugin *> plugins = m_pluginToSpec.uniqueKeys();
    plugins.removeAll(0);
    if (plugins.count() > 0) {
        qWarning("%d unloaded plugins left left in memory:", plugins.count());
        foreach (IPlugin *plugin, plugins) {
            qWarning("  - %s", qPrintable(m_pluginToSpec.value(plugin)->name()));
        }
    }
    qDeleteAll(m_pluginToSpec);
    m_pluginToSpec.clear();
}

void PluginManagerPrivate::loadPlugins(const QStringList &paths)
{
    Q_ASSERT(!paths.isEmpty());
    Q_ASSERT(m_pluginToSpec.isEmpty());

    readPluginSpecs(paths);
    resolveDependencies();
    QList<PluginSpec *> pluginLoadQueue = loadQueue();

    foreach (PluginSpec *pluginSpec, pluginLoadQueue) {
        IPlugin *plugin = pluginSpec->loadPlugin();
        if (plugin != 0) {
            m_pluginToSpec.remove(0, pluginSpec);
            m_pluginToSpec.insert(plugin, pluginSpec);
        }
    }
}

QList<IPlugin *> PluginManagerPrivate::plugins() const
{
    QList<IPlugin *> plugins = m_pluginToSpec.uniqueKeys();
    plugins.removeAll(0);

    return plugins;
}

bool PluginManagerPrivate::initializePlugins(Utils::IProgressMonitor *monitor)
{
    Q_Q(PluginManager);
    QList<PluginSpec *> pluginLoadQueue = loadQueue();
    bool allInitialized = true;
    pluginWhichRequestedShutdown.clear();
    foreach (PluginSpec *pluginSpec, pluginLoadQueue) {
        if (pluginSpec->state() == PluginSpec::Loaded) {
            monitor->setStatus(pluginSpec->name());
            bool initialized = pluginSpec->initializePlugin();
            if (!initialized) {
                allInitialized = false;

                // unload dependent plugins
                QList<PluginSpec *> queue;
                QList<PluginSpec *> circularity;
                //shutdown requested, unload all plugins and terminate app
                if (pluginSpec->plugin()->isShutdownRequested()) {
                    pluginWhichRequestedShutdown = pluginSpec->name();
                    return false;
                }
                else {
                    pluginSpec->unloadQueue(queue, circularity);
                    unloadPlugins(queue);
                    // update 'IndirectlyDisabled' state of dependent plugins
                    pluginSpec->resolveIndirectlyDisabled(true);
                }
            }
        }
    }
    emit q->pluginsInitialized();
    return allInitialized;
}

void PluginManagerPrivate::unloadPlugins(QList<PluginSpec *> unloadQueue)
{
    foreach (PluginSpec *pluginSpec, unloadQueue) {
        m_pluginToSpec.remove(pluginSpec->plugin(), pluginSpec);
        pluginSpec->unloadPlugin();
        m_pluginToSpec.insert(0, pluginSpec);
    }
}

QList<PluginSpec *> PluginManagerPrivate::pluginSpecs() const
{
    return m_pluginToSpec.values();
}

PluginSpec *PluginManagerPrivate::pluginSpec(IPlugin *plugin) const
{
    Q_ASSERT(plugin != 0);
    return m_pluginToSpec.value(plugin);
}

void PluginManagerPrivate::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("PluginManager");

    m_disabledPlugins = settings.value(
            QLatin1String("PluginSpec.DisabledPlugins")).toStringList();

    settings.endGroup(); // PluginManager
    if (debugPluginManager) {
        qDebug("PluginManager: Settings restored");
    }
}

void PluginManagerPrivate::saveSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("PluginManager"));

    QStringList tempDisabledPlugins;
    QList<PluginSpec *> pluginSpecs = m_pluginToSpec.values();

    foreach (PluginSpec *spec, pluginSpecs) {
        if (!spec->isEnabled()) {
            tempDisabledPlugins.append(spec->name());
        }
    }
    tempDisabledPlugins.removeDuplicates();

    settings.setValue(
            QLatin1String("PluginSpec.DisabledPlugins"), tempDisabledPlugins);

    settings.endGroup(); // PluginManager
    if (debugPluginManager) {
        qDebug("PluginManager: Settings saved");
    }
}

void PluginManagerPrivate::readPluginSpecs(const QStringList &paths)
{
    qDeleteAll(m_pluginToSpec);
    m_pluginToSpec.clear();

    QStringList specFileNames;
    QStringList searchPaths = paths;

    while (!searchPaths.isEmpty()) {
        const QDir dir(searchPaths.takeFirst());

        const QStringList nameFilters = QStringList() << "*.spec";
        const QFileInfoList files =
                dir.entryInfoList(nameFilters, QDir::Readable | QDir::Files);
        foreach (const QFileInfo &file, files) {
            specFileNames << file.absoluteFilePath();
        }

        const QFileInfoList subDirs = dir.entryInfoList(
                QDir::Readable | QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QFileInfo &subDir, subDirs) {
            searchPaths << subDir.absoluteFilePath();
        }
    }

    foreach (const QString &specFileName, specFileNames) {
        PluginSpec *pluginSpec = new PluginSpec();
        if (pluginSpec->read(specFileName)) {
            m_pluginToSpec.insert(0, pluginSpec);
        }
    }
}

void PluginManagerPrivate::resolveDependencies()
{
    const QList<PluginSpec *> pluginSpecs = m_pluginToSpec.values();
    foreach (PluginSpec *pluginSpec, pluginSpecs) {
        if (m_disabledPlugins.contains(pluginSpec->name())) {
            pluginSpec->setEnabled(false);
        }
        pluginSpec->resolveDependecies(pluginSpecs);
    }
    foreach (PluginSpec *pluginSpec, pluginSpecs) {
        pluginSpec->resolveIndirectlyDisabled(true);
    }
}

QList<PluginSpec *> PluginManagerPrivate::loadQueue()
{
    QList<PluginSpec *> queue;

    /*
       We need to sort pluginSpecs in ascending order of pluginSpec.name
       to ensure the same load order everywhere.
     */
    QMap<QString, PluginSpec *> pluginSpecs;
    foreach (PluginSpec *pluginSpec, m_pluginToSpec) {
        pluginSpecs.insert(pluginSpec->name(), pluginSpec);
    }

    foreach (PluginSpec *pluginSpec, pluginSpecs) {
        QList<PluginSpec *> circularityCheckQueue;
        if (pluginSpec->state() >= PluginSpec::Resolved)
            pluginSpec->loadQueue(queue, circularityCheckQueue);
    }

    if (debugPluginManager)
        qWarning() << "Load queue: " << queue;

    return queue;
}

QList<PluginSpec *> PluginManagerPrivate::unloadQueue()
{
    QList<PluginSpec *> queue;
    foreach (PluginSpec *pluginSpec, m_pluginToSpec) {
        QList<PluginSpec *> circularityCheckQueue;
        if (pluginSpec->state() >= PluginSpec::Loaded)
            pluginSpec->unloadQueue(queue, circularityCheckQueue);
    }

    if (debugPluginManager)
        qWarning() << "Unload queue: " << queue;

    return queue;
}
