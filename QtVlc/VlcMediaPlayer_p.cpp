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

#include "VlcMediaPlayer_p.h"

VLC_WRAPPER_IMPL_CPP2(VlcMediaPlayerPrivate, libvlc_media_player_t,
                      libvlc_media_player, attach_events, detach_events)

VlcMediaPlayerPrivate::VlcMediaPlayerPrivate(libvlc_instance_t *instance) :
    VlcWrapperImpl(), p_delegate(nullptr), widget(0)
{
    d = libvlc_media_player_new(instance);

    VLC_WRAPPER_IMPL_INIT();

    attach_events();
}


// media
libvlc_media_t *VlcMediaPlayerPrivate::media()
{
    return libvlc_media_player_get_media(d);
}

void VlcMediaPlayerPrivate::setMedia(libvlc_media_t *media)
{
    libvlc_media_player_set_media(d, media);
}

// widget
WId VlcMediaPlayerPrivate::videoWidget()
{
#if defined(Q_OS_WIN32)
    return (WId) libvlc_media_player_get_hwnd(d);
#elif defined(Q_OS_MAC)
    return libvlc_media_player_get_nsobject(d);
#elif defined(Q_OS_UNIX)
    return libvlc_media_player_get_xwindow(d);
#else
    qDebug("Uknown platform!!!");
    return 0;
#endif
}

void VlcMediaPlayerPrivate::setVideoWidget(WId widget)
{
    if (p_delegate == nullptr)
        this->widget = widget;
}

inline void VlcMediaPlayerPrivate::_setVideoWidget(WId widget)
{
#if defined(Q_OS_WIN32)
    libvlc_media_player_set_hwnd(d, (void *) widget);
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(d, widget);
#elif defined(Q_OS_UNIX)
    libvlc_media_player_set_xwindow(d, widget);
#else
    qDebug("Unknown Platform!!!");
#endif
}

void VlcMediaPlayerPrivate::setVideoDelegate(IVlcVideoDelegate *delegate)
{
    p_delegate = delegate;
}

IVlcVideoDelegate *VlcMediaPlayerPrivate::videoDelegate()
{
    return p_delegate;
}

// position
qint64 VlcMediaPlayerPrivate::length() const
{
    return libvlc_media_player_get_length(d);
}

qint64 VlcMediaPlayerPrivate::time() const
{
    return libvlc_media_player_get_time(d);
}

void VlcMediaPlayerPrivate::setTime(const qint64 &time)
{
    libvlc_media_player_set_time(d, time);
}

float VlcMediaPlayerPrivate::position() const
{
    return libvlc_media_player_get_position(d);
}

void VlcMediaPlayerPrivate::setPosition(const float &pos)
{
    libvlc_media_player_set_position(d, pos);
}


// events
void VlcMediaPlayerPrivate::event_cb(const libvlc_event_t *e, void *o)
{
    VlcMediaPlayerPrivate *p = static_cast<VlcMediaPlayerPrivate *>(o);

    emit p->libvlcEvent(e);

    switch(e->type)
    {
    case libvlc_MediaPlayerMediaChanged:
        emit p->mediaChanged(e->u.media_player_media_changed.new_media);
        break;
    case libvlc_MediaPlayerNothingSpecial:
        emit p->stateChanged(VlcState::NothingSpecial);
        break;
    case libvlc_MediaPlayerOpening:
        emit p->stateChanged(VlcState::Opening);
        break;
    case libvlc_MediaPlayerBuffering:
        emit p->stateChanged(VlcState::Buffering);
        emit p->buffering(e->u.media_player_buffering.new_cache);
        break;
    case libvlc_MediaPlayerPlaying:
        emit p->stateChanged(VlcState::Playing);
        break;
    case libvlc_MediaPlayerPaused:
        emit p->stateChanged(VlcState::Paused);
        break;
    case libvlc_MediaPlayerStopped:
        emit p->stateChanged(VlcState::Stopped);
        break;
    case libvlc_MediaPlayerForward:
        emit p->forward();
        break;
    case libvlc_MediaPlayerBackward:
        emit p->backward();
        break;
    case libvlc_MediaPlayerEndReached:
        emit p->endReached();
        break;
    case libvlc_MediaPlayerEncounteredError:
        emit p->encounteredError();
        break;
    case libvlc_MediaPlayerTimeChanged:
        emit p->timeChanged(e->u.media_player_time_changed.new_time);
        break;
    case libvlc_MediaPlayerPositionChanged:
        emit p->positionChanged(e->u.media_player_position_changed.new_position);
        break;
    case libvlc_MediaPlayerSeekableChanged:
        emit p->seekableChanged(e->u.media_player_seekable_changed.new_seekable);
        break;
    case libvlc_MediaPlayerPausableChanged:
        emit p->pausableChanged(e->u.media_player_pausable_changed.new_pausable);
        break;
    case libvlc_MediaPlayerTitleChanged:
        emit p->titleChanged(e->u.media_player_title_changed.new_title);
        break;
    case libvlc_MediaPlayerSnapshotTaken:
        emit p->snapshotTaken(e->u.media_player_snapshot_taken.psz_filename);
        break;
    case libvlc_MediaPlayerLengthChanged:
        emit p->lengthChanged(e->u.media_player_length_changed.new_length);
        break;
    case libvlc_MediaPlayerVout:
        emit p->voutChanged(e->u.media_player_vout.new_count);
        break;
    default:
        qDebug("VlcMediaPlayer: unknown Event: %i (%s)", e->type, libvlc_event_type_name(e->type));
    }
}

void VlcMediaPlayerPrivate::attach_events()
{
    p_eventmanager = libvlc_media_player_event_manager(d);

#define ATTACH(event_type) libvlc_event_attach(p_eventmanager, event_type, &VlcMediaPlayerPrivate::event_cb, this)
    ATTACH(libvlc_MediaPlayerMediaChanged);
    ATTACH(libvlc_MediaPlayerNothingSpecial);
    ATTACH(libvlc_MediaPlayerOpening);
    ATTACH(libvlc_MediaPlayerBuffering);
    ATTACH(libvlc_MediaPlayerPlaying);
    ATTACH(libvlc_MediaPlayerPaused);
    ATTACH(libvlc_MediaPlayerStopped);
    ATTACH(libvlc_MediaPlayerForward);
    ATTACH(libvlc_MediaPlayerBackward);
    ATTACH(libvlc_MediaPlayerEndReached);
    ATTACH(libvlc_MediaPlayerEncounteredError);
    ATTACH(libvlc_MediaPlayerTimeChanged);
    ATTACH(libvlc_MediaPlayerPositionChanged);
    ATTACH(libvlc_MediaPlayerSeekableChanged);
    ATTACH(libvlc_MediaPlayerPausableChanged);
    ATTACH(libvlc_MediaPlayerTitleChanged);
    ATTACH(libvlc_MediaPlayerSnapshotTaken);
    ATTACH(libvlc_MediaPlayerLengthChanged);
    ATTACH(libvlc_MediaPlayerVout);
#undef ATTACH
}

void VlcMediaPlayerPrivate::detach_events()
{
#define DETACH(event_type) libvlc_event_detach(p_eventmanager, event_type, &VlcMediaPlayerPrivate::event_cb, this)
    DETACH(libvlc_MediaPlayerMediaChanged);
    DETACH(libvlc_MediaPlayerNothingSpecial);
    DETACH(libvlc_MediaPlayerOpening);
    DETACH(libvlc_MediaPlayerBuffering);
    DETACH(libvlc_MediaPlayerPlaying);
    DETACH(libvlc_MediaPlayerPaused);
    DETACH(libvlc_MediaPlayerStopped);
    DETACH(libvlc_MediaPlayerForward);
    DETACH(libvlc_MediaPlayerBackward);
    DETACH(libvlc_MediaPlayerEndReached);
    DETACH(libvlc_MediaPlayerEncounteredError);
    DETACH(libvlc_MediaPlayerTimeChanged);
    DETACH(libvlc_MediaPlayerPositionChanged);
    DETACH(libvlc_MediaPlayerSeekableChanged);
    DETACH(libvlc_MediaPlayerPausableChanged);
    DETACH(libvlc_MediaPlayerTitleChanged);
    DETACH(libvlc_MediaPlayerSnapshotTaken);
    DETACH(libvlc_MediaPlayerLengthChanged);
    DETACH(libvlc_MediaPlayerVout);
#undef DETACH
}


// slots
void VlcMediaPlayerPrivate::play()
{
    if (!d) return;

    if (p_delegate)
        widget = p_delegate->request(); // TODO gather size

    _setVideoWidget(widget);

    libvlc_media_player_play(d);
}

void VlcMediaPlayerPrivate::setPause(bool pause)
{
    if (!d) return;

    if (libvlc_media_player_can_pause(d))
        libvlc_media_player_set_pause(d, pause);
}

void VlcMediaPlayerPrivate::pause()
{
    if (!d) return;

    if (libvlc_media_player_can_pause(d))
        libvlc_media_player_pause(d);
}

void VlcMediaPlayerPrivate::stop()
{
    if (!d) return;

    libvlc_media_player_stop(d);

    if (p_delegate)
    {
        p_delegate ->release();
        widget = 0;
        _setVideoWidget(0);
    }
}

bool VlcMediaPlayerPrivate::isPausable() const
{
    if (!d) return false;

    return libvlc_media_player_can_pause(d);
}

bool VlcMediaPlayerPrivate::isSeekable() const
{
    if (!d) return false;

    return libvlc_media_player_is_seekable(d);
}

VlcState::Type VlcMediaPlayerPrivate::state()
{
    if (d)
        return static_cast<VlcState::Type>(libvlc_media_player_get_state(d));
    else
        return VlcState::Error;
}

// Audio
int VlcMediaPlayerPrivate::audio_volume() const
{
    if (d)
        return libvlc_audio_get_volume(d);
    else
        return -1;
}
void VlcMediaPlayerPrivate::audio_setVolume(const int &volume)
{
    if (d)
        libvlc_audio_set_volume(d, volume);
}


bool VlcMediaPlayerPrivate::audio_isMuted() const
{
    if (d)
        return libvlc_audio_get_mute(d);
    else
        return false;
}

void VlcMediaPlayerPrivate::audio_setMuted(const int &mute)
{
    if (d)
        libvlc_audio_set_mute(d, mute);
}

void VlcMediaPlayerPrivate::audio_toggleMuted()
{
    if (d)
        libvlc_audio_toggle_mute(d);
}


int VlcMediaPlayerPrivate::audio_track() const
{
    if (d)
        return libvlc_audio_get_track(d);
    else
        return -1;
}

int VlcMediaPlayerPrivate::audio_trackCount() const
{
    if (d)
        return libvlc_audio_get_track_count(d);
    else
        return -1;
}

QHash<int, QString> VlcMediaPlayerPrivate::audio_trackDescription() const
{
    QHash<int, QString> descriptions;

    if (d)
    {
        libvlc_track_description_t *desc = libvlc_audio_get_track_description(d);
        descriptions = track_description(desc);
    }

    return descriptions;
}

void VlcMediaPlayerPrivate::audio_setTrack(const int &track)
{
    if (d)
        libvlc_audio_set_track(d, track);
}

// video
void VlcMediaPlayerPrivate::video_setKeyInput(bool on)
{
    libvlc_video_set_key_input(d, on);
}

void VlcMediaPlayerPrivate::video_setMouseInput(bool on)
{
    libvlc_video_set_mouse_input(d, on);
}

bool VlcMediaPlayerPrivate::video_getSize(unsigned num, unsigned *x, unsigned *y) const
{
    return libvlc_video_get_size(d, num, x, y) == 0;
}

bool VlcMediaPlayerPrivate::video_getCursor(unsigned num, int *x, int *y) const
{
    return libvlc_video_get_cursor(d, num, x, y) == 0;
}

float VlcMediaPlayerPrivate::video_scale() const
{
    return libvlc_video_get_scale(d);
}

void VlcMediaPlayerPrivate::video_setScale(float factor)
{
    libvlc_video_set_scale(d, factor);
}

QString VlcMediaPlayerPrivate::video_aspectRatio() const
{
    return qstring_and_free(libvlc_video_get_aspect_ratio(d));
}

void VlcMediaPlayerPrivate::video_setAspectRatio(const QString &ratio)
{
    libvlc_video_set_aspect_ratio(d, ratio.toUtf8().constData());
}

int VlcMediaPlayerPrivate::video_spu() const
{
    return libvlc_video_get_spu(d);
}

int VlcMediaPlayerPrivate::video_spuCount() const
{
    return libvlc_video_get_spu_count(d);
}

QHash<int, QString> VlcMediaPlayerPrivate::video_spuDescription() const
{
    return track_description(libvlc_video_get_spu_description(d));
}

bool VlcMediaPlayerPrivate::video_setSpu(int spu)
{
    return libvlc_video_set_spu(d, spu) == 0;
}

bool VlcMediaPlayerPrivate::video_setSubtitleFile(const QString &filename)
{
    return libvlc_video_set_subtitle_file(d, filename.toUtf8().constData());
}

qint64 VlcMediaPlayerPrivate::video_spuDelay() const
{
    return libvlc_video_get_spu_delay(d);
}

bool VlcMediaPlayerPrivate::video_setSpuDelay(qint64 delay)
{
    return libvlc_video_set_spu_delay(d, delay) == 0;
}

QHash<int, QString> VlcMediaPlayerPrivate::video_titleDescription() const
{
    return track_description(libvlc_video_get_title_description(d));
}

QHash<int, QString> VlcMediaPlayerPrivate::video_chapterDescription(int title) const
{
    return track_description(libvlc_video_get_chapter_description(d, title));
}

QString VlcMediaPlayerPrivate::video_cropGeometry() const
{
    return QString::fromUtf8(libvlc_video_get_crop_geometry(d));
}

void VlcMediaPlayerPrivate::video_setCropGeometry(const QString &geometry)
{
    libvlc_video_set_crop_geometry(d, geometry.toUtf8().constData());
}

int VlcMediaPlayerPrivate::video_teletext() const
{
    return libvlc_video_get_teletext(d);
}

void VlcMediaPlayerPrivate::video_setTeletext(int page)
{
    libvlc_video_set_teletext(d, page);
}

void VlcMediaPlayerPrivate::video_toggleTeletext()
{
    libvlc_toggle_teletext(d);
}

int VlcMediaPlayerPrivate::video_trackCount() const
{
    return libvlc_video_get_track_count(d);
}

int VlcMediaPlayerPrivate::video_track() const
{
    return libvlc_video_get_track(d);
}

QHash<int, QString> VlcMediaPlayerPrivate::video_trackDescription() const
{
    return track_description(libvlc_video_get_track_description(d));
}

bool VlcMediaPlayerPrivate::video_setTrack(int track)
{
    return libvlc_video_set_track(d, track) == 0;
}

bool VlcMediaPlayerPrivate::video_takeSnapshot(unsigned num, const QString &filename, unsigned int width, unsigned int height) const
{
    return libvlc_video_take_snapshot(d, num, filename.toUtf8().constData(), width, height) == 0;
}

void VlcMediaPlayerPrivate::video_setDeinterlace(const QString &mode)
{
    libvlc_video_set_deinterlace(d, mode.toUtf8().constData());
}

int VlcMediaPlayerPrivate::video_marqueeInt(unsigned option) const
{
    return libvlc_video_get_marquee_int(d, option);
}

QString VlcMediaPlayerPrivate::video_marqueeString(unsigned option) const
{
    return QString::fromUtf8(libvlc_video_get_marquee_string(d, option));
}

void VlcMediaPlayerPrivate::video_setMarqueeInt(unsigned option, int value)
{
    libvlc_video_set_marquee_int(d, option, value);
}

void VlcMediaPlayerPrivate::video_setMarqueeString(unsigned option, const QString &text)
{
    libvlc_video_set_marquee_string(d, option, text.toUtf8().constData());
}

int VlcMediaPlayerPrivate::video_logoInt(unsigned option) const
{
    return libvlc_video_get_logo_int(d, option);
}

void VlcMediaPlayerPrivate::video_setLogoInt(unsigned option, int value)
{
    libvlc_video_set_logo_int(d, option, value);
}

void VlcMediaPlayerPrivate::video_setLogoString(unsigned option, const QString &value)
{
    libvlc_video_set_logo_string(d, option, value.toUtf8().constData());
}

int VlcMediaPlayerPrivate::video_adjustInt(unsigned option) const
{
    return libvlc_video_get_adjust_int(d, option);
}

float VlcMediaPlayerPrivate::video_adjustFloat(unsigned option) const
{
    return libvlc_video_get_adjust_float(d, option);
}

void VlcMediaPlayerPrivate::video_setAdjustInt(unsigned option, int value)
{
    libvlc_video_set_adjust_int(d, option, value);
}

void VlcMediaPlayerPrivate::video_setAdjustFloat(unsigned option, float value)
{
    libvlc_video_set_adjust_float(d, option, value);
}
