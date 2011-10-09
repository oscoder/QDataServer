#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include "iprogressmonitor.h"

#include <QtCore/QPoint>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT SplashScreen : public QSplashScreen, public IProgressMonitor
{
    Q_OBJECT
public:
    explicit SplashScreen(const QPixmap &pixmap = QPixmap(), const QPoint &textPos = QPoint());

public:
    //from IProgressMonitor
    virtual void setStatus(const QString &status);
protected:
    // From QSplashScreen
    virtual void drawContents(QPainter *painter);

private:
    QString m_status;
    QPoint m_textPos;
};

} // namespace Utils

#endif // SPLASHSCREEN_H
