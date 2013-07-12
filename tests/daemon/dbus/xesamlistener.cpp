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
#include "xesamlistener.h"
#include "xesam/xesamdbus.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QTime>

XesamListener::XesamListener(OrgFreedesktopXesamSearchInterface* xesam)
        :eventloop(this) {
    connect(xesam, SIGNAL(HitsAdded(QString, uint)),
        this, SLOT(slotHitsAdded(QString, uint)));
    connect(xesam, SIGNAL(HitsRemoved(QString, QList<uint>)),
        this, SLOT(slotHitsRemoved(QString, QList<uint>)));
    connect(xesam, SIGNAL(SearchDone(QString)),
        this, SLOT(slotSearchDone(QString)));
    connect(xesam, SIGNAL(StateChanged(QStringList)),
        this, SLOT(slotStateChanged(QStringList)));
}
bool
XesamListener::waitForSearchToFinish(const QString& searchid,
        int millisecondtimeout) {
    QTime time;
    time.start();
    int left = millisecondtimeout;
    bool finished = finishedSearches.contains(searchid);
    while (left > 0 && !finished) {
        QTimer::singleShot(left, &eventloop, SLOT(quit()));
        eventloop.exec();
        left = millisecondtimeout - time.elapsed();
        finished = finishedSearches.contains(searchid);
    }
    return finished;
}
 uint
XesamListener::getNumberOfReportedHits(const QString& searchid) const {
    return hitsReported.value(searchid);
}
void
XesamListener::slotHitsAdded(const QString &search, uint count) {
    hitsReported[search] += count;
    eventloop.quit();
}
void
XesamListener::slotHitsModified(const QString &search,
        const QList<uint> &hit_ids) {
    eventloop.quit();
}
void
XesamListener::slotHitsRemoved(const QString &search,
        const QList<uint> &hit_ids) {
    eventloop.quit();
}
void
XesamListener::slotSearchDone(const QString &search) {
    finishedSearches.append(search);
    eventloop.quit();
}
void
XesamListener::slotStateChanged(const QStringList &state_info) {
    eventloop.quit();
}
