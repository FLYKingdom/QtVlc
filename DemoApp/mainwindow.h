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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <VlcMedia.h>
#include <VlcMediaPlayer.h>
#include <VlcMediaPlayerAudio.h>
#include <VlcMediaPlayerVideo.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    VlcMediaPlayer player;
    VlcMediaPlayerAudio audio;
    VlcMedia media;

public slots:
    void on_actionOpen_triggered();
    void on_position_sliderMoved(int);
    void on_volume_volumeChanged(int);
    void on_volume_muteChanged(bool);
    void on_playButton_clicked();

    void onStateChanged(const VlcState::Type &);
    void onPositionChanged(const float &);
    void onMediaChanged(libvlc_media_t *);
};

#endif // MAINWINDOW_H
