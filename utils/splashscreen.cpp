#include "splashscreen.h"

#include <QtGui/QPainter>

namespace Utils {

/*!
  \class SplashScreen
  \brief extended splash that support free text position
  */
/*!
  Constructor
  \param pixmap the background pixmap
  \param textPos the position of text
  */
SplashScreen::SplashScreen(const QPixmap &pixmap, const QPoint &textPos) :
    QSplashScreen(pixmap),
    m_textPos(textPos)
{
}

/*!
  \reimp
  */
void SplashScreen::setStatus(const QString &status)
{
    m_status = status;
    repaint();
}

/*!
  \reimp
  */
void SplashScreen::drawContents(QPainter *painter)
{
    painter->setFont(QFont("Arial", 8));
    painter->drawText(m_textPos, m_status);
}

} // namespace Utils
