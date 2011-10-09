#include "debugger.h"

#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1 || defined(Q_OS_MAC)
# include <execinfo.h>
#endif

#if defined(Q_CC_GNU)
# include <cxxabi.h>
#endif

#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>

using namespace Utils;

/*!
 * \class Utils::Debugger utils/debugger.h
 * \brief Application self-debugging support
 */

//! Constructs human readable backtrace.
QString Debugger::backtrace()
{
    static const QString ERR_RETVAL = "<Failed to get backtrace>";

#if (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1 || defined(Q_OS_MAC)) && defined(Q_CC_GNU)
    static const int MAX_FRAMES = 256;

    void *addresses[MAX_FRAMES];
    const int size = ::backtrace(addresses, MAX_FRAMES);
    char **const symbols = backtrace_symbols(addresses, size);

    if (symbols == 0) {
        qWarning("%s: backtrace_symbols failed", Q_FUNC_INFO);
        return ERR_RETVAL;
    }

    QString retv;

#if defined(Q_OS_MAC)
    QRegExp regexp("^[0-9]+\\s+(\\S+)\\s+(0x[0-9a-f]+) ([^+]+) \\+ ([0-9]+)$");
#else
    QRegExp regexp("^(.*)\\(([^()+]*)\\+(0x[0-9a-f]+)?\\) (\\[0x[0-9a-f]+\\])$");
#endif

    for (int i = 1; i < size; ++i) {
        if (regexp.exactMatch(symbols[i])) {
#if defined(Q_OS_MAC)
            const QString source = QFileInfo(regexp.cap(1)).fileName();
            const QString mangledSymbol = regexp.cap(3);
            const QString offset = regexp.cap(4);
            const QString frame = regexp.cap(2);
#else
            const QString source = QFileInfo(regexp.cap(1)).fileName();
            const QString mangledSymbol = regexp.cap(2);
            const QString offset = regexp.cap(3);
            const QString frame = regexp.cap(4);
#endif

            char *const demangledSymbol =
                abi::__cxa_demangle(qPrintable(mangledSymbol), 0, 0, 0);
            const QString symbol =
                demangledSymbol != 0
                ? demangledSymbol
                : (!mangledSymbol.isEmpty()
                        ? mangledSymbol
                        : QString("<???????""?>"));

            retv.append(QString("%1 %2+%3 in %4\n")
                    .arg(frame)
                    .arg(symbol)
                    .arg(offset)
                    .arg(source));

            free(demangledSymbol);
        } else {
            retv.append(QString("<%1>\n").arg(symbols[i]));
        }
    }

    free(symbols);

    return retv;
#else
    qWarning("%s: Platform unsupported", Q_FUNC_INFO);
    return ERR_RETVAL;
#endif
}
