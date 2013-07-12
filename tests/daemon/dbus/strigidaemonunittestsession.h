/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2008 Jos van den Oever <jos@vandenoever.info> 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef STRIGIDAEMONUNITTESTSESSION_H
#define STRIGIDAEMONUNITTESTSESSION_H

#include <string>

class StrigiDaemonUnitTestSession {
private:
    class Private;
    Private* const p;
public:
    StrigiDaemonUnitTestSession();
    ~StrigiDaemonUnitTestSession();
    void setStrigiDaemon(const char* strigidaemon);
    void setStrigiDir(const char* dir);
    void setDataDir(const char* dir);
    void setXDGDir(const char* xdgdir);
    void addBackend(const char* name, const char* plugindir);
    void addFile(const char* name, const std::string& content);
    void addFile(const char* name, const char* content, int contentlength);
    void start();
    void stop();
};

#endif
