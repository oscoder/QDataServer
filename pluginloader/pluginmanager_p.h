#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H
/*! \cond __pimpl */

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "pluginmanager.h"

namespace PluginLoader {

class IPlugin;
class PluginSpec;

class PluginManagerPrivate
{
public:
    PluginManagerPrivate(PluginManager *q);
    virtual ~PluginManagerPrivate();

    void loadPlugins(const QStringList &paths);
    QList<IPlugin *> plugins() const;

    bool initializePlugins(Utils::IProgressMonitor *splash);

    void unloadPlugins(QList<PluginSpec *> unloadQueue);

    QList<PluginSpec *> pluginSpecs() const;
    PluginSpec *pluginSpec(IPlugin *plugin) const;

    void restoreSettings();
    void saveSettings();

private:
    void readPluginSpecs(const QStringList &paths);
    void resolveDependencies();
    QList<PluginSpec *> loadQueue();
    QList<PluginSpec *> unloadQueue();

private:
    Q_DECLARE_PUBLIC(PluginManager)
    PluginManager *q_ptr;

    QMultiMap<IPlugin *, PluginSpec *> m_pluginToSpec;
    QStringList m_disabledPlugins;
    QString pluginWhichRequestedShutdown;
};

} // namespace PluginLoader

/*! \endcond */
#endif // PLUGINMANAGER_P_H
