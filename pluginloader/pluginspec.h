#ifndef PLUGINLOADER_PLUGINSPEC_H
#define PLUGINLOADER_PLUGINSPEC_H

#include <QtCore/QMetaType>
#include <QtCore/QObject>

#include "pluginloader_global.h"

namespace PluginLoader {

class IPlugin;

//! The structure holding the data important for solving dependency.
struct PLUGINLOADER_EXPORT PluginDependency
{
    //! Plugin's name
    QString name;
    //! Plugin's verison
    QString version;
};

class PluginSpecPrivate;

/*!
    \brief Contains the information from the plugins xml description file.

    The plugin specification is also filled with more information as the plugin
    goes through its loading process (see PluginSpec::State). If an error
    occurs, the plugin spec is the place to look for the error details.
 */
class PLUGINLOADER_EXPORT PluginSpec : public QObject
{
    Q_OBJECT

public:
    /*!
        The plugin goes through several steps while being loaded.
        The state gives a hint on what went wrong in case of an error.
     */
    enum State {
        /*!
            Starting point: Even the xml description file was not read.
         */
        Invalid,
        /*!
            The xml description file has been successfully read, and its
            information is available via the PluginSpec.
         */
        Read,
        /*!
            The dependencies given in the description file have been successfully
            found, and are available via the dependencySpecs() method.
         */
        Resolved,
        /*!
            The plugin's library is loaded and the plugin instance created
            (available through plugin()).
         */
        Loaded,
        /*!
            The plugin instance's IPlugin::initialize() method has been called
            and returned a success value.
         */
        Initialized
    };

    explicit PluginSpec();
    virtual ~PluginSpec();

    bool read(const QString &fileName);
    bool resolveDependecies(const QList<PluginSpec *> &specs);
    void resolveIndirectlyDisabled(bool forceResolve = false);
    bool loadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
            &circularityCheckQueue);
    bool unloadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
            &circularityCheckQueue);

    // Information from the xml file, valid after 'Read' state is reached
    QString name() const;
    QString version() const;
    QString description() const;
    QString category() const;
    QList<PluginDependency> dependencies() const;

    QString filePath() const;
    QString fileName() const;

    // Dependency specs, valid after 'Resolved' state is reached
    QList<PluginSpec *> dependencySpecs() const;

    // List of plugins that depend on this - e.g. this plugins provides for them
    QList<PluginSpec *> providesForSpecs() const;

    void setEnabled(bool enabled = true);
    bool isEnabled() const;
    void setPersistent(bool persistent = true);
    bool isPersistent() const;
    bool isIndirectlyDisabled() const;

    IPlugin *loadPlugin();
    void unloadPlugin();
    bool initializePlugin();
    IPlugin *plugin() const;

    // State
    State state() const;
    bool hasError() const;
    QString errorString() const;

private:
    Q_DECLARE_PRIVATE(PluginSpec)
    PluginSpecPrivate *d_ptr;
};

} // namespace PluginLoader

#if !defined(QT_NO_DEBUG_STREAM)
PLUGINLOADER_EXPORT QDebug operator<<(QDebug dbg,
        const PluginLoader::PluginSpec *pluginSpecPtr);
#endif

Q_DECLARE_METATYPE(PluginLoader::PluginSpec *)

#endif // PLUGINLOADER_PLUGINSPEC_H
