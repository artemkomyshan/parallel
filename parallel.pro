include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "C:\Program Files\boost\boost_1_68_0"

SOURCES += \
    test/main.cpp

HEADERS += \
    sequence.hpp \
    utility/sequence.hpp \
    utility/thread_raii.hpp \
    containers/queue.hpp \
    containers/thread_pool.hpp \
    raii/multi_lock.hpp \
    raii/scoped_thread.hpp \
    test/tst_parallel.hpp \
    assert.hpp \
    property.hpp \
    utility/not_null.hpp \
    utility/not_null.hpp \
    utility/property.hpp
