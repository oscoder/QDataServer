#ifndef PLUGINLOADER_PLUGINVIEW_H
#define PLUGINLOADER_PLUGINVIEW_H

#include "pluginloader_global.h"

#include <QtGui/QWidget>

namespace PluginLoader {

class PluginViewPrivate;

/*!
    \brief Widget that shows a list of all plugins and their state.

    This can be embedded e.g. in a dialog in the application that uses the
    plugin manager.
*/
class PLUGINLOADER_EXPORT PluginView : public QWidget
{
    Q_OBJECT

public:
    explicit PluginView(QWidget *parent = 0);
    virtual ~PluginView();

signals:
    //! Emitted when selection of plugins has changed.
    void pluginSettingsChanged();

private:
    Q_DECLARE_PRIVATE(PluginView)
    PluginViewPrivate *d_ptr;
};

} // namespae PluginLoader

#endif // PLUGINLOADER_PLUGINVIEW_H
