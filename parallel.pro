include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "C:\Program Files\boost\boost_1_68_0"

SOURCES += \
    test/main.cpp

HEADERS += \
    sequence.hpp \
    utility/sequence.hpp \
    utility/thread_raii.hpp \
    utility/std14.hpp \
    containers/queue.hpp \
    containers/thread_pool.hpp \
    draft/draft.hpp \
    raii/multi_lock.hpp \
    raii/scoped_thread.hpp \
    test/tst_parallel.hpp
