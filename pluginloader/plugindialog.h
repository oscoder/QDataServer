#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "pluginloader_global.h"

namespace PluginLoader {

class PluginView;

/*!
    \brief The dialog that allows to manage plugins.

    The PluginDialog class shows a dialog containing list of all available
    plugins. User can see which plugins are loaded correctly, which are not
    loaded because user explicitly disabled them, which are not loaded because
    of plugins they depend on are not loaded or plugin which are not loaded
    for some error.
    Error message can be found in a tooltip on particular row.
 */
class PLUGINLOADER_EXPORT PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(QWidget *parent);

private slots:
    void updateRestartRequired();

private:
    PluginView *m_view;
    QPushButton *m_closeButton;
    QLabel *m_restartRequired;
    static bool m_isRestartRequired;
};

} // namespace PluginLoader

#endif // PLUGINDIALOG_H
