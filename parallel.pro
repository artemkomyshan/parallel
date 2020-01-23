include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -pthread

INCLUDEPATH += "/home/artem/Artem/googletest/googletest"
INCLUDEPATH += "/home/artem/Artem/boost_1_72_0"

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
