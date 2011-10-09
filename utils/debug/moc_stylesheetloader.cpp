/****************************************************************************
** Meta object code from reading C++ file 'stylesheetloader.h'
**
** Created: Sun Oct 9 13:10:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../stylesheetloader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stylesheetloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__StyleSheetLoader[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   25,   24,   24, 0x0a,
      44,   24,   24,   24, 0x0a,
      53,   24,   24,   24, 0x0a,
      62,   24,   24,   24, 0x08,
      89,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Utils__StyleSheetLoader[] = {
    "Utils::StyleSheetLoader\0\0name\0"
    "load(QString)\0unload()\0reload()\0"
    "startWatcherOnActiveFile()\0stopWatcher()\0"
};

const QMetaObject Utils::StyleSheetLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Utils__StyleSheetLoader,
      qt_meta_data_Utils__StyleSheetLoader, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Utils::StyleSheetLoader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Utils::StyleSheetLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Utils::StyleSheetLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__StyleSheetLoader))
        return static_cast<void*>(const_cast< StyleSheetLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int Utils::StyleSheetLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: load((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: unload(); break;
        case 2: reload(); break;
        case 3: startWatcherOnActiveFile(); break;
        case 4: stopWatcher(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
