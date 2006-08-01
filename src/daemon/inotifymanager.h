/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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
#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <string>
#include <map>
#include <set>
#include <pthread.h>

class InotifyEventQueue;

namespace jstreams
{
    class IndexReader;
}

class InotifyManager
{
    public:
        InotifyManager();

        ~InotifyManager();
    
        bool init();
        
        void addWatch (const std::string path);
        void addWatches (const std::set<std::string> &watches);
        void setIndexedDirectories (const std::set<std::string> &dirs);
        
        void setInotifyEventQueue (InotifyEventQueue* eventQueue) { m_eventQueue = eventQueue; }
        
        void setIndexReader (jstreams::IndexReader* ireader) { m_pIndexReader = ireader;}
        
        bool start();
        void* run(void*);
        void stop();
        
        static bool ignoreFileCallback(const char* path, uint dirlen, uint len, time_t mtime);
        static bool indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime);
        static void watchDirCallback(const char* path);
        
        friend void* InotifyManagerStart (void* inotifyManager);
        
    private:
        enum State {Idling, Watching, Stopping};
        State m_state;
        
        std::string eventToString(int events);
        bool isRelevant (struct inotify_event * event);
        void watch ();
        
        InotifyEventQueue* m_eventQueue;
        int m_iInotifyFD;
        int m_iEvents;
        pthread_t m_thread;
        std::map<unsigned int, std::string> m_watches;
        bool m_bMonitor;
        bool m_bInitialized;
        std::set<std::string> m_toIndex;
        jstreams::IndexReader* m_pIndexReader;
};

#endif
