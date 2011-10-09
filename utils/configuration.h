#ifndef UTILS_CONFIGURATION_H
#define UTILS_CONFIGURATION_H

#include <QtCore/QLatin1String>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

#include "uniqueid.h"

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT Configuration
{
public:
    enum Type {
        Not,
        Comma, And, Or,
        Exists,
        Lt, Le, Eq, Ne, Ge, Gt
    };

    class Resource;
    class Version;

    typedef int (*VersionCompareFunction)(const Version &, const Version &);

public:
    Configuration();
    Configuration(const Resource &resource);
    Configuration(const Configuration &other);
    ~Configuration();

private:
    // private constructors are used by the following friend operators
    Configuration(const Configuration &left, Type type, const Configuration
            &right);
    Configuration(Type type, const Configuration &right);
    Configuration(const Resource &resource, Type type, const Version &version);

    friend Configuration operator,(const Configuration &e1, const Configuration
            &e2);
    friend Configuration operator,(const Configuration &e, const Resource &r);
    friend Configuration operator&&(const Configuration &e1, const
            Configuration &e2);
    friend Configuration operator||(const Configuration &e1, const
            Configuration &e2);
    friend Configuration operator!(const Configuration &e);
    friend Configuration operator<(const Resource &r, const Version &v);
    friend Configuration operator<=(const Resource &r, const Version &v);
    friend Configuration operator==(const Resource &r, const Version &v);
    friend Configuration operator!=(const Resource &r, const Version &v);
    friend Configuration operator>=(const Resource &r, const Version &v);
    friend Configuration operator>(const Resource &r, const Version &v);

public:
    static void registerVersionCompareFunction(const Resource &resource,
            VersionCompareFunction versionCompareFunction);
    static int defaultVersionCompareFunction(const Version &v1, const Version
            &v2);

public:
    bool isNull() const;
    bool satisfies(const Configuration &requiredConfiguration) const;

public:
    Type type() const;
    Configuration left() const;
    Configuration right() const;
    Resource resource() const;
    Version version() const;

private:
    class Data;
    QSharedDataPointer<Data> d;
};

class UTILS_EXPORT Configuration::Resource
{
public:
    Resource();
    explicit Resource(UniqueId id);

public:
    UniqueId id() const;
    bool isNull() const;

    bool operator==(const Resource &o) const { return m_id == o.m_id; }

private:
    const UniqueId m_id;
};

class UTILS_EXPORT Configuration::Version
{
public:
    Version();
    Version(const char *spec);
    Version(const QLatin1String &spec);
    Version(const QString &spec);

public:
    QString spec() const;
    bool isNull() const;

private:
    const QString m_spec;
};

inline bool Configuration::isNull() const
{
    return d.data() == 0;
}

inline Configuration::Resource::Resource()
{
}

inline Configuration::Resource::Resource(UniqueId id)
    : m_id(id)
{
    Q_ASSERT(id.isValid());
}

inline bool Configuration::Resource::isNull() const
{
    return !m_id.isValid();
}

inline UniqueId Configuration::Resource::id() const
{
    Q_ASSERT(!isNull());
    return m_id;
}

inline Configuration::Version::Version()
{
}

inline Configuration::Version::Version(const char *spec)
    : m_spec(spec)
{
    Q_ASSERT(!QString(spec).isEmpty());
}

inline Configuration::Version::Version(const QLatin1String &spec)
    : m_spec(spec)
{
    Q_ASSERT(!QString(spec).isEmpty());
}

inline Configuration::Version::Version(const QString &spec)
    : m_spec(spec)
{
    Q_ASSERT(!spec.isEmpty());
}

inline bool Configuration::Version::isNull() const
{
    return m_spec.isEmpty();
}

inline QString Configuration::Version::spec() const
{
    Q_ASSERT(!isNull());
    return m_spec;
}

//! Combines two configurations to one of type Configuration::Comma
/*! \relates Configuration */
inline Configuration operator,(const Configuration &e1, const Configuration
        &e2)
{
    return Configuration(e1, Configuration::Comma, e2);
}

//! Combines two configurations to one of type Configuration::Comma
/*! \relates Configuration */
inline Configuration operator,(const Configuration &e, const
        Configuration::Resource &r)
{
    return Configuration(e, Configuration::Comma, r);
}

//! Combines two configurations to one of type Configuration::And
/*! \relates Configuration */
inline Configuration operator&&(const Configuration &e1, const Configuration
        &e2)
{
    return Configuration(e1, Configuration::And, e2);
}

//! Combines two configurations to one of type Configuration::Or
/*! \relates Configuration */
inline Configuration operator||(const Configuration &e1, const Configuration
        &e2)
{
    return Configuration(e1, Configuration::Or, e2);
}

//! Creates configuration of type Configuration::Not
/*! \relates Configuration */
inline Configuration operator!(const Configuration &e)
{
    return Configuration(Configuration::Not, e);
}

//! Combines \a r and \a v into configuration of type Configuration::Lt
/*! \relates Configuration */
inline Configuration operator<(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Lt, v);
}

//! Combines \a r and \a v into configuration of type Configuration::Le
/*! \relates Configuration */
inline Configuration operator<=(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Le, v);
}

//! Combines \a r and \a v into configuration of type Configuration::Eq
/*! \relates Configuration */
inline Configuration operator==(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Eq, v);
}

//! Combines \a r and \a v into configuration of type Configuration::Ne
/*! \relates Configuration */
inline Configuration operator!=(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Ne, v);
}

//! Combines \a r and \a v into configuration of type Configuration::Ge
/*! \relates Configuration */
inline Configuration operator>=(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Ge, v);
}

//! Combines \a r and \a v into configuration of type Configuration::Gt
/*! \relates Configuration */
inline Configuration operator>(const Configuration::Resource &r, const
        Configuration::Version &v)
{
    return Configuration(r, Configuration::Gt, v);
}

} //namespace Utils

#endif //UTILS_CONFIGURATION_H
