#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QRegExp>
#include <QtCore/QStack>

#include <utils/filehelper.h>

#include "iplugin.h"

using namespace PluginLoader;

enum {
    debugPluginSpec = 0
};

PluginSpec::PluginSpec()
    : d_ptr(new PluginSpecPrivate(this))
{
}

PluginSpec::~PluginSpec()
{
    Q_D(PluginSpec);
    delete d;
}

/*!
    Parses the given file.
    If file is successfully parsed the plugin status is changed to
    PluginSpec::Read.
    \param fileName the file to be read
    \return true if file was successfully parsed
    \sa PluginSpec::State
 */
bool PluginSpec::read(const QString &fileName)
{
    Q_D(PluginSpec);
    return d->read(fileName);
}

/*!
    Tries to resolve dependences of given plugins.
    If dependences of given plugins were successfully solved the plugin status
    is changed to PluginSpec::Resolved.
    \param specs the list of plugin specifications
    \return true if all dependences were successfully solved
    \sa PluginSpec::State
 */
bool PluginSpec::resolveDependecies(const QList<PluginSpec *> &specs)
{
    Q_D(PluginSpec);
    return d->resolveDependencies(specs);
}

/*!
    Updates the plugin flag "indirectly disabled".
    \param forceResolve true causes re-resolving the dependences
 */
void PluginSpec::resolveIndirectlyDisabled(bool forceResolve)
{
    Q_D(PluginSpec);
    d->resolveIndirectlyDisabled(forceResolve);
}

/*!
    Creates loading queue in the given \a queue , checks for circular dependencies.
    \param queue in/out argument
    \param circularityCheckQueue in/out argument
    \return true if all dependences were successfully solved
 */
bool PluginSpec::loadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
        &circularityCheckQueue)
{
    Q_D(PluginSpec);
    return d->loadQueue(queue, circularityCheckQueue);
}

/*!
    Creates unloading queue in the given \a queue , checks for circular dependencies.
    \param queue in/out argument
    \param circularityCheckQueue in/out argument
    \return true if all dependences were successfully solved
 */
bool PluginSpec::unloadQueue(QList<PluginSpec *> &queue, QList<PluginSpec *>
        &circularityCheckQueue)
{
    Q_D(PluginSpec);
    return d->unloadQueue(queue, circularityCheckQueue);
}

/*!
    The plugin name.
    This is valid after the PluginSpec::Read state is reached.
    \sa PluginSpec::State
 */
QString PluginSpec::name() const
{
    Q_D(const PluginSpec);
    return d->name;
}

/*!
    The plugin version.
    This is valid after the PluginSpec::Read state is reached.
    \sa PluginSpec::State
 */
QString PluginSpec::version() const
{
    Q_D(const PluginSpec);
    return d->version;
}

/*!
    The plugin description.
    This is valid after the PluginSpec::Read state is reached.
    \sa PluginSpec::State
 */
QString PluginSpec::description() const
{
    Q_D(const PluginSpec);
    return d->description;
}

/*!
    The category that the plugin belongs to.
    Categories are groups of plugins which allow for keeping them together in
    the UI. Returns an empty string if the plugin does not belong to a category.
    This is valid after the PluginSpec::Read state is reached.
    \sa PluginSpec::State
 */
QString PluginSpec::category() const
{
    Q_D(const PluginSpec);
    return d->category;
}

/*!
    Enables or disables plugin loading at startup.
    \param enabled true (the default value) to enable the plugin
    \sa PluginSpec::isEnabled()
 */
void PluginSpec::setEnabled(bool enabled)
{
    Q_D(PluginSpec);
    if (d->persistent && !enabled) {
        return;
    }
    d->enabled = enabled;
}

/*!
    Returns whether to load the plugin at startup or not.
    True by default - the user can change it from the Plugin settings.
    \sa PluginSpec::setEnabled(bool)
    \return true if plugin is enabled
 */
bool PluginSpec::isEnabled() const
{
    Q_D(const PluginSpec);
    return d->enabled || d->persistent;
}

/*!
    Sets or resets plugin loading at startup.
    \param persistent true (the default value) to enable the plugin
    \sa PluginSpec::isPersistent()
 */
void PluginSpec::setPersistent(bool persistent)
{
    Q_D(PluginSpec);
    d->persistent = persistent;
    if (persistent) {
        d->enabled = persistent;
    }
}

/*!
    Returns whether the plugin can be disabled or not.
    False by default - the user cannot change it from the UI nor xml description
    file.
    \return true if plugin is persistent
 */
bool PluginSpec::isPersistent() const
{
    Q_D(const PluginSpec);
    return d->persistent;
}

/*!
    Returns true if loading was not done due to user unselecting this plugin or
    its dependencies.
    \return true if plugin was not loaded
 */
bool PluginSpec::isIndirectlyDisabled() const
{
    Q_D(const PluginSpec);
    return d->indirectlyDisabled;
}

/*!
    The list of plugins this plugin depends on.
    This is valid after the PluginSpec::Read state is reached.
    \return the plugin dependencies
    \sa PluginSpec::State
 */
QList<PluginDependency> PluginSpec::dependencies() const
{
    Q_D(const PluginSpec);
    return d->dependencies;
}

/*!
    The absolute path to the plugin xml description file (without file name)
    this PluginSpec corresponds to.
    \return the absolute path to the file
    \sa PluginSpec::fileName()
 */
QString PluginSpec::filePath() const
{
    Q_D(const PluginSpec);
    return d->filePath;
}

/*!
    The file name of the plugin xml description file (without a path)
    \return the file name
    \sa PluginSpec::filePath()
 */
QString PluginSpec::fileName() const
{
    Q_D(const PluginSpec);
    return d->fileName;
}

/*!
    Returns the list of dependencies, already resolved to existing plugin specs.
    Valid if PluginSpec::Resolved state is reached.
    \sa PluginSpec::dependencies()
    \sa PluginSpec::State
 */
QList<PluginSpec *> PluginSpec::dependencySpecs() const
{
    Q_D(const PluginSpec);
    return d->dependencySpecs;
}

/*!
    Returns the list of plugins that depend on this one.
    \sa PluginSpec::dependencySpecs()
 */
QList<PluginSpec *> PluginSpec::providesForSpecs() const
{
    Q_D(const PluginSpec);
    return d->providesSpecs;
}

/*!
    Loads plugin library and creates plugin instance.
    \sa PluginSpec::plugin()
 */
IPlugin *PluginSpec::loadPlugin()
{
    Q_D(PluginSpec);
    return d->loadPlugin();
}

/*!
    Unloads plugin.
    \sa PluginSpec::loadPlugin()
 */
void PluginSpec::unloadPlugin()
{
    Q_D(PluginSpec);
    d->unloadPlugin();
}

/*!
    Initialize plugin.
    \return Returns true if initialized successfully.
 */
bool PluginSpec::initializePlugin()
{
    Q_D(PluginSpec);
    return d->initializePlugin();
}

/*!
    The corresponding IPlugin instance, if the plugin library has already been
    successfully loaded, i.e. the PluginSpec::Loaded state is reached.
    \sa PluginSpec::loadPlugin()
    \sa PluginSpec::State
 */
IPlugin *PluginSpec::plugin() const
{
    Q_D(const PluginSpec);
    return d->plugin;
}

/*!
    The state in which the plugin currently is.
    \sa PluginSpec::State
 */
PluginSpec::State PluginSpec::state() const
{
    Q_D(const PluginSpec);
    return d->state;
}

/*!
    Returns whether an error occurred while reading/starting the plugin or not.
    \return true if some error occurred
    \sa PluginSpec::errorString()
 */
bool PluginSpec::hasError() const
{
    Q_D(const PluginSpec);
    return d->hasError;
}

/*!
    Detailed, possibly multi-line, user-readable error description in case of an
    error.
    \return error string
    \sa PluginSpec::hasError()
 */
QString PluginSpec::errorString() const
{
    Q_D(const PluginSpec);
    return d->errorString;
}

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug dbg, const PluginLoader::PluginSpec *pluginSpecPtr)
{
    dbg.nospace() << "PluginSpec(name: " << pluginSpecPtr->name()
            << ", plugin: " << pluginSpecPtr->plugin() << ')';
    return dbg.space();
}
#endif

namespace {
    const char * const PLUGIN = "plugin";
    const char * const PLUGIN_NAME = "name";
    const char * const PLUGIN_VERSION = "version";
    const char * const DESCRIPTION = "description";
    const char * const CATEGORY = "category";
    const char * const DEPENDENCYLIST = "dependencyList";
    const char * const DEPENDENCY = "dependency";
    const char * const DEPENDENCY_NAME = "name";
    const char * const DEPENDENCY_VERSION = "version";
}

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *q)
    : enabled(false),
    persistent(false),
    indirectlyDisabled(false),
    initializationFailed(false),
    circularDependencyDetected(false),
    plugin(0),
    state(PluginSpec::Invalid),
    hasError(false),
    q_ptr(q)
{
}

PluginSpecPrivate::~PluginSpecPrivate()
{
}

bool PluginSpecPrivate::read(const QString &specFileName)
{
    name.clear();
    version.clear();
    description.clear();
    category.clear();
    errorString.clear();
    dependencies.clear();
    enabled = false;
    indirectlyDisabled = false;
    circularDependencyDetected = false;
    providesSpecs.clear();
    dependencySpecs.clear();
    plugin = 0;
    state = PluginSpec::Invalid;
    hasError = false;

    QFile file(specFileName);
    if (!file.exists()) {
        return reportError(
                PluginSpec::tr("File does not exist: %1").arg(specFileName));
    }
    if (!file.open(QIODevice::ReadOnly)) {
        return reportError(PluginSpec::tr(
                    "File could not be opened for read: %1").arg(specFileName));
    }

    QFileInfo fileInfo(file);
    filePath = fileInfo.absolutePath();
    fileName = fileInfo.fileName();

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            readPluginSpec(reader);
            break;
        default:
            break;
        }
    }
    if (reader.hasError()) {
        return reportError(
                PluginSpec::tr("Error parsing spec file %1: %2, at line %3")
                    .arg(fileName).arg(filePath).arg(reader.errorString())
                    .arg(reader.lineNumber()));
        return false;
    }

    state = PluginSpec::Read;
    enabled = true;
    return true;
}

bool PluginSpecPrivate::resolveDependencies(const QList<PluginSpec *> &specs)
{
    Q_Q(PluginSpec);
    if (hasError) {
        return false;
    }

    if (state == PluginSpec::Resolved) {
        // Go back, so we just re-resolve the dependencies
        state = PluginSpec::Read;
    }

    Q_ASSERT(state == PluginSpec::Read);

    QList<PluginSpec *> resolvedDependencies;
    foreach (const PluginDependency &dependency, dependencies) {
        PluginSpec *found = 0;

        foreach (PluginSpec *spec, specs) {
            if (spec->name() == dependency.name) {
                found = spec;
                spec->d_ptr->providesSpecs.append(q);
                break;
            }
        }
        if (!found) {
            reportError(PluginSpec::tr(
                        "Plugin %1 - could not resolve dependency on %2.")
                    .arg(name).arg(dependency.name));
            continue;
        }
        else {
            resolvedDependencies.append(found);
        }
    }
    if (hasError) {
        return false;
    }

    dependencySpecs = resolvedDependencies;

    state = PluginSpec::Resolved;

    return true;
}

void PluginSpecPrivate::resolveIndirectlyDisabled(bool forceResolve)
{
    static QStack<PluginSpecPrivate *> resolvedPluginsStack;

    if (circularDependencyDetected)
        return;

    if (resolvedPluginsStack.contains(this)) {
        // circular dependency found!!!
        indirectlyDisabled = true;
        circularDependencyDetected = true;

        QString pluginOrder = name;
        QStack<PluginSpecPrivate *>::const_iterator i =
                resolvedPluginsStack.constEnd();
        while (i != resolvedPluginsStack.constBegin()) {
            --i;
            PluginSpecPrivate *pluginSpec = *i;
            pluginOrder.append(QString(" -> ")).append(pluginSpec->name);
            if (pluginSpec == this)
                break;
        }

        resolvedPluginsStack.push(this);

        // Resolve again plugin which depends on me and have circular dependency
        //resolvedPluginsStack.top()->resolveIndirectlyDisabled(true);
        foreach (PluginSpec *providesSpec, providesSpecs) {
            providesSpec->resolveIndirectlyDisabled(true);
        }
        reportError(PluginSpec::tr("Circular dependency detected: %1")
                .arg(pluginOrder));

        PluginSpecPrivate *me = resolvedPluginsStack.pop();
        Q_ASSERT(me == this);
        return;
    }

    if (forceResolve) {
        indirectlyDisabled = false;
    }
    else if (indirectlyDisabled) {
        return;
    }

    resolvedPluginsStack.push(this);

    foreach (PluginSpec *dependencySpec, dependencySpecs) {
        if (dependencySpec->hasError()
                || dependencySpec->isIndirectlyDisabled()
                || !dependencySpec->isEnabled()
                || dependencySpec->d_func()->initializationFailed) {
            indirectlyDisabled = true;
            break;
        }
    }
    if (indirectlyDisabled || forceResolve) {
        // Resolve again plugins which depends on me
        foreach (PluginSpec *providesSpec, providesSpecs) {
            providesSpec->resolveIndirectlyDisabled(forceResolve);
        }
    }

    PluginSpecPrivate *me = resolvedPluginsStack.pop();
    Q_ASSERT(me == this);
}

bool PluginSpecPrivate::loadQueue(QList<PluginSpec *> &queue,
        QList<PluginSpec *> &circularityCheckQueue)
{
    Q_Q(PluginSpec);
    Q_ASSERT(state >= PluginSpec::Resolved);

    if (!enabled || indirectlyDisabled) {
        return false;
    }

    if (queue.contains(q)) {
        return true;
    }

    if (circularityCheckQueue.contains(q)) {
        QString pluginOrder = circularityCheckQueue.at(0)->name();
        for (int i = 1; i < circularityCheckQueue.size(); ++i) {
            pluginOrder.append(QString(" -> "))
                .append(circularityCheckQueue.at(i)->name());
        }
        pluginOrder.append(QString(" -> ")).append(name);

        reportError(PluginSpec::tr("Circular dependency detected: %1")
                .arg(pluginOrder));
        return false;
    }
    circularityCheckQueue.append(q);

    foreach (PluginSpec *pluginSpec, dependencySpecs) {
        if (!pluginSpec->loadQueue(queue, circularityCheckQueue)) {
            reportError(PluginSpec::tr(
                    "Plugin %1 cannot be loaded because dependency %2 failed.")
                    .arg(name).arg(pluginSpec->name()));
            return false;
        }
    }

    queue.append(q);

    return true;
}

bool PluginSpecPrivate::unloadQueue(QList<PluginSpec *> &queue,
        QList<PluginSpec *> &circularityCheckQueue)
{
    Q_Q(PluginSpec);
    Q_ASSERT(state >= PluginSpec::Resolved);

    if ((!enabled || indirectlyDisabled) && (state < PluginSpec::Loaded)) {
        return false;
    }

    if (queue.contains(q)) {
        return true;
    }

    if (circularityCheckQueue.contains(q)) {
        QString pluginOrder = circularityCheckQueue.at(0)->name();
        for (int i = 1; i < circularityCheckQueue.size(); ++i) {
            pluginOrder.append(QString(" -> "))
                .append(circularityCheckQueue.at(i)->name());
        }
        pluginOrder.append(QString(" -> ")).append(name);

        reportError(PluginSpec::tr("Circular dependency detected: %1")
                .arg(pluginOrder));
        return false;
    }
    circularityCheckQueue.append(q);

    foreach (PluginSpec *pluginSpec, providesSpecs) {
        pluginSpec->unloadQueue(queue, circularityCheckQueue);
    }

    queue.append(q);

    return true;
}

IPlugin *PluginSpecPrivate::loadPlugin()
{
    Q_ASSERT(state == PluginSpec::Resolved);

    const QString libName = Utils::FileHelper::buildPluginName(filePath, name);
    Q_ASSERT(QLibrary::isLibrary(libName));
    Q_ASSERT(QFile::exists(libName));

    foreach (PluginSpec *dependencySpec, dependencySpecs) {
        if (dependencySpec->plugin() == 0) {
            // plugin I depend on should be loaded before me and isn't
            return 0;
        }
    }

    QPluginLoader pluginLoader(libName);
    QObject *object = pluginLoader.instance();
    if (object != 0) {
        plugin = qobject_cast<IPlugin *>(object);
        if (plugin != 0) {
            state = PluginSpec::Loaded;
            if (debugPluginSpec) {
                qDebug("Plugin loaded: %s", qPrintable(libName));
            }
        }
        else {
            pluginLoader.unload();

            qWarning("The file \'%s\' is not compatible plugin.", qPrintable(libName));
            reportError(PluginSpec::tr(
                    "The file \'%1\' is not compatible plugin.")
                    .arg(libName));
        }
    }
    else {
        qWarning("%s", qPrintable(pluginLoader.errorString()));
        reportError(pluginLoader.errorString());
    }
    return plugin;
}

void PluginSpecPrivate::unloadPlugin()
{
    if (plugin == 0)
        return;

    if (state >= PluginSpec::Initialized)
        plugin->shutdown();

    const QString libName = Utils::FileHelper::buildPluginName(filePath, name);
    QPluginLoader pluginLoader(libName);

    // to be able to unload plugin QPluginLoader has to be initialized and connected with plugin,
    // so load() or instance() has to be called,
    // but unload is successful only if no other QPluginLoader helds instance of that plugin.
    pluginLoader.load();
    bool unloaded = pluginLoader.unload();
    if (unloaded) {
        if (debugPluginSpec) {
            qDebug("Plugin unloaded: %s", qPrintable(name));
        }
    }
    else {
        qWarning("Plugin %s could not be unloaded: %s",
                 qPrintable(name),
                 qPrintable(pluginLoader.errorString()));
    }
    plugin = 0;

    state = PluginSpec::Resolved;
}

bool PluginSpecPrivate::initializePlugin()
{
    Q_ASSERT(plugin != 0);
    Q_ASSERT(state == PluginSpec::Loaded);

    QString errorString;
    if (!plugin->initialize(&errorString)) {
        qWarning("Initialization of \'%s\' plugin failed: %s",
                qPrintable(name), qPrintable(errorString));
        reportError(PluginSpec::tr(
                "Initialization of \'%1\' plugin failed: ")
                .arg(name));
        initializationFailed = true;
        return false;
    }

    if (debugPluginSpec) {
        qDebug("Plugin initialized: %s, version=%s, category=%s, "
                "description=%s",
                qPrintable(name),
                qPrintable(version),
                qPrintable(category),
                qPrintable(description));
    }

    initializationFailed = false;
    state = PluginSpec::Initialized;
    return true;
}

bool PluginSpecPrivate::isValidVersion(const QString &version)
{
    return versionRegExp().exactMatch(version);
}

int PluginSpecPrivate::versionCompare(const QString &version1, const QString
        &version2)
{
    QRegExp reg1 = versionRegExp();
    QRegExp reg2 = versionRegExp();
    if (!reg1.exactMatch(version1)) {
        return 0;
    }
    if (!reg2.exactMatch(version2)) {
        return 0;
    }
    int number1;
    int number2;
    for (int i = 0; i < 4; ++i) {
        number1 = reg1.cap(i + 1).toInt();
        number2 = reg2.cap(i + 1).toInt();
        if (number1 < number2) {
            return -1;
        }
        if (number1 > number2) {
            return 1;
        }
    }
    return 0;
}

bool PluginSpecPrivate::reportError(const QString &err)
{
    if (!errorString.isEmpty()) {
        errorString.append(QLatin1Char('\n'));
    }

    errorString.append(err);
    hasError = true;

    return false;
}

void PluginSpecPrivate::readPluginSpec(QXmlStreamReader &reader)
{
    QString element = reader.name().toString();
    if (element != QLatin1String(PLUGIN)) {
        reportError(PluginSpec::tr(
                    "Expected element '%1' as top level element").arg(PLUGIN));
        return;
    }
    name = reader.attributes().value(PLUGIN_NAME).toString();
    if (name.isEmpty()) {
        reportError(PluginSpec::tr("Expected attribut '%1' at element %2")
                    .arg(PLUGIN_NAME).arg(PLUGIN));
        return;
    }
    version = reader.attributes().value(PLUGIN_VERSION).toString();
    if (!isValidVersion(version)) {
        version.clear();
    }
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            element = reader.name().toString();
            if (element == DESCRIPTION)
                description = reader.readElementText().trimmed();
            else if (element == CATEGORY)
                category = reader.readElementText().trimmed();
            else if (element == DEPENDENCYLIST)
                readDependencies(reader);
            break;
        case QXmlStreamReader::EndDocument:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::EndElement:
        case QXmlStreamReader::Characters:
        default:
            break;
        }
    }
}

void PluginSpecPrivate::readDependencies(QXmlStreamReader &reader)
{
    QString element;
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            element = reader.name().toString();
            if (element == DEPENDENCY) {
                readDependencyEntry(reader);
            }
            break;
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::EndElement:
        default:
            break;
        }
    }
}

void PluginSpecPrivate::readDependencyEntry(QXmlStreamReader &reader)
{
    PluginDependency dep;
    dep.name = reader.attributes().value(DEPENDENCY_NAME).toString();
    if (dep.name.isEmpty()) {
        reportError(PluginSpec::tr("Expected attribut '%1' at element %2")
                      .arg(DEPENDENCY_NAME).arg(DEPENDENCY));
        return;
    }
    dep.version = reader.attributes().value(DEPENDENCY_VERSION).toString();
    if (!isValidVersion(dep.version)) {
        dep.version.clear();
    }

    dependencies.append(dep);
    reader.readNext();
}

QRegExp &PluginSpecPrivate::versionRegExp()
{
    static QRegExp reg("([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?");
    return reg;
}
