#include "stylesheetloader.h"

#include <QtCore/QDir>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtGui/QApplication>

#include "filesystemwatcher.h"

using namespace Utils;

namespace {
    const char * const STYLESHEET_SUFFIX = "qss";
}

/*!
 * \class Utils::StyleSheetLoader
 * \brief Can find, load and reload automatically upon change a Qt Style Sheet
 */

StyleSheetLoader::StyleSheetLoader()
    : QObject(0),
    m_oldStyleSheet(qApp->styleSheet()),
    m_fileWatcher(new FileSystemWatcher(this))
{
    connect(m_fileWatcher, SIGNAL(pathChanged(QString)),
            this, SLOT(reload()));

    restoreSettings();
}

StyleSheetLoader::~StyleSheetLoader()
{
}

//! Implements the Singleton pattern
StyleSheetLoader *StyleSheetLoader::instance()
{
    static StyleSheetLoader loader;
    return &loader;
}

//! WORKAROUND: Use it to prevent focus rectangle to collide with button label
/*! This function is provided as central point for temporary fix related to
 * focus rectangle which covers some letters with vertical lines on left/right
 * side (B {Back, Browse}, N {Next}, l {Cancel}).  Qt has poor support for
 * style sheets, a lot of bugs reported are not fixed even alter few years.
 * Until that changes or we change font to another with "better" metrics the
 * button text is extended by whitespaces.
 */
QString StyleSheetLoader::fixButtonText(const QString &text)
{
    return QString(" %1 ").arg(text);
}

//! Query if a global style sheet is set at the moment
bool StyleSheetLoader::isStyleSheetSet() const
{
    return !qApp->styleSheet().isEmpty();
}

//! Select the default style sheet
void StyleSheetLoader::setDefaultName(const QString &name)
{
    m_defaultName = name;
}

//! Query the default style sheet
QString StyleSheetLoader::defaultName() const
{
    return m_defaultName;
}

//! Set paths to search for style sheets
void StyleSheetLoader::setPaths(const QStringList &paths)
{
    m_paths = QSet<QString>::fromList(paths);
    findFiles();
}

//! Query paths searched for style sheets
QStringList StyleSheetLoader::paths() const
{
    return m_paths.values();
}

//! Query names of available style sheets
QStringList StyleSheetLoader::names() const
{
    QStringList names = m_namePathMap.keys();
    names.sort();
    return names;
}

//! Query name of the active style sheet
QString StyleSheetLoader::activeName() const
{
    return m_activeName;
}

//! Load style sheet given by its name
void StyleSheetLoader::load(const QString &name)
{
    m_activeName = name;
    reload();
}

//! Unload active style sheet
void StyleSheetLoader::unload()
{
    stopWatcher();
    qApp->setStyleSheet(QString());
}

//! Reload active style sheet
void StyleSheetLoader::reload()
{
    findFiles();
    unload();

    if (m_activeName.isEmpty()) {
        qApp->setStyleSheet(m_oldStyleSheet);
    }
    else {
        qApp->setStyleSheet(QLatin1String("file:///")
                + m_namePathMap[m_activeName]);

        startWatcherOnActiveFile();
    }
}

void StyleSheetLoader::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("StyleSheetLoader");

    m_activeName =
        settings.value(QLatin1String("CurrentStyleSheet")).toString();

    settings.endGroup();
}

void StyleSheetLoader::saveSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("StyleSheetLoader"));

    settings.setValue(QLatin1String("CurrentStyleSheet"), m_activeName);

    settings.endGroup();
}

void StyleSheetLoader::findFiles()
{
    m_namePathMap.clear();
    QFileInfoList infoList;
    foreach (const QString &path, m_paths) {
        QDir dir(path);
        if (!dir.exists()) {
            continue;
        }

        // Search for files in registered paths
        infoList << dir.entryInfoList(
                QStringList() << QString("*.%1").arg(STYLESHEET_SUFFIX),
                QDir::Files);

        // Search for files in subfolders in registered paths - not recursively
        QFileInfoList dirInfoList = dir.entryInfoList(
                QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QFileInfo &dirInfo, dirInfoList) {
            QFileInfo info(QString("%1/%2.%3").arg(dirInfo.absoluteFilePath())
                    .arg(dirInfo.completeBaseName()).arg(STYLESHEET_SUFFIX));
            if (info.exists()) {
                infoList << info;
            }
        }
    }

    // Update map
    foreach (const QFileInfo &info, infoList) {
        m_namePathMap[info.completeBaseName()] = info.absoluteFilePath();
    }

    // Update internal variables if needed
    if (!m_namePathMap.keys().contains(m_defaultName)) {
        m_defaultName.clear();
    }
    if (!m_namePathMap.keys().contains(m_activeName)) {
        m_activeName = m_defaultName;
    }

    saveSettings();
}

void StyleSheetLoader::startWatcherOnActiveFile()
{
    Q_ASSERT(!m_activeName.isEmpty());
    Q_ASSERT(m_namePathMap.keys().contains(m_activeName));

    stopWatcher();
    if (!m_fileWatcher->paths().contains(m_namePathMap[m_activeName]))
        m_fileWatcher->addPath(m_namePathMap[m_activeName]);
}

void StyleSheetLoader::stopWatcher()
{
    if (m_fileWatcher->paths().contains(m_namePathMap[m_activeName]))
        m_fileWatcher->removePath(m_namePathMap[m_activeName]);
}
