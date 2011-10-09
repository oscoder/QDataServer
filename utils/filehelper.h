#ifndef UTILS_FILEHELPER_H
#define UTILS_FILEHELPER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRegExp>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT FileHelper
{
public:
    static QString buildFileName(const QString &path, const QString &baseName,
            const QString &extension);
    static QString ensureHasExtension(const QString &fileName, const QString
            &extension);
    static QString addSuffix(const QString &fileName, const QString &suffix);
    static QString createUniqueName(const QStringList &existingNames, const
            QString &nameTemplate, int indexFieldWidth = 2);
    static bool promptOverwrite(const QString &fileName, QString *errorMessage
            = 0);
    static bool createFile(const QString &fileName, const QString &content,
            QString *errorMessage = 0);
    static QString buildPluginName(const QString &path, const QString &name);
    static QRegExp fileNameValidation();
    static QRegExp locationValidation();
};

} // namespace Utils

#endif // UTILS_FILEHELPER_H
