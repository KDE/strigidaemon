/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#ifndef DBUSMESSAGEWRITER_H
#define DBUSMESSAGEWRITER_H

#include <string>
#include <set>
#include <map>
#include <vector>

#include <dbus/dbus.h>

class DBusMessageWriter {
    DBusConnection* const conn;
    DBusMessage* const reply;
    DBusMessage* error;
public:
    DBusMessageIter it;

    DBusMessageWriter(DBusConnection* conn, DBusMessage* msg);
    ~DBusMessageWriter();
    void setError(const std::string& error);
};

DBusMessageWriter&
operator<<(DBusMessageWriter&, const bool b);
DBusMessageWriter&
operator<<(DBusMessageWriter&, int32_t i);
DBusMessageWriter& operator<<(DBusMessageWriter&, int64_t i);
DBusMessageWriter& operator<<(DBusMessageWriter&, double d);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::string& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::set<std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<int32_t>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::map<std::string, std::string>& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::pair<bool, std::string> >& s);
//    DBusMessageWriter& operator<<(const ClientInterface::Hits& s);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::multimap<int, std::string>&);
DBusMessageWriter& operator<<(DBusMessageWriter&, const std::vector<std::pair<std::string, uint32_t> >&);
//    DBusMessageWriter& operator<<(const Variant& v);
//    DBusMessageWriter& operator<<(const std::vector<std::vector<Variant> >& v);
#endif