/****************************************************************************
** Meta object code from reading C++ file 'filesystemwatcher.h'
**
** Created: Thu Sep 8 19:42:39 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../filesystemwatcher.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filesystemwatcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__FileSystemWatcher[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   26,   25,   25, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Utils__FileSystemWatcher[] = {
    "Utils::FileSystemWatcher\0\0path\0"
    "pathChanged(QString)\0"
};

const QMetaObject Utils::FileSystemWatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Utils__FileSystemWatcher,
      qt_meta_data_Utils__FileSystemWatcher, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Utils::FileSystemWatcher::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Utils::FileSystemWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Utils::FileSystemWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__FileSystemWatcher))
        return static_cast<void*>(const_cast< FileSystemWatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int Utils::FileSystemWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pathChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Utils::FileSystemWatcher::pathChanged(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< Utils::FileSystemWatcher *>(this), &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
