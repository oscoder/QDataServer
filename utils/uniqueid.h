#ifndef UTILS_UNIQUEID_H
#define UTILS_UNIQUEID_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QMetaType>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT UniqueId
{
public:
    UniqueId();
    UniqueId(const char *id);
    UniqueId(const QString &id);

public:
    static inline bool hasUniqueId(const char *id);
    static inline bool hasUniqueId(const QString &id);

public:
    bool isValid() const;

    QString toString() const;
    QByteArray toLocal8Bit() const;

    inline int toInt() const;
    static UniqueId fromInt(int id);
    static UniqueId fromInt(int id, bool *ok);

public:
    UniqueId &operator=(const char *id);
    UniqueId &operator=(const QString &id);

    friend bool operator==(const UniqueId &id1, const UniqueId &id2);
    friend bool operator!=(const UniqueId &id1, const UniqueId &id2);
    bool operator<(const UniqueId &other) const;

private:
    explicit inline UniqueId(int id);
    static int uniqueId(const QString &id);
    static inline bool isKnown(int id);

private:
    enum {
        INVALID_ID = -1
    };
    int m_id;
    static QHash<QString, int> m_idMap;
};

/*! \relates UniqueId */
inline uint qHash(UniqueId id)
{
    return uint(id.toInt());
}

inline UniqueId::UniqueId()
    : m_id(INVALID_ID)
{
}

inline UniqueId::UniqueId(const char *id)
    : m_id(uniqueId(QString(id)))
{
}

inline UniqueId::UniqueId(const QString &id)
    : m_id(uniqueId(id))
{
}

inline bool UniqueId::hasUniqueId(const char *id)
{
    return hasUniqueId(QString(id));
}

inline bool UniqueId::hasUniqueId(const QString &id)
{
    return m_idMap.contains(id);
}

inline bool UniqueId::isValid() const
{
    return m_id != INVALID_ID;
}

inline QString UniqueId::toString() const
{
    Q_ASSERT(isKnown(m_id));
    return m_idMap.key(m_id);
}

inline QByteArray UniqueId::toLocal8Bit() const
{
    return toString().toLocal8Bit();
}

inline int UniqueId::toInt() const
{
    return m_id;
}

inline UniqueId UniqueId::fromInt(int id)
{
    Q_ASSERT(isKnown(id));
    return UniqueId(id);
}

inline UniqueId UniqueId::fromInt(int id, bool *ok)
{
    Q_ASSERT(ok != 0);
    return UniqueId((*ok = isKnown(id)) ? id : INVALID_ID);
}

inline UniqueId &UniqueId::operator=(const QString &id)
{
    m_id = uniqueId(id);
    return *this;
}

inline UniqueId &UniqueId::operator=(const char *id)
{
    m_id = uniqueId(QString(id));
    return *this;
}

/*! \relates UniqueId */
inline bool operator==(const UniqueId &id1, const UniqueId &id2)
{
    return id1.m_id == id2.m_id;
}

/*! \relates UniqueId */
inline bool operator!=(const UniqueId &id1, const UniqueId &id2)
{
    return id1.m_id != id2.m_id;
}

inline bool UniqueId::operator<(const UniqueId &other) const
{
    return m_id < other.m_id;
}

inline UniqueId::UniqueId(int id)
    : m_id(id)
{
    Q_ASSERT(isKnown(id));
}

inline bool UniqueId::isKnown(int id)
{
    return id >= INVALID_ID && id < m_idMap.size();
}

} //namespace Utils

#if defined(Q_CC_MSVC)
UTILS_EXTERN template class UTILS_EXPORT QSet<Utils::UniqueId>;
#endif

Q_DECLARE_METATYPE(Utils::UniqueId)

#endif // UTILS_UNIQUEID_H
