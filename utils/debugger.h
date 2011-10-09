#ifndef UTILS_DEBUGGER_H
#define UTILS_DEBUGGER_H

#include <QtCore/QString>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT Debugger
{
public:
    static QString backtrace();
};

} // namespace Utils

#endif // UTILS_DEBUGGER_H
