#ifndef PLUGINLOADER_IPLUGIN_H
#define PLUGINLOADER_IPLUGIN_H

#include <QtCore/QtPlugin>

#include "pluginloader_global.h"

namespace PluginLoader {

/*!
    \brief The general abstract API for plugin.

    The IPlugin class is an interface that must be implemented once for each
    plugin.
    A plugin consists of two parts: A description file, and a library that at
    least contains the IPlugin implementation.
 */
class PLUGINLOADER_EXPORT IPlugin
{
public:
    virtual ~IPlugin() {}

    /*!
        The plugin should allocate all resources and initialize its internal
        state in this method. The initialize methods of plugins that depend on
        this plugin are called after the initialize method of this plugin has
        been called. If initialization wasn't successful, the \a errorString
        should be set to a user-readable message describing the reason.
        \param errorString possible error message
        \return true if plugin was successfully initialized
     */
    virtual bool initialize(QString *errorString = 0) = 0;
    /*!
        The plugin should store its internal state and release all allocated
        resources in this method. The shutdown methods of plugins are called
        in reverse order contrary to initialization.
     */
    virtual void shutdown() = 0;
    /*!
        If initialization fails, the plugin can request shutdown whole
        application.
     */
    virtual bool isShutdownRequested() { return false; }
};

} // namespace PluginLoader

Q_DECLARE_INTERFACE(PluginLoader::IPlugin,
        "cn.oscoder.QDataServer.IPlugin/1.0");

#endif // PLUGINLOADER_IPLUGIN_H
