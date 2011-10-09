#ifndef UTILS_TRACEFN_H
#define UTILS_TRACEFN_H

#include "utils_global.h"

#include <QtCore/QtGlobal>

namespace Utils {

/*! \cond false */ /* undocumented feature */
#if defined(QT_NO_DEBUG)
# define TraceFn() /* nothing */
#else
# define TraceFn() Utils::Tracer(this, Q_FUNC_INFO);
class UTILS_EXPORT Tracer
{
public:
    Tracer(const void *instance, const char *function)
        : m_instance(instance),
          m_function(function)
    {
        qDebug("%s: %p/%p [[[", m_function, m_instance, this);
    }

    ~Tracer()
    {
        qDebug("%s: %p/%p ]]]", m_function, m_instance, this);
    }

private:
    const void *const m_instance;
    const char *const m_function;
};
#endif
/*! \endcond */

} //namespace Utils

#endif //UTILS_TRACEFN_H
