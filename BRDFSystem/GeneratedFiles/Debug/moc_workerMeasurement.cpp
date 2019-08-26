/****************************************************************************
** Meta object code from reading C++ file 'workerMeasurement.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../workerMeasurement.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'workerMeasurement.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WorkerMeasurement_t {
    QByteArrayData data[11];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorkerMeasurement_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorkerMeasurement_t qt_meta_stringdata_WorkerMeasurement = {
    {
QT_MOC_LITERAL(0, 0, 17), // "WorkerMeasurement"
QT_MOC_LITERAL(1, 18, 4), // "done"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 12), // "readyForGrab"
QT_MOC_LITERAL(4, 37, 3), // "sID"
QT_MOC_LITERAL(5, 41, 3), // "iID"
QT_MOC_LITERAL(6, 45, 9), // "CheckDone"
QT_MOC_LITERAL(7, 55, 8), // "workerID"
QT_MOC_LITERAL(8, 64, 10), // "StartTimer"
QT_MOC_LITERAL(9, 75, 11), // "measureFlag"
QT_MOC_LITERAL(10, 87, 11) // "CloseWorker"

    },
    "WorkerMeasurement\0done\0\0readyForGrab\0"
    "sID\0iID\0CheckDone\0workerID\0StartTimer\0"
    "measureFlag\0CloseWorker"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorkerMeasurement[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    2,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   45,    2, 0x08 /* Private */,
       8,    1,   48,    2, 0x08 /* Private */,
      10,    0,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,

       0        // eod
};

void WorkerMeasurement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WorkerMeasurement *_t = static_cast<WorkerMeasurement *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->done(); break;
        case 1: _t->readyForGrab((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->CheckDone((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->StartTimer((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->CloseWorker(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WorkerMeasurement::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WorkerMeasurement::done)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WorkerMeasurement::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WorkerMeasurement::readyForGrab)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject WorkerMeasurement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WorkerMeasurement.data,
      qt_meta_data_WorkerMeasurement,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WorkerMeasurement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkerMeasurement::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WorkerMeasurement.stringdata0))
        return static_cast<void*>(const_cast< WorkerMeasurement*>(this));
    return QObject::qt_metacast(_clname);
}

int WorkerMeasurement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void WorkerMeasurement::done()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void WorkerMeasurement::readyForGrab(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
