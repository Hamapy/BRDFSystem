/****************************************************************************
** Meta object code from reading C++ file 'workerCCD.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../workerCCD.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'workerCCD.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WorkerCCD_t {
    QByteArrayData data[9];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorkerCCD_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorkerCCD_t qt_meta_stringdata_WorkerCCD = {
    {
QT_MOC_LITERAL(0, 0, 9), // "WorkerCCD"
QT_MOC_LITERAL(1, 10, 10), // "sendingMat"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 8), // "workerID"
QT_MOC_LITERAL(4, 31, 3), // "mat"
QT_MOC_LITERAL(5, 35, 10), // "sendingImg"
QT_MOC_LITERAL(6, 46, 3), // "img"
QT_MOC_LITERAL(7, 50, 10), // "StartTimer"
QT_MOC_LITERAL(8, 61, 15) // "SetExposureTime"

    },
    "WorkerCCD\0sendingMat\0\0workerID\0mat\0"
    "sendingImg\0img\0StartTimer\0SetExposureTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorkerCCD[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    2,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   44,    2, 0x08 /* Private */,
       8,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,    3,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void WorkerCCD::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WorkerCCD *_t = static_cast<WorkerCCD *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendingMat((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        case 1: _t->sendingImg((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        case 2: _t->StartTimer(); break;
        case 3: _t->SetExposureTime(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WorkerCCD::*_t)(int , QImage );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WorkerCCD::sendingMat)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WorkerCCD::*_t)(int , QImage );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WorkerCCD::sendingImg)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject WorkerCCD::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WorkerCCD.data,
      qt_meta_data_WorkerCCD,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WorkerCCD::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkerCCD::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WorkerCCD.stringdata0))
        return static_cast<void*>(const_cast< WorkerCCD*>(this));
    return QObject::qt_metacast(_clname);
}

int WorkerCCD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void WorkerCCD::sendingMat(int _t1, QImage _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WorkerCCD::sendingImg(int _t1, QImage _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
