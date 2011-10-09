#include "uniqueid.h"

#if defined(QT_DEBUG)
#include <QtCore/QRegExp>
#endif

using namespace Utils;

/*!
 * \class Utils::UniqueId
 * \brief Fast manipulation with human-readable unique identifiers
 */

/*!
  \var UniqueId::m_idMap
  id map
  */
QHash<QString, int> UniqueId::m_idMap;

/*!
 * \fn Utils::UniqueId::UniqueId()
 * \brief Constructs an invalid ID
 * \see isValid()
 */

/*!
 * \fn Utils::UniqueId::UniqueId(const char *id)
 * \brief This is an overloaded contructor
 */

/*!
 * \fn Utils::UniqueId::UniqueId(const QString &id)
 * \brief Primary constructor
 */

int UniqueId::uniqueId(const QString &id)
{
    Q_ASSERT(!id.isEmpty());

    const QHash<QString, int>::const_iterator it = m_idMap.find(id);
    if (it != m_idMap.end())
        return *it;

#if defined(QT_DEBUG)
    static QRegExp *const space = new QRegExp("\\s");
    if (id.contains(*space))
        qWarning("%s: id contains spaces <%s>", Q_FUNC_INFO, qPrintable(id));
#endif

    const int uid = m_idMap.size();
    m_idMap.insert(id, uid);
    return uid;
}

/*!
 * \fn bool Utils::UniqueId::isValid() const
 * \brief ID contructed using the default constructor is invalid
 */

/*!
 * \fn QString Utils::UniqueId::toString() const
 * \brief Query the human-readable form of the identifier it represents
 */

/*!
 * \fn QByteArray Utils::UniqueId::toLocal8Bit() const
 * \brief Enables use of \c qPrintable() macro to print UniqueId values
 */

/*!
 * \fn int Utils::UniqueId::toInt() const
 * \brief Enables to store UniqueId in \c union
 * \attention The only valid case you can use this function is to store the
 * represented value into a C \c union which can not store values of types with
 * constructor. DO NOT MISUSE!
 * \see fromInt(int id), fromInt(int id, bool *ok)
 */

/*!
 * \fn Utils::UniqueId::fromInt(int id)
 * \brief Enables to store UniqueId on \c union
 *
 * This version may abort you application when an invalid \c id is given
 *
 * \attention The only valid case you can use this function is to store the
 * represented value into a C \c union which can not store values of types with
 * constructor. DO NOT MISUSE!
 * \see toInt(), fromInt(int id, bool *ok)
 */

/*!
 * \fn Utils::UniqueId::fromInt(int id, bool *ok)
 * \brief Enables to store UniqueId on \c union
 *
 * It will set \c *ok to \c false when an invalid \c id is given
 *
 * \attention The only valid case you can use this function is to store the
 * represented value into a C \c union which can not store values of types with
 * constructor. DO NOT MISUSE!
 * \see toInt(), fromInt(int id)
 */

/*!
 * \fn Utils::UniqueId::operator=(const QString &id)
 * \brief Assignment operator
 */

/*!
 * \fn Utils::UniqueId::operator=(const char *id)
 * \brief Assignment operator
 */

/*!
 * \fn Utils::operator==(const UniqueId &id1, const UniqueId &id2)
 * \brief Comparison operator
 * \relates UniqueId
 */

/*!
 * \fn Utils::operator!=(const UniqueId &id1, const UniqueId &id2)
 * \brief Comparison operator
 * \relates UniqueId
 */

/*!
 * \fn Utils::UniqueId::operator<(const UniqueId &other) const
 * \brief Comparison operator
 */

/*!
 * \fn bool Utils::UniqueId::hasUniqueId(const char *id)
 * \brief Query if any instance has been instantiated for the given \a id
 */

/*!
 * \fn bool Utils::UniqueId::hasUniqueId(const QString &id)
 * \brief Query if any instance has been instantiated for the given \a id
 */
