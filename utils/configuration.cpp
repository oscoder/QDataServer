#include "configuration_p.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>

using namespace Utils;

/*!
 * \class Utils::Configuration
 * \brief Describes set of required or provided resources.
 *
 * In the simplest form, a configuration can be described as an enumeration of
 * available resources, optionally with their exact versions mentioned.
 *
 * This simple form is well suitable while describing set of resources which
 * are \e provided.
 *
 * When describing set of \e required resources, it would not be possible to
 * express all the satisfying combinations of resources and their versions this
 * way -- for that case, it is possible to not specify versions exactly but
 * relatively and to combine partial configurations into complex expressions
 * using logical operations.
 *
 * For best readability, this class enables declarative style of writing such
 * expressions through operators overloading. This is the overall grammar
 * supported:
 *
 * \verbatim
   Configuration -> Configuration , Configuration
                 |  Configuration && Configuration
                 |  Configuration || Configuration
                 |  !Configuration
                 |  Resource
                 |  Resource { <, <=, ==, !=, >=, > } Version
   \endverbatim
 *
 * The effect of the <code>,</code> (comma) operator is the same as of
 * <code>&&</code>. Comma operator has lower priority.
 *
 * Here is an example:
 *
 * \code
namespace Constants {
const char *const SYMBIAN_PLATFORM  = "com.nokia.sw.symbian";
const char *const MEEGO_PLATFORM    = "com.nokia.sw.meego";
const char *const QT_FRAMEWORK      = "com.nokia.sw.qt";
const char *const QT_FRAMEWORK_GUI  = "com.nokia.sw.qt.gui";
const char *const QT_FRAMEWORK_QML  = "com.nokia.sw.qt.qml";
const char *const QML_SYMBIAN_BASE  = "com.nokia.sw.symbian.qml";
const char *const QML_MEEGO_BASE    = "com.nokia.sw.meego.qml";
const char *const QML_MEEGO_MEDIA   = "com.nokia.sw.meego.qml-media";
const char *const DISPLAY_LANDSCAPE = "com.nokia.hw.display.orient.landscape";
const char *const DISPLAY_PORTRAIT  = "com.nokia.hw.display.orient.portrait";
const char *const DISPLAY_RORATE    = "com.nokia.hw.display.orient.rotate";
const char *const DISPLAY_TOUCH     = "com.nokia.hw.display.touch-screen";
const char *const KBD_JOYSTICK      = "com.nokia.hw.keyboard.joystick";
const char *const KBD_MEDIA         = "com.nokia.hw.keyboard.media-buttons";
} //namespace Constants

using namespace Constants;
typedef Configuration::Resource Resource;

Configuration requiredConfiguration =
    ( Resource(MEEGO_PLATFORM)
    , !Resource(SYMBIAN_PLATFORM)
    , Resource(QT_FRAMEWORK) >= "4.6.5" && Resource(QT_FRAMEWORK) < "4.8"
    , Resource(QT_FRAMEWORK_GUI)
    , Resource(QT_FRAMEWORK_QML)
    , !Resource(DISPLAY_RORATE) ||
            ( Resource(QT_FRAMEWORK) >= "4.7"
            && Resource(QML_MEEGO_BASE) >= "1.3"))
    , !Resource(KBD_JOYSTICK) || Resource(QT_FRAMEWORK) >= "4.6.8"
    , !Resource(KBD_MEDIA) || Resource(QML_MEEGO_MEDIA) >= "1.2"
    );
 * \endcode
 *
 * \attention Note the use of the toplevel parenthesis -- this is required for
 * compilation to success!
 *
 * And here is an example of a provided configuration. One which satisfies
 * requirements specified by \c providedConfiguration.
 *
 * \code
Configuration providedConfiguration =
    ( Resource(MEEGO_PLATFORM)
    , Resource(QT_FRAMEWORK) == "4.7"
    , Resource(QT_FRAMEWORK_GUI)
    , Resource(QT_FRAMEWORK_QML)
    , Resource(KBD_JOYSTICK)
    );
 * \endcode
 *
 * To test if the \c providedConfiguration satisfies the \c
 * requiredConfiguration call satisfies() on the \c providedConfiguration.
 *
 * \code
 * if (providedConfiguration.satisfies(requiredConfiguration)) {
 *     ...
 * } else {
 *     ...
 * }
 * \endcode
 */

/*!
 * \enum Utils::Configuration::Type
 * \brief The type of this particular configuration expression
 */

/*!
 * \var Utils::Configuration::Not
 * \brief Satisfied if right() is not satisfied
 */

/*!
 * \var Utils::Configuration::Comma
 * \brief Satisfied if both left() and right() are satisfied
 */

/*!
 * \var Utils::Configuration::And
 * \brief Satisfied if both left() and right() are satisfied
 */

/*!
 * \var Utils::Configuration::Or
 * \brief Satisfied if any of left() and right() is satisfied
 */

/*!
 * \var Utils::Configuration::Exists
 * \brief Satisfied if resource() exists
 */

/*!
 * \var Utils::Configuration::Lt
 * \brief Satisfied if resource() exists in version older than version()
 */

/*!
 * \var Utils::Configuration::Le
 * \brief Satisfied if resource() exists in version older or equal to version()
 */

/*!
 * \var Utils::Configuration::Eq
 * \brief Satisfied if resource() exists in version equal to version()
 */

/*!
 * \var Utils::Configuration::Ne
 * \brief Satisfied if resource() exists in version other than version()
 */

/*!
 * \var Utils::Configuration::Ge
 * \brief Satisfied if resource() exists in version never or equal to version()
 */

/*!
 * \var Utils::Configuration::Gt
 * \brief Satisfied if resource() exists in version never than version()
 */

/*!
 * \typedef Utils::Configuration::VersionCompareFunction
 * \brief Pointer to function to compare two versions
 * \returns Number less than, equal or greater than zero if the version given
 * as the first argument is older, the same or newer than the second one.
 */

//! Constructs null configuration
Configuration::Configuration()
    : d(0)
{}

//! Constructs elementary configuration expression declaring resource existence
Configuration::Configuration(const Resource &resource)
    : d(new Data(resource))
{}

//! Copy constructor
Configuration::Configuration(const Configuration &other)
    : d(other.d)
{}

Configuration::~Configuration()
{
}

Configuration::Configuration(const Configuration &left, Type type, const
        Configuration &right)
    : d(new Data(left, type, right))
{}

Configuration::Configuration(Type type, const Configuration &right)
    : d(new Data(type, right))
{}

Configuration::Configuration(const Resource &resource, Type type, const Version
        &version)
    : d(new Data(resource, type, version))
{}

//! Registers function to use for comparing versions of the specified resource
/*!
 * For resources with no compare function registered, the
 * defaultVersionCompareFunction() will be used.
 *
 * Register a custom compare function when the defaultVersionCompareFunction()
 * cannot be used with the format of version string used with your particular
 * resource.
 */
void Configuration::registerVersionCompareFunction(const Resource &resource,
        VersionCompareFunction versionCompareFunction)
{
    Q_ASSERT(!Data::versionCompareFunctions.contains(resource.id()));
    Q_ASSERT(versionCompareFunction != 0);
    Q_ASSERT(versionCompareFunction != defaultVersionCompareFunction);

    Data::versionCompareFunctions.insert(resource.id(),
            versionCompareFunction);
}

//! Compares versions specified as a series of dot-separated numbers
/*!
 * The version specification strings must conform to this regular expression:
 * <tt>^[0-9]+(\\.[0-9]+)*$</tt>.
 */
int Configuration::defaultVersionCompareFunction(const Version &v1, const
        Version &v2)
{
    Q_ASSERT(QRegExp("[0-9]+(\\.[0-9]+)*").exactMatch(v1.spec()));
    Q_ASSERT(QRegExp("[0-9]+(\\.[0-9]+)*").exactMatch(v2.spec()));

    const QStringList sv1 = v1.spec().split('.');
    const QStringList sv2 = v2.spec().split('.');

    const int minCount = qMin(sv1.count(), sv2.count());
    for (int i = 0; i < minCount; i++) {
        if (const int cmp = sv1.at(i).toInt() - sv2.at(i).toInt())
            return cmp;
    }

    return sv1.count() - sv2.count();
}

/*!
 * \fn Utils::Configuration::isNull() const
 * \brief Null configurations are created using the default constructor
 */

//! Does this configuration meets requirements specified by the other one?
/*!
 * This verifies the set of resources declared by this configuration covers any
 * valid combination of resources and their versions, specified by
 * \a requiredConfiguration.
 *
 * \attention The configuration on the left side, can only be a plain
 * enumeration of resources and optionally their versions, built up using only
 * operators <code>,</code> (comma) and <code>==</code>.
 */
bool Configuration::satisfies(const Configuration &requiredConfiguration) const
{
    Q_UNUSED(requiredConfiguration);

    switch (requiredConfiguration.type()) {
    case Not:
        return !satisfies(requiredConfiguration.right());
    case Comma:
    case And:
        return satisfies(requiredConfiguration.left())
            && satisfies(requiredConfiguration.right());
    case Or:
        return satisfies(requiredConfiguration.left())
            || satisfies(requiredConfiguration.right());
    case Exists:
        switch (d->type) {
        case Comma:
            return left().satisfies(requiredConfiguration)
                || right().satisfies(requiredConfiguration);
        case Exists:
        case Eq:
            return resource() == requiredConfiguration.resource();
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Configuration on left side can "
                    "only use ',' and '==' operators.");
        }
    case Lt:
    case Le:
    case Eq:
    case Ne:
    case Ge:
    case Gt:
        switch (d->type) {
        case Comma:
            return left().satisfies(requiredConfiguration)
                || right().satisfies(requiredConfiguration);
        case Exists:
            if (resource() == requiredConfiguration.resource())
                qWarning("%s: Resource exists but version not specified - "
                        "cannot satisfy", Q_FUNC_INFO);
            return false;
        case Eq:
            return resource() == requiredConfiguration.resource()
                && d->satisfiesVersion(requiredConfiguration.type(),
                        requiredConfiguration.version());
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Configuration on left side can "
                    "only use ',' and '==' operators.");
        }
    }

    Q_ASSERT(false);
    return false;
}

//! Ask the type of this particular expression
Configuration::Type Configuration::type() const
{
    return d->type;
}

//! Valid for \c Comma, \c And and \c Or expressions
Configuration Configuration::left() const
{
    Q_ASSERT(d->type == Comma || d->type == And || d->type == Or);

    return d->left;
}

//! Valid for \c Comma, \c And, \c Or and \c Not expressions
Configuration Configuration::right() const
{
    Q_ASSERT(d->type == Comma || d->type == And || d->type == Or || d->type ==
            Not);

    return d->right;
}

//! Valid for \c Exists, \c Lt ... \c Gt expressions
Configuration::Resource Configuration::resource() const
{
    Q_ASSERT(d->type == Exists || (d->type >= Lt && d->type <= Gt));

    return d->resource;
}

//! Valid for \c Lt ... \c Gt expressions
Configuration::Version Configuration::version() const
{
    Q_ASSERT(d->type >= Lt && d->type <= Gt);

    return d->version;
}

/*!
 \class Utils::Configuration::Data
 \brief data holder
 */

/*!
  \var Configuration::Data::versionCompareFunctions
  version compare functions
  */
QMap<UniqueId, Configuration::VersionCompareFunction>
    Configuration::Data::versionCompareFunctions =
        QMap<UniqueId, Configuration::VersionCompareFunction>();

bool Configuration::Data::satisfiesVersion(Type relation, const Version
        &version) const
{
    const int cmp = versionCompareFunctions.value(resource.id(),
            defaultVersionCompareFunction)(this->version, version);

    switch (relation) {
    case Lt: return cmp < 0;
    case Le: return cmp <= 0;
    case Eq: return cmp == 0;
    case Ne: return cmp != 0;
    case Ge: return cmp >= 0;
    case Gt: return cmp > 0;
    default: Q_ASSERT(false);
             return false;
    }
}

/*!
 * \class Utils::Configuration::Resource
 * \brief Represents an entity being subject to configuration management
 *
 * A resource is identified by its unique identifier. There is no constraint
 * on how the indetifier should look like -- one can use what suits best for
 * the particular domain.
 */

/*!
 * \fn Configuration::Resource::Resource()
 * \brief Constructs a null resource
 */

/*!
 * \fn Configuration::Resource::Resource(UniqueId id)
 * \brief Constructs an instance describing resource with the given identifier
 */

/*!
 * \fn Configuration::Resource::id() const
 * \brief Returns the identifier passed on construction
 */

/*!
 * \fn Configuration::Resource::isNull() const
 * \brief Null resources are created using the default constructor
 */

/*!
 * \fn Configuration::Resource::operator==(const Resource &o) const
 * \brief Compares two resources' identifiers
 */

/*!
 * \class Utils::Configuration::Version
 * \brief Describes resource version
 *
 * Resource version is specified simply as a string of printable characters.
 * Two Version instances associated with a resource are compared by comparing
 * their spec() strings using the default compare function or the compare
 * function registered for the particular resource.
 *
 * \see Configuration::defaultVersionCompareFunction(),
 * Configuration::registerVersionCompareFunction()
 */

/*!
 * \fn Configuration::Version::Version()
 * \brief Contructs a null version
 */

/*!
 * \fn Configuration::Version::Version(const char *spec)
 * \brief Contructs from the given specification string
 */

/*!
 * \fn Configuration::Version::Version(const QLatin1String &spec)
 * \brief Contructs from the given specification string
 */

/*!
 * \fn Configuration::Version::Version(const QString &spec)
 * \brief Contructs from the given specification string
 */

/*!
 * \fn Configuration::Version::spec() const
 * \brief Returns the specification string passed on construction
 */

/*!
 * \fn Configuration::Version::isNull() const
 * \brief Null instances are constructed using the default constructor.
 */
