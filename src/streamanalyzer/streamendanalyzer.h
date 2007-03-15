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
#ifndef STREAMENDANALYZER_H
#define STREAMENDANALYZER_H

#include "inputstream.h"
#include "cnstr.h"

namespace Strigi {
class StreamAnalyzer;
class AnalysisResult;
class RegisteredField;
class FieldRegister;

class StreamEndAnalyzer {
protected:
    std::string error;
    static char testStream(jstreams::InputStream *in);
public:
    virtual ~StreamEndAnalyzer() {};
    virtual bool checkHeader(const char* header, int32_t headersize) const = 0;
    virtual char analyze(Strigi::AnalysisResult& idx, jstreams::InputStream* in)=0;
    const std::string& getError() const { return error; }
    virtual const char* getName() const = 0;
};

class StreamEndAnalyzerFactory {
public:
    virtual ~StreamEndAnalyzerFactory(){}
    virtual const char* getName() const = 0;
    virtual void registerFields(Strigi::FieldRegister&) = 0;
    virtual StreamEndAnalyzer* newInstance() const = 0;
    virtual bool analyzesSubStreams() const { return false; }
};


}

#endif