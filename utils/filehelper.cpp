#include "filehelper.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringBuilder>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

using namespace Utils;

/*!
    \class Utils::FileHelper
    \brief Helps with common operations on files and file names
 */

/*!
    \brief Constructs the file name with absolute \a path
    \deprecated Used by wizards only, but current UI does not use wizards

    The given \a extension is added to the file name in case the \a baseName
    doesn't contain one.
    \return the file name with absolute path and extension
 */
QString FileHelper::buildFileName(const QString &path, const QString &baseName,
        const QString &extension)
{
    const QString fullName = QDir::cleanPath(ensureHasExtension(
                QDir(path).absoluteFilePath(baseName), extension));

    return fullName;
}

//! Adds preferred extension if missing
/*!
 * - If the \a fileName already has \a extension, no change is performed.
 * - If the extension of \a fileName matches case insensitively, it is replaced
 * - Otherwise \a extension is appended (with the separating dot)
 */
QString FileHelper::ensureHasExtension(const QString &fileName, const QString
        &extension)
{
    const QFileInfo fileInfo(fileName);

    if (fileInfo.suffix() == extension)
        return fileName;

    if (fileInfo.suffix().compare(extension, Qt::CaseInsensitive) == 0)
        return fileInfo.completeBaseName() % "." % extension;

    return fileName % "." % extension;
}

//! Adds suffix to the file name, before the file name extension
/*!
 * \code
 * FileHelper::addSuffix("foo.zip", "_bar") => "foo_bar.zip"
 * \endcode
 */
QString FileHelper::addSuffix(const QString &fileName, const QString &suffix)
{
    const QFileInfo fileInfo(fileName);

    return fileInfo.completeBaseName() % suffix % "." % fileInfo.suffix();
}

//! Builds file name from template, unique among given existing names
/*!
 * \param existingNames Names to consider for selection of unique value for the
 * index field.
 * \param nameTemplate Template for the file name. MUST contain exactly one
 * occurrence of "%1" place holder for the index field.
 * \param indexFieldWidth How much to pad the index field with '0' (zero)
 * characters.
 *
 * Example
 *
 * \code
 * const QDir dir = ...;
 * const QString newFileName = FileHelper::createUniqueName(dir.entryList(),
 *         "New File %1.txt");
 * \endcode
 *
 */
QString FileHelper::createUniqueName(const QStringList &existingNames, const
        QString &nameTemplate, int indexFieldWidth)
{
    Q_ASSERT(nameTemplate.contains("%1"));

    const QRegExp rx(QRegExp::escape(nameTemplate).arg("(\\d+)"),
            Qt::CaseInsensitive);

    int biggestExistingIndex = 0;

    foreach (const QString &existingName, existingNames) {
        if (rx.exactMatch(existingName))
            biggestExistingIndex = qMax(biggestExistingIndex,
                    rx.cap(1).toInt(0, 10));
    }

    return nameTemplate.arg(biggestExistingIndex + 1, indexFieldWidth, 10,
            QChar('0'));
}

/*!
    \brief Displays dialog to user asking whether to overwrite a file or not

    This method checks for common mistakes when creating file on file system.
    \return true if file doesn't exist or user permits overwriting
 */
bool FileHelper::promptOverwrite(const QString &fileName, QString *errorMessage)
{
    const QFileInfo fileInfo(fileName);

    if (!fileInfo.exists())
        return true;

    if (fileInfo.isDir()) {
        if (errorMessage != 0)
            *errorMessage = QObject::tr("'%1' is a directory not a file.")
                .arg(fileName);
        return false;
    }
    if (!fileInfo.isWritable()) {
        if (errorMessage != 0)
            *errorMessage = QObject::tr("'%1' is read-only file.")
                .arg(fileName);
        return false;
    }

    const QString message =
        QObject::tr("The following file already exists:\n %1\n"
                "Would you like to overwrite it?")
            .arg(QDir::toNativeSeparators(QDir::cleanPath(fileName)));
    const QMessageBox::StandardButton chosenButton =
        QMessageBox::question(0, qApp->applicationName(), message,
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return chosenButton == QMessageBox::Yes;
}

/*!
    \brief Creates the file on file system

    The given text file \a fileName is created on file system. If directory
    doesn't exist yet it's created. In case the file already exists, user is
    asked whether to overwrite or not.
    File is filled with the \a content.
    If some error occurs the error message is available via \a errorMessage (if
    not null).
    \return true for successfully created file
 */
bool FileHelper::createFile(const QString &fileName, const QString &content,
        QString *errorMessage)
{
    const QFileInfo fileInfo(fileName);

    // Create the directory if not created yet
    const QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            if (errorMessage != 0)
                *errorMessage =
                    QObject::tr("Unable to create the directory '%1'.")
                        .arg(QDir::toNativeSeparators(dir.absolutePath()));
            return false;
        }
    }

    const bool overwriteAllowed = promptOverwrite(fileName, errorMessage);
    if (!overwriteAllowed)
        return true; // User discarded creation means method was successful

    QFile file(fileName);
    QIODevice::OpenMode flags = QIODevice::WriteOnly | QIODevice::Truncate;
    //! \todo Extend functionality in case we need to deal with binary file
    flags |= QIODevice::Text;

    // Create or re-open and clear the file
    if (!file.open(flags)) {
        if (errorMessage != 0)
            *errorMessage =
                QObject::tr("Unable to open '%1' for writing: %2")
                    .arg(fileName, file.errorString());
        return false;
    }

    // Add file's default content
    if (!content.isEmpty()) {
        QTextStream fileStream(&file);
        fileStream << content;
    }

    return true;
}

//! Portable way to build plugin/library file name
QString FileHelper::buildPluginName(const QString &path, const QString &name)
{
#ifdef QT_NO_DEBUG

#ifdef Q_OS_WIN
    const QString libFormat("%1/%2.dll");
#elif defined(Q_OS_MAC)
    const QString libFormat("%1/lib%2.dylib");
#else
    const QString libFormat("%1/lib%2.so");
#endif

#else // Q_NO_DEBUG

#ifdef Q_OS_WIN
    const QString libFormat("%1/%2d.dll");
#elif defined(Q_OS_MAC)
    const QString libFormat("%1/lib%2_debug.dylib");
#else
    const QString libFormat("%1/lib%2.so");
#endif

#endif // Q_NO_DEBUG

    return libFormat.arg(path.isEmpty() ? QChar('.') : path).arg(name);
}

//! Regexp to validate file name
QRegExp FileHelper::fileNameValidation()
{
    return QRegExp("((^[^\\|<>?:*/\\\\\"\\.\\s]+[^\\|<>?:*/\\\\\"]*"
            "[^\\|<>?:*/\\\\\"\\.\\s]{1}$)|(^[^\\|<>?:*/\\\\\"\\.\\s]+$))");
}

//! Regexp to validate file path
QRegExp FileHelper::locationValidation()
{
#ifdef Q_OS_WIN
    return QRegExp("^([a-zA-Z]:[\\\\])?[^?:*|\"/]*$");
#else
    return QRegExp("^[^?:*|\"\\\\]*$");
#endif
}
