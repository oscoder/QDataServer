#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "pluginloader_global.h"

namespace Utils {
    class IProgressMonitor;
}
namespace PluginLoader {

class IPlugin;
class PluginSpec;

class PluginManagerPrivate;

/*!
    \brief The manager that takes care of plugins.

    The PluginManager class is usually the first manager you use in application
    which is able to localize and recognize all valid plugins for this
    application.
    \sa IPlugin
 */
class PLUGINLOADER_EXPORT PluginManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginManager)

private:
    explicit PluginManager();
    virtual ~PluginManager();

public:
    static PluginManager *instance();
    static QStringList getPluginPaths();

    void loadPlugins(const QStringList &paths);
    QList<IPlugin *> plugins() const;

    bool initializePlugins(Utils::IProgressMonitor *monitor);
    bool isShutdownRequested(QString *pluginName = 0);

    void unloadPlugins();

    QList<PluginSpec *> pluginSpecs() const;
    PluginSpec *pluginSpec(IPlugin *plugin) const;

    bool isPluginLoaded(const QString &pluginName) const;

signals:
    //! Emitted after all plugins were successfully initialized.
    void pluginsInitialized();

private:
    Q_DECLARE_PRIVATE(PluginManager)
    PluginManagerPrivate *d_ptr;
};

} // namespace PluginLoader

#endif // PLUGINMANAGER_H
