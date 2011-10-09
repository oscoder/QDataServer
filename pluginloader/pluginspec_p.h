#ifndef PLUGINLOADER_PLUGINSPEC_P_H
#define PLUGINLOADER_PLUGINSPEC_P_H
/*! \cond __pimpl */

#include "pluginspec.h"

#include <QtCore/QXmlStreamReader>

namespace PluginLoader {

class PluginSpecPrivate
{
public:
    PluginSpecPrivate(PluginSpec *q);
    virtual ~PluginSpecPrivate();

    bool read(const QString &specFileName);
    bool provides(const QString &pluginName, const QString &version) const;
    bool resolveDependencies(const QList<PluginSpec *> &specs);
    void resolveIndirectlyDisabled(bool forceResolve);
    bool loadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
            &circularityCheckQueue);
    bool unloadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
            &circularityCheckQueue);
    IPlugin *loadPlugin();
    void unloadPlugin();
    bool initializePlugin();

    QString name;
    QString version;
    QString description;
    QString category;
    QList<PluginDependency> dependencies;
    bool enabled;
    bool persistent;
    bool indirectlyDisabled;
    bool initializationFailed;
    bool circularDependencyDetected;

    QString filePath;
    QString fileName;

    QList<PluginSpec *> providesSpecs;
    QList<PluginSpec *> dependencySpecs;
    IPlugin *plugin;

    PluginSpec::State state;
    bool hasError;
    QString errorString;

    static bool isValidVersion(const QString &version);
    static int versionCompare(const QString &version1, const QString &version2);

private:
    bool reportError(const QString &err);
    void readPluginSpec(QXmlStreamReader &reader);
    void readDependencies(QXmlStreamReader &reader);
    void readDependencyEntry(QXmlStreamReader &reader);

    static QRegExp &versionRegExp();

private:
    Q_DECLARE_PUBLIC(PluginSpec)
    PluginSpec *q_ptr;
};

} // namespace PluginLoader

/*! \endcond */
#endif // PLUGINLOADER_PLUGINSPEC_P_H
