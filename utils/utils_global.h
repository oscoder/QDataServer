#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILS_LIBRARY)
#  define UTILS_EXPORT Q_DECL_EXPORT
#else
#  define UTILS_EXPORT Q_DECL_IMPORT
#endif

#if defined(UTILS_LIBRARY) || !defined(Q_CC_MSVC)
#  define UTILS_EXTERN /* nothing */
#else
#  define UTILS_EXTERN extern
#endif

#endif // UTILS_GLOBAL_H
