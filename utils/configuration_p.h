#ifndef UTILS_CONFIGURATION_P_H
#define UTILS_CONFIGURATION_P_H
/*! \cond __pimpl */

#include "configuration.h"

#include <QtCore/QMap>
#include <QtCore/QSharedData>

namespace Utils {

class Configuration::Data : public QSharedData
{
    friend class Configuration;

private:
    Data(const Resource &resource);
    Data(const Configuration &left, Type type, const Configuration &right);
    Data(Type type, const Configuration &right);
    Data(const Resource &resource, Type type, const Version &version);

private:
    bool satisfiesVersion(Type relation, const Version &version) const;

private:
    static QMap<UniqueId, VersionCompareFunction> versionCompareFunctions;
    const Type type;
    const Configuration left;
    const Configuration right;
    const Resource resource;
    const Version version;
};

inline Configuration::Data::Data(const Resource &resource)
    : type(Exists),
      resource(resource)
{}

inline Configuration::Data::Data(const Configuration &left, Type type, const
        Configuration &right)
    : type(type),
      left(left),
      right(right)
{
    Q_ASSERT(type == Comma || type == And || type == Or);
}

inline Configuration::Data::Data(Type type, const Configuration &right)
    : type(Not),
      right(right)
{
    Q_ASSERT(type == Not);
    Q_UNUSED(type);
}

inline Configuration::Data::Data(const Resource &resource, Type type, const
        Version &version)
    : type(type),
      resource(resource),
      version(version)
{
    Q_ASSERT(type >= Lt && type <= Gt);
}

} //namespace Utils

/*! \endcond */
#endif //UTILS_CONFIGURATION_P_H
