/*
 * Remoteroid - A remote control solution for Android platform, including handy file transfer and notify-to-PC.
 * Copyright (C) 2012 Taeho Kim(jyte82@gmail.com), Hyomin Oh(ohmnia1112@gmail.com), Hongkyun Kim(godgjdgjd@nate.com), Yongwan Hwang(singerhwang@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef INPUT_H_
#define INPUT_H_

#include "include/LogHelper.h"
#include <linux/input.h>
#include "include/suinput.h"

#define BUS_VIRTUAL 0x06
#define LOGTAG "RemoteroidInput"

/**
 * Open input device using suinput.
 * It will change /dev/uinput's permission to 666 first to read/write event from app, then attempt to open device. set true, uses su -c to change /dev/uinput permission to 666.
 * @return true if open succeeds, false otherwise.
 */
bool openInput(const int scrWidth, const int scrHeight);

/**
 * Open input device using suinput, without setting permission 666 to /dev/uinput.
 * If user has su binary that doesn't supports 'su -c' option, which enables running shell command with root permission,
 * Change permission through org.secmem.remoteroie.util.ComandLine.execAsRoot() first, then use this command to open device.
 */
bool openInputWithoutPermission(const int scrWidth, const int scrHeight);

/**
 * Close input device.
 */
void closeInput();

/**
 * Close input device, without reverting back /dev/uinput's permission to 660.
 */
void closeInputWithoutRevertPermission();

//void sendNativeEvent(const char* dev, int type, int code, int value);

/**
 * A simple wrapper for suinput_write()
 */
int sendNativeEvent(int uinput_fd, uint16_t type, uint16_t code, int32_t value);

#endif
