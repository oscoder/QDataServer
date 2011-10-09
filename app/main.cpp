#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QWindowsStyle>
#include <QtGui/QWidget>
#include <QtGui/QBitmap>
#include <QtGui/QPixmap>
#include <QtGui/QMessageBox>

#include <utils/stylesheetloader.h>
#include <utils/splashscreen.h>

#include <pluginloader/iplugin.h>
#include <pluginloader/pluginmanager.h>
#include <pluginloader/pluginspec.h>

#include "qtsingleapplication/qtsingleapplication.h"

bool checkRunningApplication()
{
    QtSingleApplication *const app =
        qobject_cast<QtSingleApplication *const>(qApp);
    Q_ASSERT(app != 0);
    if (app->isRunning()) {
        // Every message is accepted to raise main window
        app->sendMessage(":-)");
        return true;
    }
    return false;
}

QString readArgumentValue(const QStringList &arguments, const QString &argument)
{
    const int ArgIndex = arguments.indexOf(argument, 1);
    if (ArgIndex > -1) {
        if (ArgIndex + 1 < arguments.count())
            return arguments[ArgIndex + 1];
        else {
            qWarning("%s: Value is missing for argument '%s'",
                    Q_FUNC_INFO, qPrintable(argument));
            return "";
        }
    }
    return "";
}
/*! \endcond */

int main(int argc, char *argv[])
{
    QStringList arguments;
    for (int n = 0; n < argc; ++n) {
        arguments << argv[n];
    }
    const QString dataLocation =
        QDesktopServices::storageLocation(QDesktopServices::DataLocation);

//    QString appId;
//    if (brand->singleInstance() == Brand::StrictSingleInstance) {
//        appId = dataLocation + brand->applicationName();
//    }
//    else {
        QtSingleApplication app(argc, argv);
        appId = app.id();
//    }

    QScopedPointer<QtSingleApplication> app(
            new QtSingleApplication(appId, argc, argv));
    if (brand->singleInstance() != Brand::MultipleInstances)
        if (checkRunningApplication()) {
            QString appName = brand->applicationName();
            QMessageBox::information(0, "Oh Noes!",
                       QString("It seems that " + appName + " is already running. ") +
                       QString("If this is not true, please try again after 10 seconds. \r\n\r\n") +
                       QString("It is not possible to open " + appName + " multiple times."),
                                     QMessageBox::Ok);

            qCritical("Application is already running.");
            return -1;
        }

    // Set default style to unify application look & feel on all platforms
    // NOTE: This is useful only for widgets that are not handled in style sheet
    // In ideal case everything is declared in CSS and following line is surplus
    if (!arguments.contains("-style"))
        qApp->setStyle(new QWindowsStyle);

    // Check that is splash given in parameter
    QString splashPath = readArgumentValue(arguments, "-splash");
    if(splashPath.isEmpty()) {
        // Use default if not given as parameter
        splashPath = brand->applicationSplashName();
    }

    Utils::SplashScreen *splash = new Utils::SplashScreen(QPixmap(splashPath), QPoint(302, 387));

    // if no mask, then don't install it
    if(!arguments.contains("-nomask")) {
        // Check that is splash mask given in parameter (used for helping the mask fitting)
        QString splashMaskPath = readArgumentValue(arguments, "-splashmask");
        if(splashMaskPath.isEmpty()) {
            // Use default if not given as parameter
            splashMaskPath = brand->applicationSplashMaskName();
        }

        // Mask can be left empty, then no mask is installed
        if(!splashMaskPath.isEmpty()) {
            QPixmap mask(splashMaskPath);
            splash->setMask(QBitmap(mask));
        }
    }

    splash->setStatus("Theme");
    splash->show();

    /*! To access data stored by application you should use default QSettings
        contructor as it's shown in following example:
        \code
        QSettings settings;
        QString text = settings.value("Text", "default text").toString();
        \endcode
     */
    QCoreApplication::setApplicationName(brand->applicationName());
    QCoreApplication::setOrganizationName(brand->applicationVendor());
    QCoreApplication::setApplicationVersion(brand->applicationVersion());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // Create data location if not exists yet
    const QDir dataLocationDir(dataLocation);
    if (!dataLocationDir.exists()) {
        dataLocationDir.mkpath(dataLocation);
        dataLocationDir.refresh();
    }
    Q_ASSERT(dataLocationDir.exists());

    // Prevent loading of Qt plugins from the qt build dir on mac
    // Qt plugins are placed next to the folder with plugins for this application
#ifdef Q_OS_MAC
    QStringList libraryPaths = QCoreApplication::libraryPaths();
    libraryPaths.prepend(QApplication::applicationDirPath()
            + "/../" + QString(UITOOLS_REL_PLUGINS_DIR) + "/..");
    QCoreApplication::setLibraryPaths(libraryPaths);
#endif

    // Set theme search path
    const QString themePath = QCoreApplication::applicationDirPath()
        + "/../" + QString(UITOOLS_REL_THEMES_DIR);
    QStringList searchPaths = QIcon::themeSearchPaths();
    searchPaths.removeAll(themePath);
    searchPaths.removeAll(brand->themeSearchPath());
    searchPaths.insert(0, themePath);
    searchPaths.insert(1, brand->themeSearchPath());
    QIcon::setThemeSearchPaths(searchPaths);

    // Try to get the theme name from command line argument first
    QString themeName = readArgumentValue(arguments, "-theme");
    // Default theme name is product branding
    if (themeName.isEmpty())
        themeName = brand->themeName();

    QIcon::setThemeName(themeName);
    if (!QIcon::hasThemeIcon(QLatin1String("icon_placeholder"))) {
        qWarning("%s: Theme '%s' not found. You have to install a freedesktop "
                "compatible icon set named '%s' into '%s' or any folder "
                "returned by QIcon::themeSearchPaths().",
                Q_FUNC_INFO, qPrintable(themeName), qPrintable(themeName),
                qPrintable(QDir::toNativeSeparators(QDir::cleanPath(themePath))));
        // Use internal theme if everything else fails
        themeName = "base";
        QIcon::setThemeName(themeName);
        qWarning("%s: Used internal fallback theme '%s'", Q_FUNC_INFO,
                qPrintable(themeName));
    }

    qApp->setWindowIcon(QIcon::fromTheme(brand->applicationIconName()));

    PluginLoader::PluginManager *pm = PluginLoader::PluginManager::instance();
    QStringList pluginPaths = PluginLoader::PluginManager::getPluginPaths();
    pm->loadPlugins(pluginPaths);

    bool coreFound = false;
    foreach (PluginLoader::PluginSpec *pluginSpec, pm->pluginSpecs()) {
        if (pluginSpec->name() == QLatin1String("Core")) {
            coreFound = true;
            // It's not possible to disable core plugin
            pluginSpec->setPersistent();
        }
    }
    if (!coreFound) {
        qCritical("Core plugin has not been found");
        return -3;
    }

    // StyleSheetLoader has to be initialized before initializing plugins
    Utils::StyleSheetLoader *const loader =
        Utils::StyleSheetLoader::instance();
    loader->setDefaultName(brand->styleSheetName());
    const QString styleSheetsPath = qApp->applicationDirPath()
        + "/../" + QString(UITOOLS_REL_STYLESHEETS_DIR);
    loader->setPaths(QStringList(styleSheetsPath));

    // Style sheet could be specified by command line option '-stylesheet'
    if (!loader->isStyleSheetSet()) {
        // Load last active style sheet (saved in settings) or default if set
        loader->reload();
    }

    splash->setStatus("Plugins");
    if (!pm->initializePlugins(splash)) {
        QString pluginWhichRequestedShutdown;
        if (pm->isShutdownRequested(&pluginWhichRequestedShutdown)) {
            qCritical("Plugin '%s' requested shutdown of application",
                    qPrintable(pluginWhichRequestedShutdown));
            loader->unload();
            pm->unloadPlugins();
            return -2;
        }
    }

    splash->setStatus("Ready");
    if (brand->singleInstance() != Brand::MultipleInstances) {
        //! \todo Replace hardcoded string with proper constant
        Cci::Control::RibbonMainWindow *const ribbonMainWindow =
            Cci::Control::AbstractControl::DB::ribbonMainWindow("core.mainWindow");
        Q_ASSERT(ribbonMainWindow);

        QWidget *topLevelWidget = ribbonMainWindow->widget();
        while (topLevelWidget->parentWidget()) {
            topLevelWidget = topLevelWidget->parentWidget();
        }
        Q_ASSERT(topLevelWidget);

        app->setActivationWindow(topLevelWidget);
        topLevelWidget->show();
    }

    splash->close();
    delete splash;

    const int result = app->exec();

    pm->unloadPlugins();

    return result;
}

