#include "plugindialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "pluginview.h"

using namespace PluginLoader;

bool PluginDialog::m_isRestartRequired = false;

/*!
    Constructs a PluginDialog and fills the list of plugins.
    \param parent the parent widget
 */
PluginDialog::PluginDialog(QWidget *parent)
    : QDialog(parent),
      m_view(new PluginView(this))
{
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(m_view);

    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setEnabled(true);
    m_closeButton->setDefault(true);

    m_restartRequired = new QLabel(tr("Restart required."), this);
    if (!m_isRestartRequired)
        m_restartRequired->setVisible(false);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(m_restartRequired);
    hl->addStretch(5);
    hl->addWidget(m_closeButton);

    vl->addLayout(hl);

    resize(800, 400);
    setWindowTitle(tr("Installed Plugins"));

    connect(m_view, SIGNAL(pluginSettingsChanged()),
            this, SLOT(updateRestartRequired()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void PluginDialog::updateRestartRequired()
{
    // just display the notice all the time after once changing something
    m_isRestartRequired = true;
    m_restartRequired->setVisible(true);
}
