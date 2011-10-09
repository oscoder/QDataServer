#ifndef UTILS_AGGREGATION11_H
#define UTILS_AGGREGATION11_H

#include <QtCore/QMap>
#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>

#include "utils_global.h"

#if defined(Q_CC_MSVC)
/* We do not provide definition for static Aggregation11::outerByInnerMap to
 * prevent extra instantiation in every linked binary. With MSVC, the code is
 * compiled with this warning:
 *
 * No suitable definition provided for explicit template instantiation request
 */
# pragma warning(disable:4661)
#endif

namespace Utils {

//! Helper class template to implement 1:1 aggregations
/*!
 * This class template can help you if you need to ensure only one instance of
 * \a OUTER_T exists for one instance of \a INNER_T, i.e., the relation between
 * \a OUTER_T and \a INNER_T instances is strictly 1:1. This is mostly useful
 * while implementing wrapper/adapter classes, where you need to preserve
 * identity.
 *
 * \verbatim
 * +---------+ 1        1 +---------+
 * | OUTER_T |<>--------->| INNER_T |
 * +---------+            +---------+
 * \endverbatim
 *
 * This class template expects the constructor of the \a OUTER_T to accept one
 * argument -- an instance of \a INNER_T and provides a static member function
 * create() which is then to be used to construct \a OUTER_T instances.
 *
 * \attention If, for whatever reason, the INNER_T association end for your
 * OUTER_T instance changes, you MUST call aggregation11ResetInner()
 * immediately to keep the internal map of instances consistent!
 *
 * It is also required to instantiate the class' (private) static data for your
 * combination of template arguments - linker will remind you. Use the macro
 * AGGREGATION11_DEFINE_STATIC_DATA() to do so.
 *
 * Here is an example how to use this class
 *
 * \code
 * // iresource.h
 * class IResource
 * {
 *     // ...
 * };
 * \endcode
 *
 * \code
 * // actualresource.h
 * class ActualResource
 * {
 *      // ...
 * };
 * \endcode
 *
 * \code
 * // fooresource.h
 * #include <iresource.h>
 * #include <actualresource.h>
 *
 * class FooResource : public IResource,
 *                     public Aggregation11<FooResource, ActualResource>
 * {
 * private:
 *     friend class Aggregation11<FooResource, ActualResource>;
 *     explicit FooResource(const ActualResource &actualResource);
 *     FooResource(const FooResource &other);
 *
 * public:
 *     ~FooResource();
 *
 *     // ...
 *
 * private:
 *     const ActualResource m_actualResource;
 * };
 * \endcode
 *
 * \code
 * // fooresource.cpp
 * #include "fooresource.h"
 *
 * AGGREGATION11_DEFINE_STATIC_DATA(FooResource);
 * ...
 * \endcode
 *
 * \code
 * #include "fooresource.h"
 * // ...
 * ActualResource *const actualResource1 = ...;
 * ActualResource *const actualResource2 = ...;
 *
 * FooResource *const resource11 = FooResource::create(actualResource1);
 * FooResource *const resource12 = FooResource::create(actualResource1);
 * FooResource *const resource21 = FooResource::create(actualResource2);
 * FooResource *const resource22 = FooResource::create(actualResource2);
 *
 * Q_ASSERT(resource11 != resource21);
 * Q_ASSERT(resource11 == resource12);
 * Q_ASSERT(resource21 == resource22);
 * // ...
 * \endcode
 */
template<class OUTER_T, class INNER_T>
class Aggregation11
{
public:
    //! Shortcut - with this it is possible to omit the template argumens
    typedef Aggregation11<OUTER_T, INNER_T> Aggregation11Base;

public:
    //! Use this function instead of OUTER_T's constructor()
    static QSharedPointer<OUTER_T> create(const INNER_T &inner);

    //! Use it to probe if the instance for \a inner exists
    static QSharedPointer<OUTER_T> aggregation11Probe(const INNER_T &inner);

protected:
    //! Call it whenever the association changes to update the map of instances
    void aggregation11ResetInner(const INNER_T &newInner);

private:
    static void destroy(OUTER_T *ptr);

private:
    /*! \cond false */
    // We need to store the pointer to OUTER_T instance in two ways
    //
    //  - as a QWeakPointer
    //      - that way we can construct QSharedPointer from it
    //  - as a plain pointer
    //      - this allow us to search for it when the QWeakPointer is already
    //      set to 0 -- in order to remove it from the outerByInnerMap
    //
    // This class combines those two ways
    struct OuterPointer
    {
        OuterPointer(const QSharedPointer<OUTER_T> &sharedPointer)
            : m_weakPointer(sharedPointer),
              m_plainPointer(sharedPointer.data())
        {
        }

        OuterPointer(OUTER_T *plainPointer = 0)
            : m_weakPointer(),
              m_plainPointer(plainPointer)
        {
        }

        QWeakPointer<OUTER_T> weakPointer() const
        {
            Q_ASSERT(!m_weakPointer.isNull()); // important!
            return m_weakPointer;
        }
        bool operator==(const OuterPointer &other) const
        {
            return m_plainPointer == other.m_plainPointer;
        }

    private:
        // We do not store const pointers to allow assignment..
        QWeakPointer<OUTER_T> m_weakPointer;
        OUTER_T *m_plainPointer;
    };

    // To use QWeakPointer as a key in QMap, we need the '<' operator
    template<class T>
    struct ComparableWeakPointer : public QWeakPointer<T>
    {
        ComparableWeakPointer()
        {
        }

        ComparableWeakPointer(const QSharedPointer<T> &sharedPointer)
            : QWeakPointer<T>(sharedPointer)
        {
        }

        bool operator<(const ComparableWeakPointer &other) const
        {
            Q_ASSERT(this->data() != 0);
            Q_ASSERT(other.data() != 0);
            return this->data() < other.data();
        }
    };

    // In case the INNER_T is actually QSharedPointer<INNER_T>, QMap is not
    // enough as we can not use QSharedPointer<INNER_T> as a key.
    template<class K, class V>
    struct OuterByInnerMap_ : public QMap<K, V>
    {
    };

    template<class K, class V>
    struct OuterByInnerMap_<QSharedPointer<K>, V> : public
            QMap<ComparableWeakPointer<K>, V>
    {
    };

    // IMPORTANT: This typedef-step is required to compile on MSVC
    typedef OuterByInnerMap_<INNER_T, OuterPointer> OuterByInnerMap;
    static OuterByInnerMap outerByInnerMap;
    /*! \endcond */
};

#if defined(Q_CC_MSVC)
# define AGGREGATION11_DEFINE_STATIC_DATA(CLASS) \
    template<> \
        CLASS::Aggregation11Base::OuterByInnerMap \
        CLASS::Aggregation11Base::outerByInnerMap = \
        CLASS::Aggregation11Base::OuterByInnerMap();
#else
# define AGGREGATION11_DEFINE_STATIC_DATA(CLASS) \
    template<> Q_DECL_EXPORT \
        CLASS::Aggregation11Base::OuterByInnerMap \
        CLASS::Aggregation11Base::outerByInnerMap = \
        CLASS::Aggregation11Base::OuterByInnerMap();
#endif

template<class OUTER_T, class INNER_T>
QSharedPointer<OUTER_T> Aggregation11<OUTER_T, INNER_T>::create(const INNER_T
        &inner)
{
    if (!outerByInnerMap.contains(inner)) {
        QSharedPointer<OUTER_T> outer(new OUTER_T(inner), destroy);
        outerByInnerMap.insert(inner, outer);
        return outer; // important: it MUST be returned from here!
    }

    return outerByInnerMap.value(inner).weakPointer();
}

template<class OUTER_T, class INNER_T>
QSharedPointer<OUTER_T> Aggregation11<OUTER_T, INNER_T>::aggregation11Probe(
        const INNER_T &inner)
{
    if (outerByInnerMap.contains(inner))
        return outerByInnerMap.value(inner).weakPointer();
    else
        return QSharedPointer<OUTER_T>();
}

template<class OUTER_T, class INNER_T>
void Aggregation11<OUTER_T, INNER_T>::aggregation11ResetInner(const INNER_T
        &newInner)
{
    Q_ASSERT(aggregation11Probe(newInner) == 0);

    INNER_T oldInner = outerByInnerMap.key(
            OuterPointer(static_cast<OUTER_T *>(this)));
    const QSharedPointer<OUTER_T> outer =
        outerByInnerMap.value(oldInner).weakPointer();
    outerByInnerMap.remove(oldInner);
    outerByInnerMap.insert(newInner, outer);
}

template<class OUTER_T, class INNER_T>
void Aggregation11<OUTER_T, INNER_T>::destroy(OUTER_T *ptr)
{
#if !defined(QT_NO_DEBUG)
    const int nRemoved =
#endif
    outerByInnerMap.remove(outerByInnerMap.key(OuterPointer(ptr)));
#if !defined(QT_NO_DEBUG)
    Q_ASSERT(nRemoved == 1);
#endif

    ptr->deleteLater();
}

} //namespace Utils

#endif // UTILS_AGGREGATION11_H
