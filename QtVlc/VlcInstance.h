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

#ifndef QTVLC_VLCINSTANCE_H
#define QTVLC_VLCINSTANCE_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "QtVlcConfig.h"

struct libvlc_instance_t;
class VlcInstancePrivate;

/**
 * @brief The VlcInstance class
 * The Entity controlling EVERYTHING ;)
 */
class QtVlc_EXPORT VlcInstance : public QObject
{
    Q_OBJECT
    VlcInstancePrivate *d;

public:
    /**
     * @brief Check if this VlcInstance is valid
     * True if this VlcInstance has underlying data.
     * Most methods will throw a NullPointer exception if called invalid objects.
     */
    bool isValid();

    // copy, assignment & libvlc primitive
    VlcInstance(const VlcInstance &);
    VlcInstance &operator =(const VlcInstance &);
    VlcInstance(libvlc_instance_t *);
    VlcInstance &operator =(libvlc_instance_t *);
    libvlc_instance_t *data(); // refcount is NOT increased!

    /**
     * @brief VlcInstance constructor
     * The global VlcInstance
     */
    VlcInstance();

    /**
     * @brief The raw global instance
     */
    static libvlc_instance_t *globalInstance();

    /**
     * @brief VlcInstance constructor
     * @param args the libvlc commandline arguments
     */
    explicit VlcInstance(const QStringList &args);

    // destructor
    virtual ~VlcInstance();

    /**
     * @brief Set the application name.
     * @param application Human-readable application name
     * @param version HTTP user agent
     */
    void setUserAgent(const QString &application, const QString &http);

    // version
    static QString libvlc_version();
    static QString libvlc_compiler();
    static QString libvlc_changeset();

    static QString QtVlc_version();
    static QString QtVlc_version_git();
    static QString QtVlc_build_libvlc_version();
    static QString QtVlc_build_qt_version();
};

#endif // QTVLC_VLCINSTANCE_H
