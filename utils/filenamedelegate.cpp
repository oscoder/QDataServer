#include "filenamedelegate.h"

#include <QtGui/QLineEdit>

#include "filehelper.h"

using namespace Utils;

/*!
 * \class Utils::FileNameDelegate
 * \brief File name validating item delegate
 * \sa FileHelper::fileNameValidation()
 */

//! Constructor
FileNameDelegate::FileNameDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

/*!
  \reimp
*/
void FileNameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    Q_ASSERT(index.isValid());
    Q_ASSERT(lineEdit);

    lineEdit->setValidator(new QRegExpValidator(
            FileHelper::fileNameValidation(), lineEdit));

    lineEdit->setText(index.data(Qt::DisplayRole).toString());
}
