/*****************************************************************************
 * QtVlc - C++ bindings for libVLC using Qt awesomeness
 * Copyright (C) 2013 Orochimarufan <orochimarufan.x3@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef QTVLC_COMMON_P_H
#define QTVLC_COMMON_P_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QAtomicInt>

class VlcWrapperImpl : public QObject
{
    Q_OBJECT

    QAtomicInt ref;

public:
    VlcWrapperImpl() : QObject()
    {
        ref = 1;
    }

    inline void retain()
    {
        ref.ref();
    }

    inline bool release()
    {
        if (!ref.deref())
            delete this;
    }
};

#define VLC_WRAPPER_IMPL_PRIVATE(T, S) \
    S * d; \
    static QHash<S *, T *> instances; \
    T(S *);

#define VLC_WRAPPER_IMPL_PUBLIC(T, LIBVLC) \
    static T *instance(LIBVLC *); \
    virtual ~ T (); \
    LIBVLC *data();

#define VLC_WRAPPER_IMPL_CPP_C(T, LIBVLC) \
    T * T :: instance(LIBVLC *ptr) { \
        if (ptr == nullptr) return nullptr; \
        T * inst; \
        if (instances.contains(ptr)) { \
            inst = instances[ptr]; \
            inst->retain(); \
        } else \
            inst = new T(ptr); \
        return inst; \
    } \
    LIBVLC * T :: data() { \
        return d; \
    } \
    QHash<LIBVLC *, T *> T :: instances;

#define VLC_WRAPPER_IMPL_INIT() \
    instances[d] = this;

#define VLC_WRAPPER_IMPL_EXIT() \
    instances.remove(d);

#define VLC_WRAPPER_IMPL_INST(T, LIBVLC, LIBVLC_PREFIX) \
    T :: T (LIBVLC *ptr) : \
        VlcWrapperImpl(), d(ptr) \
    { \
        LIBVLC_PREFIX ## _retain(d); \
        VLC_WRAPPER_IMPL_INIT() \
    } \
    T :: ~ T () { \
/*        qDebug("delete " #T); */\
        VLC_WRAPPER_IMPL_EXIT() \
        LIBVLC_PREFIX ## _release(d); \
    }

#define VLC_WRAPPER_IMPL_INST2(T, LIBVLC, LIBVLC_PREFIX, INIT_FUNC, EXIT_FUNC) \
    T :: T (LIBVLC *ptr) : \
        VlcWrapperImpl(), d(ptr) \
    { \
        LIBVLC_PREFIX ## _retain(d); \
        VLC_WRAPPER_IMPL_INIT() \
        INIT_FUNC (); \
    } \
    T :: ~ T () { \
        qDebug("delete " #T); \
        VLC_WRAPPER_IMPL_EXIT() \
        EXIT_FUNC (); \
        LIBVLC_PREFIX ## _release(d); \
    }

#define VLC_WRAPPER_IMPL_CPP(T, LIBVLC, LIBVLC_PREFIX) \
    VLC_WRAPPER_IMPL_CPP_C(T, LIBVLC) \
    VLC_WRAPPER_IMPL_INST(T, LIBVLC, LIBVLC_PREFIX)

#define VLC_WRAPPER_IMPL_CPP2(T, LIBVLC, LIBVLC_PREFIX, INIT_FUNC, EXIT_FUNC) \
    VLC_WRAPPER_IMPL_CPP_C(T, LIBVLC) \
    VLC_WRAPPER_IMPL_INST2(T, LIBVLC, LIBVLC_PREFIX, INIT_FUNC, EXIT_FUNC)

// cannot call members on const references
template <class T>
inline T *getref(const T &in)
{
    return (T *)&in;
}

#endif // QTVLC_COMMON_P_H
