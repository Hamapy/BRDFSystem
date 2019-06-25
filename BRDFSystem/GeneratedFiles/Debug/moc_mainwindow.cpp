/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[308];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 10), // "startTimer"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 18), // "TurnToMeasurement1"
QT_MOC_LITERAL(4, 42, 18), // "TurnToMeasurement2"
QT_MOC_LITERAL(5, 61, 18), // "TurnToMeasurement3"
QT_MOC_LITERAL(6, 80, 15), // "TurnToModeling1"
QT_MOC_LITERAL(7, 96, 15), // "TurnToModeling2"
QT_MOC_LITERAL(8, 112, 14), // "TurnToSettings"
QT_MOC_LITERAL(9, 127, 10), // "TurnToTest"
QT_MOC_LITERAL(10, 138, 15), // "TurnToPreCamera"
QT_MOC_LITERAL(11, 154, 35), // "PushButton_StartMeasurement_P..."
QT_MOC_LITERAL(12, 190, 15), // "StopMeasurement"
QT_MOC_LITERAL(13, 206, 10), // "ConnectRGB"
QT_MOC_LITERAL(14, 217, 25), // "PushButton_IniCCD_Pressed"
QT_MOC_LITERAL(15, 243, 38), // "PushButton_CaptureContinuousl..."
QT_MOC_LITERAL(16, 282, 12), // "DisplayImage"
QT_MOC_LITERAL(17, 295, 8), // "workerID"
QT_MOC_LITERAL(18, 304, 3) // "img"

    },
    "MainWindow\0startTimer\0\0TurnToMeasurement1\0"
    "TurnToMeasurement2\0TurnToMeasurement3\0"
    "TurnToModeling1\0TurnToModeling2\0"
    "TurnToSettings\0TurnToTest\0TurnToPreCamera\0"
    "PushButton_StartMeasurement_Pressed\0"
    "StopMeasurement\0ConnectRGB\0"
    "PushButton_IniCCD_Pressed\0"
    "PushButton_CaptureContinuously_Pressed\0"
    "DisplayImage\0workerID\0img"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   90,    2, 0x08 /* Private */,
       4,    0,   91,    2, 0x08 /* Private */,
       5,    0,   92,    2, 0x08 /* Private */,
       6,    0,   93,    2, 0x08 /* Private */,
       7,    0,   94,    2, 0x08 /* Private */,
       8,    0,   95,    2, 0x08 /* Private */,
       9,    0,   96,    2, 0x08 /* Private */,
      10,    0,   97,    2, 0x08 /* Private */,
      11,    0,   98,    2, 0x08 /* Private */,
      12,    0,   99,    2, 0x08 /* Private */,
      13,    0,  100,    2, 0x08 /* Private */,
      14,    0,  101,    2, 0x08 /* Private */,
      15,    0,  102,    2, 0x08 /* Private */,
      16,    2,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,   17,   18,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startTimer(); break;
        case 1: _t->TurnToMeasurement1(); break;
        case 2: _t->TurnToMeasurement2(); break;
        case 3: _t->TurnToMeasurement3(); break;
        case 4: _t->TurnToModeling1(); break;
        case 5: _t->TurnToModeling2(); break;
        case 6: _t->TurnToSettings(); break;
        case 7: _t->TurnToTest(); break;
        case 8: _t->TurnToPreCamera(); break;
        case 9: _t->PushButton_StartMeasurement_Pressed(); break;
        case 10: _t->StopMeasurement(); break;
        case 11: _t->ConnectRGB(); break;
        case 12: _t->PushButton_IniCCD_Pressed(); break;
        case 13: _t->PushButton_CaptureContinuously_Pressed(); break;
        case 14: _t->DisplayImage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QImage(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::startTimer)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::startTimer()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
