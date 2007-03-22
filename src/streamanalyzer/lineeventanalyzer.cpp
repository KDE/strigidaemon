/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "lineeventanalyzer.h"
#include "streamlineanalyzer.h"
#include "textutils.h"
#include "string.h"
using namespace Strigi;
using namespace jstreams;
using namespace std;

LineEventAnalyzer::LineEventAnalyzer(vector<StreamLineAnalyzer*>& l)
        :line(l), ready(true) {
}
LineEventAnalyzer::~LineEventAnalyzer() {
    vector<StreamLineAnalyzer*>::iterator l;
    for (l = line.begin(); l != line.end(); ++l) {
        delete *l;
    }
}
void
LineEventAnalyzer::startAnalysis(AnalysisResult* r) {
    result = r;
    ready = line.size() == 0;
    missingBytes = 0;
    lineBuffer.assign("");
    byteBuffer.assign("");
}
void
LineEventAnalyzer::endAnalysis() {
    vector<StreamLineAnalyzer*>::iterator l;
    for (l = line.begin(); l != line.end(); ++l) {
        (*l)->endAnalysis();
    }
}
void
LineEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (ready) return;

    // if we have missing characters, handle them
    if (missingBytes) {
        if (length > (unsigned char)missingBytes) {
            // we have enough data to finish the character
            byteBuffer.append(data, missingBytes);
            if (!checkUtf8(byteBuffer)) {
                ready = true;
                return;
            }
            lineBuffer.append(byteBuffer);
            data += missingBytes;
            length += missingBytes;
            // clean up the byte buffer
            byteBuffer.assign("");
            missingBytes = 0;
        } else {
            byteBuffer.append(data, length);
            missingBytes -= length;
            return;
        }
    }

    // validate the utf8
    const char* p = checkUtf8(byteBuffer.c_str(), byteBuffer.length(),
        missingBytes);
    if (p || missingBytes == 0) {
        // not valid
        ready = true;
        return;
    }

    // find the first \n
    p = data;
    const char* end = data + length;
    do {
        if (*p == '\n') break; 
    } while (++p != end);

    if (p == end) { // no '\n' was found, we put this in the buffer
        lineBuffer.append(data, length);
        return;
    }
    lineBuffer.append(data, p-data);

    // if we have an data in the linebuffer, add the rest of the line to it
    // validate the text
    //if (
    if (initialized) {
    } else {
    }
//    fprintf(stderr, "another line\n");

    // check if we are done
    bool more = false;
    vector<StreamLineAnalyzer*>::iterator i;
    for (i = line.begin(); i != line.end(); ++i) {
        more = more || !(*i)->isReadyWithStream();
    }
    ready = !more;
}
bool
LineEventAnalyzer::isReadyWithStream() {
    return ready;
}
