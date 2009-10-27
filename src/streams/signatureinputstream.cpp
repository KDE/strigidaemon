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
#include "signatureinputstream.h"
#include <strigi/strigiconfig.h>
#include "kmpsearcher.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace Strigi;

class SignatureInputStream::Private {
public:
    int32_t signaturesize;
    InputStream* input;
    std::string signature;

    Private(InputStream* i, int32_t s)
            : signaturesize(s), input(i) {
    }
};

SignatureInputStream::SignatureInputStream(InputStream* i,
        int32_t signaturesize) :p(new Private(i, signaturesize)) {
}
SignatureInputStream::~SignatureInputStream() {
    delete p;
}
int32_t
SignatureInputStream::read(const char*& start, int32_t min, int32_t max) {
    return p->input->read(start, min, max);
}
int64_t
SignatureInputStream::reset(int64_t newpos) {
    return p->input->reset(newpos);
}
int64_t
SignatureInputStream::skip(int64_t ntoskip) {
    return p->input->skip(ntoskip);
}
std::string
SignatureInputStream::signature() const {
    return p->signature;
}
