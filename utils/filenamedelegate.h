#ifndef UTILS_FILENAMEDELEGATE_H
#define UTILS_FILENAMEDELEGATE_H

#include <QtGui/QStyledItemDelegate>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT FileNameDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FileNameDelegate(QObject *parent = 0);

public:
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

} // Utils

#endif // UTILS_FILENAMEDELEGATE_H
