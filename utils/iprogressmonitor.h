#ifndef IPROGRESSMONITOR_H
#define IPROGRESSMONITOR_H

#include <QString>

#include "utils_global.h"

namespace Utils {

/*!
  \class IProgressMonitor
  \brief interface class to provide monitoring of progress
  */
class UTILS_EXPORT IProgressMonitor
{

public:
    /*!
      set the current status of progress in verbal form
      \param status verbal status
      */
    virtual void setStatus(const QString &status) = 0;

};

} // namespace Utils

#endif // IPROGRESSMONITOR_H
