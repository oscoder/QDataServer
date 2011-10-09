#ifndef PLUGINLOADER_PLUGINVIEW_P_H
#define PLUGINLOADER_PLUGINVIEW_P_H
/*! \cond __pimpl */

#include "pluginview.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace PluginLoader {

namespace Ui {
    class PluginView;
} // namespace Ui

class PluginSpec;

class PluginViewPrivate : public QObject
{
    Q_OBJECT

public:
    explicit PluginViewPrivate(PluginView *q);
    virtual ~PluginViewPrivate();

private:
    enum IconType {
        IconOK = 1,
        IconNotLoaded = 2,
        IconError = 4
    };

private:
    QIcon getIcon(IconType type);
    void createTree();
    void updateTree();
    Qt::CheckState parsePluginSpecs(QTreeWidgetItem *parentItem,
            QList<PluginSpec*> pluginSpecs);

private slots:
    void updatePluginSettings(QTreeWidgetItem *item, int column);

private:
    Q_DECLARE_PUBLIC(PluginView)
    PluginView *q_ptr;

    Ui::PluginView *m_ui;
    QList<QTreeWidgetItem*> m_items;
    QHash<PluginSpec*, QTreeWidgetItem*> m_specToItem;
};

} // namespace PluginLoader

/*! \endcond */
#endif // PLUGINLOADER_PLUGINVIEW_P_H
