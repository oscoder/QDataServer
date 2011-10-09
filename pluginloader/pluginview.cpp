#include "pluginview.h"
#include "pluginview_p.h"

#include <QtCore/QDir>
#include <QtGui/QHeaderView>
#include <QtGui/QPalette>
#include <QtGui/QTreeWidgetItem>

#include "pluginmanager.h"
#include "pluginspec.h"

#include "ui_pluginview.h"

using namespace PluginLoader;

/*!
    Constructs a new PluginView with \a parent.
    \param parent the parent widget
 */
PluginView::PluginView(QWidget *parent)
    : QWidget(parent),
      d_ptr(new PluginViewPrivate(this))
{
}

PluginView::~PluginView()
{
    Q_D(PluginView);
    delete d;
}

namespace {
    const int C_NAME = 0;
    const int C_ENABLED = 1;
    const int C_INDIRECTLY_DISABLED = 2;
    const int C_VERSION = 3;
    const int C_DESCRIPTION = 4;
    const int C_DEPENDENCY = 5;
}

PluginViewPrivate::PluginViewPrivate(PluginView *q)
    : q_ptr(q),
    m_ui(new Ui::PluginView)
{
    m_ui->setupUi(q);

    QHeaderView *header = m_ui->pluginsTree->header();
    header->setResizeMode(C_NAME, QHeaderView::ResizeToContents);
    header->setResizeMode(C_ENABLED, QHeaderView::ResizeToContents);
    header->setResizeMode(C_INDIRECTLY_DISABLED, QHeaderView::ResizeToContents);
    header->setResizeMode(C_VERSION, QHeaderView::ResizeToContents);

    connect(m_ui->pluginsTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updatePluginSettings(QTreeWidgetItem*,int)));

    createTree();
}

PluginViewPrivate::~PluginViewPrivate()
{
    delete m_ui;
}

QIcon PluginViewPrivate::getIcon(IconType type)
{
    static QIcon okIcon =
        QIcon(QLatin1String(":/pluginloader/images/ok.png"));
    static QIcon errorIcon =
        QIcon(QLatin1String(":/pluginloader/images/error.png"));
    static QIcon notLoadedIcon =
        QIcon(QLatin1String(":/pluginloader/images/not-loaded.png"));

    if (type & IconError) {
        return errorIcon;
    }
    else if (type & IconNotLoaded) {
        return notLoadedIcon;
    }
    return okIcon;
}

void PluginViewPrivate::createTree()
{
    QMultiMap<QString, PluginSpec *> pluginCollections;

    const QList<PluginSpec *> pluginSpecs =
        PluginManager::instance()->pluginSpecs();
    foreach (PluginSpec *pluginSpec, pluginSpecs) {
        pluginCollections.insert(pluginSpec->category(), pluginSpec);
    }

    const QStringList categories = pluginCollections.uniqueKeys();
    foreach (const QString &category, categories) {
        if (category.isEmpty()) {
            parsePluginSpecs(0, pluginCollections.values(category));
        }
        else {
            QTreeWidgetItem *categoryItem = new QTreeWidgetItem(QStringList()
                << category // name
                << QString() // enabled
                << QString() // indirectly disabled
                << QString() // version
                << QString() // description
                << QString()); // dependency
            m_items.append(categoryItem);

            Qt::CheckState catogoryCheckState = parsePluginSpecs(categoryItem,
                    pluginCollections.values(category));

            categoryItem->setData(C_ENABLED, Qt::CheckStateRole,
                    QVariant(catogoryCheckState));
            categoryItem->setData(C_NAME, Qt::UserRole,
                    qVariantFromValue(category));

            categoryItem->setToolTip(C_NAME,
                    PluginView::tr("Category: %1").arg(category));
        }
    }

    m_ui->pluginsTree->clear();
    if (!m_items.isEmpty()) {
        m_ui->pluginsTree->addTopLevelItems(m_items);
        m_ui->pluginsTree->expandAll();
    }

    m_ui->pluginsTree->sortItems(C_NAME, Qt::AscendingOrder);
    if (m_ui->pluginsTree->topLevelItemCount())
        m_ui->pluginsTree->setCurrentItem(m_ui->pluginsTree->topLevelItem(0));
}

void PluginViewPrivate::updateTree()
{
    for (int i = 0; i < m_ui->pluginsTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *topLevelItem = m_ui->pluginsTree->topLevelItem(i);
        if (topLevelItem->data(C_NAME, Qt::UserRole).canConvert<QString>()) {
            // Top level item is category folder
            bool someEnabled = false;
            bool someDisabled = false;

            for (int j = 0; j < topLevelItem->childCount(); ++j) {
                QTreeWidgetItem *item = topLevelItem->child(j);
                if (item->data(C_NAME, Qt::UserRole).canConvert<PluginSpec *>()) {
                    PluginSpec *spec =
                        item->data(C_NAME, Qt::UserRole).value<PluginSpec *>();

                    Qt::CheckState enabledCheckState = Qt::Unchecked;
                    if (spec->isEnabled()) {
                        enabledCheckState = Qt::Checked;
                        someEnabled = true;
                    }
                    else {
                        someDisabled = true;
                    }
                    item->setData(C_ENABLED, Qt::CheckStateRole,
                            enabledCheckState);

                    Qt::CheckState indirectlyDisabledCheckState =
                        spec->isIndirectlyDisabled()
                        ? Qt::Checked : Qt::Unchecked;

                    item->setData(C_INDIRECTLY_DISABLED, Qt::CheckStateRole,
                            indirectlyDisabledCheckState);
                }
            }

            Qt::CheckState categoryCheckState = Qt::Unchecked;
            if (someEnabled && !someDisabled) {
                categoryCheckState = Qt::Checked;
            }
            else if (someEnabled && someDisabled) {
                categoryCheckState = Qt::PartiallyChecked;
            }
            else {
                categoryCheckState = Qt::Unchecked;
            }
            topLevelItem->setData(C_ENABLED, Qt::CheckStateRole,
                    QVariant(categoryCheckState));
        }
        else if (topLevelItem->data(C_NAME,
                    Qt::UserRole).canConvert<PluginSpec *>()) {
            // Top level item is plugin
            PluginSpec *spec =
                topLevelItem->data(C_NAME, Qt::UserRole).value<PluginSpec *>();

            Qt::CheckState checkState =
                spec->isEnabled() ? Qt::Checked : Qt::Unchecked;
            topLevelItem->setData(C_ENABLED, Qt::CheckStateRole, checkState);

            Qt::CheckState indirectlyDisabledCheckState =
                spec->isIndirectlyDisabled() ? Qt::Checked : Qt::Unchecked;

            topLevelItem->setData(C_INDIRECTLY_DISABLED, Qt::CheckStateRole,
                    indirectlyDisabledCheckState);
        }
    }
}

Qt::CheckState PluginViewPrivate::parsePluginSpecs(QTreeWidgetItem *parentItem,
        QList<PluginSpec*> pluginSpecs)
{
    bool someEnabled = false;
    bool someDisabled = false;

    foreach (PluginSpec *spec, pluginSpecs) {

        QString dependecies;
        bool firstDependency = true;
        foreach (const PluginDependency &dependency, spec->dependencies()) {
            if(!firstDependency) {
                dependecies += ", ";
            }
            else {
                firstDependency = false;
            }
            dependecies += dependency.name;
        }

        QTreeWidgetItem *pluginItem = new QTreeWidgetItem(QStringList()
            << spec->name() // name
            << QString() // enabled
            << QString() // indirectly disabled
            << spec->version() // version
            << spec->description() // description
            << dependecies); // dependency

        IconType iconType;
        QString tooltip;
        if (spec->hasError()) {
            iconType = IconError;
            tooltip = PluginView::tr("Plugin error:\n%1").arg(spec->errorString());
        }
        else if (!spec->hasError() && !spec->plugin()) {
            iconType = IconNotLoaded;
            tooltip = PluginView::tr("Plugin not loaded.");
        }
        else {
            iconType = IconOK;
        }

        pluginItem->setToolTip(C_NAME, tooltip);
        pluginItem->setIcon(C_NAME, getIcon(iconType));
        pluginItem->setData(C_NAME, Qt::UserRole, qVariantFromValue(spec));

        Qt::CheckState enabledCheckState = Qt::Unchecked;
        if (spec->isEnabled()) {
            enabledCheckState = Qt::Checked;
            someEnabled = true;
        }
        else {
            someDisabled = true;
        }

        pluginItem->setData(C_ENABLED, Qt::CheckStateRole, enabledCheckState);

        if (spec->isPersistent()) {
            pluginItem->setData(C_ENABLED, Qt::CheckStateRole, Qt::Checked);
            pluginItem->setFlags(Qt::ItemIsSelectable);
        }

        Qt::CheckState indirectlyDisabledCheckState =
            spec->isIndirectlyDisabled() ? Qt::Checked : Qt::Unchecked;

        pluginItem->setData(C_INDIRECTLY_DISABLED, Qt::CheckStateRole,
                indirectlyDisabledCheckState);

        m_specToItem.insert(spec, pluginItem);

        if (parentItem) {
            parentItem->addChild(pluginItem);
        }
        else {
            m_items.append(pluginItem);
        }
    }

    Qt::CheckState parentCheckState = Qt::Unchecked;
    if (someEnabled && !someDisabled) {
        parentCheckState = Qt::Checked;
    }
    else if (someEnabled && someDisabled) {
        parentCheckState = Qt::PartiallyChecked;
    }
    else {
        parentCheckState = Qt::Unchecked;
    }

    return parentCheckState;
}

void PluginViewPrivate::updatePluginSettings(QTreeWidgetItem *item, int column)
{
    Q_Q(PluginView);

    if (column != C_ENABLED) {
        updateTree();
        return;
    }

    static bool updateInProgress = false;

    if (updateInProgress) {
        return;
    }
    else {
        updateInProgress = true;
    }

    bool enabled = item->data(C_ENABLED, Qt::CheckStateRole).toBool();

    if (item->data(C_NAME, Qt::UserRole).canConvert<PluginSpec *>()) {
        // User changed enabled flag for single plugin
        PluginSpec *spec =
            item->data(C_NAME, Qt::UserRole).value<PluginSpec *>();

        if (column == C_ENABLED) {
            spec->setEnabled(enabled);
            foreach (PluginSpec *providesSpec, spec->providesForSpecs()) {
                providesSpec->resolveIndirectlyDisabled(true);
            }
        }
    }
    else if (item->data(C_NAME, Qt::UserRole).canConvert<QString>()) {
        // User changed enabled flag for whole category
        QString category = item->data(C_NAME, Qt::UserRole).value<QString>();

        QMultiMap<QString, PluginSpec *> pluginCollections;
        QList<PluginSpec *> pluginSpecs =
            PluginManager::instance()->pluginSpecs();
        foreach (PluginSpec *pluginSpec, pluginSpecs) {
            pluginCollections.insert(pluginSpec->category(), pluginSpec);
        }

        foreach (PluginSpec *spec, pluginCollections.values(category)) {
            spec->setEnabled(enabled);
            foreach (PluginSpec *providesSpec, spec->providesForSpecs()) {
                providesSpec->resolveIndirectlyDisabled(true);
            }
        }
    }
    else {
        return;
    }

    updateTree();

    updateInProgress = false;

    emit q->pluginSettingsChanged();
}
