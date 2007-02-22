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
#include "jstreamsconfig.h"
#include "pdfparser.h"
#include "stringterminatedsubstream.h"
#include "gzipinputstream.h"
#include "subinputstream.h"
#include <ctype.h>
using namespace jstreams;
using namespace std;

int32_t streamcount = 0;

PdfParser::PdfParser() :streamhandler(0), texthandler(0) {
}

StreamStatus
PdfParser::read(int32_t min, int32_t max) {
    int32_t off = pos-start;
    int32_t d = stream->getPosition() - objdefstart;
    min += d;
    if (max > 0) max += d;
    stream->reset(objdefstart);
    int32_t n = stream->read(start, min, max);
//    printf("objstart %i %i\n", d, n);
    if (n < min) return stream->getStatus();
    pos = start + off;
    end = start + n;
    return Ok;
}
StreamStatus
PdfParser::read2(int32_t min, int32_t max) {
//    printf("pos %lli\n", stream->getPosition());
    int32_t n = stream->read(start, min, max);
    if (n < min) return stream->getStatus();
    pos = start;
    end = pos + n;
    return Ok;
}
StreamStatus
PdfParser::checkForData(int32_t m) {
    StreamStatus n = Ok;
    if (end - pos < m) {
        n = read(m, 0);
    }
//    fprintf(stderr, "checkForData %i\n", n);
    return n;
}

bool
PdfParser::isInString(char c, const char* s, int32_t n) {
    for (int i=0; i<n; ++i) {
        if (s[i] == c) return true;
    }
    return false;
}
StreamStatus
PdfParser::skipDigits() {
    StreamStatus s;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end && isdigit(*pos)) pos++;
    } while (pos == end);
    return Ok;
}
StreamStatus
PdfParser::skipXChars() {
    StreamStatus s;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end && isxdigit(*pos)) pos++;
    } while (pos == end);
    return Ok;
}
StreamStatus
PdfParser::skipFromString(const char*str, int32_t n) {
    StreamStatus s;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end && isInString(*pos, str, n)) pos++;
    } while (pos == end);
    return Ok;
}
StreamStatus
PdfParser::skipNotFromString(const char*str, int32_t n) {
    StreamStatus s;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end && !isInString(*pos, str, n)) pos++;
    } while (pos == end);
    return Ok;
}
StreamStatus
PdfParser::skipKeyword(const char* str, int32_t len) {
    if (end - pos < len) {
        StreamStatus s;
        if ((s = read(len, 0)) != Ok) {
            error.assign("Premature end of stream.");
            return Error;
        }
    }
//    printf("skipKeyword %s '%.*s'\n", str, (len>end-pos)?end-pos:len, pos);
    if (strncmp(pos, str, len) != 0) {
            error.assign("Keyword ");
            error.append(str, len);
            error.append(" not found.");
            return Error;
    }
    pos += len;
    return Ok;
}
/**
 * Skip whitespace in the stream. Return amount of whitespace skipped.
 * After calling this function the position in the stream is after the
 * whitespace. Skip characters from \t\n\f\r\t and space.
 **/
StreamStatus
PdfParser::skipWhitespace() {
    StreamStatus s;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end && isspace(*pos)) pos++;
    } while (pos == end);
    return Ok;
}
StreamStatus
PdfParser::parseComment() {
    if (*pos != '%') return Ok;
    pos++; // skip '%'
    return skipNotFromString("\r\n", 2);
}
StreamStatus
PdfParser::skipWhitespaceOrComment() {
//    printf("skipWhitespaceOrComment\n");
    int64_t o;
    int64_t no = pos - start;
    StreamStatus s;
    do {
        o = no;
        if ((s = skipWhitespace()) != Ok) return s;
        if ((s = parseComment()) != Ok) return s;
        no = pos - start;
    } while (o != no);
    return Ok;
}
StreamStatus
PdfParser::parseBoolean() {
    return (*pos == 't') ?skipKeyword("true", 4) :skipKeyword("false", 5);
}
StreamStatus
PdfParser::skipNumber() {
    char ch = *pos;
    if (ch == '+' || ch == '-') pos++;
    StreamStatus n = skipDigits();
    if (n != Ok) return n;
    if (pos < end && *pos == '.') {
        pos++;
        n = skipDigits();
    }
    return n;
}
// - number : [+-]?\d+(.\d+)?
StreamStatus
PdfParser::parseNumber() {
    int64_t p = pos - start;
    char ch = *pos;
    if (ch == '+' || ch == '-') pos++;
    StreamStatus n = skipDigits();
    if (n != Ok) return n;
    if (pos < end && *pos == '.') {
        pos++;
        n = skipDigits();
        const char *s = start + p;
        lastNumber = strtod(s, 0);
    } else {
        const char *s = start + p;
        lastNumber = strtol(s, 0, 10);
    }
    // parse a number
    lastObject = &lastNumber;
    if (lastNumber > 300 || lastNumber < -300) lastString.append(" ", 1);
    return n;
}
StreamStatus
PdfParser::parseNumberOrIndirectObject() {
//    printf("parseNumberOrIndirectObject\n");
    StreamStatus s = parseNumber();
    if (s != Ok) return s;
    s = skipWhitespace();
    if (s != Ok) return s;
    // now we must check if this is an indirect object
    if (isdigit(*pos)) {
        //const char*ss= start;
        int64_t p = pos - start;
        s = parseNumber();
        if (s != Ok) return s;
        s = skipWhitespace();
        if (s != Ok) return s;
        if (*pos == 'R') {
            pos++;
            lastObject = 0;
        } else {
            // set the position in front of the previous number
            // because it is a separate number and not part of a reference
            pos = start + p;
        }
    }
//    printf("<parseNumberOrIndirectObject\n");
    return Ok;
}
StreamStatus
PdfParser::parseLiteralString() {
    StreamStatus s;
    int par = 1;
    pos++;
    bool escape = false;
    do {
        if ((s = checkForData(1)) != Ok) return s;
        while (pos < end) {
            char c = *pos;
            if (escape) {
                escape = false;
            } else {
                if (c == ')') {
                    if (--par == 0) {
                        pos++;
                        return Ok;
                    }
                    lastString += c;
                } else if (c == '(') {
                    lastString += c;
                    par++;
                } else if (c == '\\') {
                    escape = true;
                } else if (isascii(*pos)) {
                    lastString += c;
                }
            }
            pos++;
        }
    } while (1);
    return Ok;
}
StreamStatus
PdfParser::parseHexString() {
    skipKeyword("<", 1);
//    fprintf(stderr, "parseHexString\n");
    if (skipXChars() != Ok) {
//         fprintf(stderr, "not a hex string\n");
         error.assign("invalid hexstring.");
         return Error;
    }
//    printf("parseHexString ok\n");
    return skipKeyword(">", 1);
}
StreamStatus
PdfParser::parseName() {
//    printf("parseName %.*s\n", (10>end-pos)?end-pos:10, pos);
    pos++;
    int64_t p = pos - start;
    StreamStatus r = skipNotFromString("()<>[]{}/%\t\n\f\r ", 16);
    if (r == Error) {
        error.assign(stream->getError());
        return r;
    }
    const char *s = start + p;
    lastName.assign(s, pos-s);
    lastObject = &lastName;
    return r;
}
StreamStatus
PdfParser::parseOperator() {
    int64_t p = pos - start;
    StreamStatus r = skipNotFromString("()<>[]{}/%\t\n\f\r ", 16);
    if (r == Error) {
        error.assign(stream->getError());
        return r;
    }
    const char *s = start + p;
    lastOperator.assign(s, pos-s);
    if (lastOperator == "TJ" || lastOperator == "Tj") {
        if (texthandler) {
            texthandler->handle(lastString);
        }
        lastString.resize(0);
    }
    lastObject = &lastOperator;
    return r;
}
StreamStatus
PdfParser::parseDictionaryOrStream() {
    enum Mode {None, Length, Filter, Type, First, N};
    Mode mode = None;
//    fprintf(stderr, "parseDictionary %p\n", this);
    StreamStatus r;
    pos += 2;
    skipWhitespaceOrComment();
    bool hasfilter = false;
    string filter;
    string type;
    int length = -1;
    int offset = 0;
    int numberofobjects = 0;
    while (*pos != '>') {
        if (parseName() != Ok) {
            error.assign("Expected a name.");
            return Error;
        }
        if (lastName == "Length") mode = Length;
        else if (lastName == "Filter") mode = Filter;
        else if (lastName == "Type") mode = Type;
        else if (lastName == "First") mode = First;
        else if (lastName == "N") mode = N;
        else mode = None;
        if (skipWhitespaceOrComment() != Ok) {
            error.assign("Error parsing whitespace in dictionary.");
            return Error;
        }
        lastObject = 0;
        if (parseObjectStreamObject() != Ok) {
            error.assign("Error parsing dictionary value.");
            return Error;
        }
        if (mode == Length && lastObject == &lastNumber) {
            length = (int32_t)lastNumber;
        } else if (mode == Filter) {
            hasfilter = true;
            if (lastObject == &lastName) {
                filter = lastName;
            }
        } else if (mode == Type && lastObject == &lastName) {
            type = lastName;
        } else if (mode == First && lastObject == &lastNumber) {
            offset = (int32_t)lastNumber;
        } else if (mode == N && lastObject == &lastNumber) {
            numberofobjects = (int32_t)lastNumber;
        }
        if (skipWhitespaceOrComment() != Ok) {
            error.assign("Error reading whitespace after dictionary value.");
            return Error;
        }
    }
    if (skipKeyword(">>", 2) != Ok) return Error;
    r = skipWhitespaceOrComment();
    if (r != Ok) return r;
    if (checkForData(6) == Ok && *pos == 's' && pos[2] == 'r') {
//        fprintf(stderr, "stream %i\n", end-pos);
        skipKeyword("stream", 6);
        if (checkForData(11) != Ok) return Error;
        if (*pos == '\r') pos++;
        if (*pos != '\n') return Error;
        pos++;

        // read stream until 'endstream'
        int64_t p = pos-start;
        if (p != stream->reset(p)) return Error;
//        fprintf(stderr, "filter: %s\n", filter.c_str());
//        fprintf(stderr, "type: %s %i\n", type.c_str(), streamcount);
//        printf("position: %lli length %i\n", p, length);
        if (length == -1) {
            StringTerminatedSubStream sub(stream, "endstream");
            if (handleSubStream(&sub, type, offset, numberofobjects, hasfilter,
                    filter) != Eof) {
                return Error;
            }
            p += sub.getSize();
        } else {
            SubInputStream sub(stream, length);
            if (handleSubStream(&sub, type, offset, numberofobjects, hasfilter,
                    filter) != Eof) {
                return Error;
            }
            p += sub.getSize();
        }
        if (p != stream->reset(p)) return Error;
        if (read(1, 0) != Ok) return Error;
        pos = start + p;
        //pos = start + (stream->getPosition()-objdefstart);
//        printf("hi %i\n", off+(pos-start));
        //printf("hi %i\n", *pos);
        if (skipWhitespaceOrComment() != Ok) return Error;
//        printf("hi %i %.*s\n", pos-start, 10, pos);
        if (skipKeyword("endstream", 9) != Ok) return Error;
//        printf("endstream\n");
        streamcount++;
    }
//    printf("endDictionary %p\n", this);
    return Ok;
}
StreamStatus
PdfParser::parseArray() {
    lastString.resize(0);
    pos++;
    if (skipWhitespaceOrComment() != Ok) return Error;
    while (*pos != ']') {
        if (parseObjectStreamObject() != Ok) return Error;
        if (skipWhitespaceOrComment() != Ok) return Error;
    }
    pos++;
    lastObject = 0;
    return Ok;
}
StreamStatus
PdfParser::parseNull() {
    return skipKeyword("null", 4);
}
StreamStatus
PdfParser::parseObjectStreamObject() {
//    printf("parseObjectStreamObject %.*s\n", (5>end-pos)?end-pos:5, pos);
    StreamStatus r = read(2, 0);
    if (r == Error) return r;

    char ch = *pos;
    if (ch == 't' || ch == 'f') {
        r = parseBoolean();
    } else if (ch == '+' || ch == '-' || ch == '.' || isdigit(ch)) {
        r = parseNumberOrIndirectObject();
    } else if (ch == '(') {
        r = parseLiteralString();
    } else if (ch == '/') {
        r = parseName();
    } else if (ch == '<') {
        if (end-pos > 1 && pos[1] == '<') {
            r = parseDictionaryOrStream();
        } else {
            r = parseHexString();
        }
    } else if (ch == '[') {
        r = parseArray();
    } else if (ch == 'n') {
        r = parseNull();
    } else {
        return Error;
    }
    if (r != Ok) return r;
    r = skipWhitespaceOrComment();
    return r;
}
StreamStatus
PdfParser::parseContentStreamObject() {
    StreamStatus r = read(2, 0);
    if (r == Error) return r;
//    fprintf(stderr, "parseContentStreamObject %.*s\n",
//        (5>end-pos)?end-pos:5, pos);
//    fprintf(stderr, "pos: %lli\n", stream->getPosition());

    char ch = *pos;
    if (ch == '+' || ch == '-' || ch == '.' || isdigit(ch)) {
        r = parseNumber();
    } else if (ch == '(') {
        r = parseLiteralString();
    } else if (ch == '/') {
        r = parseName();
    } else if (ch == '<') {
        if (end-pos > 1 && pos[1] == '<') {
            r = parseDictionaryOrStream();
        } else {
            r = parseHexString();
        }
    } else if (ch == '[') {
        r = parseArray();
    } else if (isalpha(ch)) {
        r = parseOperator();
    } else {
        return Error;
    }
    if (r != Ok) return r;
    r = skipWhitespaceOrComment();
    return r;
}
StreamStatus
PdfParser::parseObjectStream(StreamBase<char>* s, int32_t offset, int32_t n) {
    stream = s;
    end = pos = start = 0;
    objdefstart = 0;

    stream->skip(offset);
    StreamStatus r = Ok;
    for (int32_t i=0; r == Ok && i<n; ++i) {
        r = parseObjectStreamObject();
    }
    while (r == Ok) {
        s->skip(1000);
        r = s->getStatus();
    }
    if (r == Eof) {
//        printf("size: %i\n", s->getSize());
    }
    return r;
}
StreamStatus
PdfParser::parseContentStream(StreamBase<char>* s) {
    stream = s;
    end = pos = start = 0;
    objdefstart = 0;
    StreamStatus r = skipWhitespaceOrComment();
//    fprintf(stderr, "eh %i %i\n", r, Eof);
    if (r != Ok) return r;
    while ((r = parseContentStreamObject()) == Ok) {};
//    fprintf(stderr, "PdfParser::parseContentStream %i %i\n", r, Eof);
    return r;
}
StreamStatus
PdfParser::skipXRef() {
    // skip header
    if (skipKeyword("xref", 4) != Ok || skipWhitespaceOrComment() != Ok
            || parseNumber() != Ok || skipWhitespaceOrComment() != Ok
            || parseNumber() != Ok || skipWhitespaceOrComment() != Ok) {
        return Error;
    }
    // parse number of entreis
    int entries = (int)lastNumber;
    for (int i = 0; i != entries; ++i) {
        if (parseNumber() != Ok || skipWhitespaceOrComment() != Ok
            || parseNumber() != Ok || skipWhitespaceOrComment() != Ok
            || skipFromString("fn", 2) != Ok
            || skipWhitespaceOrComment() != Ok) {
                return Error;
        }
    }
    return Ok;
}
StreamStatus
PdfParser::skipTrailer() {
    if (skipKeyword("trailer", 7) != Ok || skipWhitespaceOrComment() != Ok
            || parseDictionaryOrStream() != Ok) {
        return Error;
    }
    return Ok;
}
StreamStatus
PdfParser::skipStartXRef() {
    if (skipKeyword("startxref", 9) != Ok || skipWhitespaceOrComment() != Ok
            || parseNumber() != Ok) {
        fprintf(stderr, "error in startxref 1\n");
        return Error;
    }
    return skipWhitespaceOrComment();
}
StreamStatus
PdfParser::parseObjectStreamObjectDef() {
//    objdefstart = pos-start;
    if (*pos == 'x') return skipXRef();
    if (*pos == 't') return skipTrailer();
    if (*pos == 's') return skipStartXRef();
    if (checkForData(13) != Ok) return Error;
//    fprintf(stderr, "parseObjectStreamObjectDef %.*s\n", ((10>end-pos)?end-pos:10), pos);
    if (parseNumber() != Ok || skipWhitespaceOrComment() != Ok
        || parseNumber() != Ok || skipWhitespaceOrComment() != Ok
        || skipKeyword("obj", 3) != Ok || skipWhitespaceOrComment() != Ok
        || parseObjectStreamObject() != Ok || skipWhitespaceOrComment() != Ok
        || skipKeyword("endobj", 6) != Ok) {
        return Error;
    }
    StreamStatus r = skipWhitespaceOrComment();
//    fprintf(stderr, "parsed obj ok %i\n", r);
    return r;
}
StreamStatus
PdfParser::parse(StreamBase<char>* stream) {
    // for now we need to load the entire stream in memory :(
    // this is due to a sneaky bug somewhere, not a design issue
    const char* c;
    //warning:use limited to sized streams
    stream->read(c, 1+stream->getSize(), 0);
    stream->reset(0);
    StreamStatus r = stream->getStatus();
    if (r != Ok) {
        fprintf(stderr, "Error: %s\n", stream->getError());
        return r;
    }

    // initialize the stream status
    this->stream = stream;
    end = pos = start = 0;
    objdefstart = 0;

    // initialize the parsed field containers
    lastNumber = -1;
    lastName.resize(0);
    lastObject = 0;

    r = skipWhitespaceOrComment();
    if (r != Ok) {
        fprintf(stderr, "Error: %s\n", stream->getError());
        return r;
    }
    while ((r = parseObjectStreamObjectDef()) == Ok) {};
//    fprintf(stderr, "%i %i %i\n", r, streamcount, Eof);
    if (r == Error) {
        fprintf(stderr, "Error in parsing: %s\n", error.c_str());
    }
    return r;
}
jstreams::StreamStatus
PdfParser::DefaultStreamHandler::handle(jstreams::StreamBase<char>* s) {
    static int count = 0;
    char name[32];
    const char *c;
    int32_t n = s->read(c, 1, 0);
    if (n <= 0) {
        return s->getStatus();
    }
    sprintf(name, "out/%i", ++count);
    FILE* file = fopen(name, "wb");
    if (file == 0) {
        return Error;
    }
    do {
        fwrite(c, 1, n, file);
        n = s->read(c, 1, 0);
    } while (n > 0);
    fclose(file);
    return s->getStatus();
}
jstreams::StreamStatus
PdfParser::DefaultTextHandler::handle(const string& s) {
    printf("%s\n", s.c_str());
    return Ok;
}
void
PdfParser::forwardStream(StreamBase<char>* s) {
    const char* c;
    int32_t n = s->read(c, 1024, 0);
    while (n >= 0 && s->getStatus() == Ok) {
        n = s->read(c, n, 0);
    }
}
StreamStatus
PdfParser::handleSubStream(StreamBase<char>* s, const std::string& type,
        int32_t offset, int32_t numberofobjects, bool hasfilter,
        const string& filter) {
    if (hasfilter) {
        if (filter == "FlateDecode") {
            GZipInputStream gzip(s, GZipInputStream::ZLIBFORMAT);
            return handleSubStream(&gzip, type, offset, numberofobjects);
        } else {
            // we cannot handle these filters, so we send the raw data
            return handleSubStream(s, type, 0, 0);
        }
    } else {
        return handleSubStream(s, type, offset, numberofobjects);
    }
}
StreamStatus
PdfParser::handleSubStream(StreamBase<char>* s, const std::string& type,
        int32_t offset, int32_t numberofobjects) {
    // try to parse as an object stream
    PdfParser parser;
    parser.texthandler = texthandler;
    parser.streamhandler = streamhandler;
    if (type == "ObjStm") {
        if (parser.parseObjectStream(s, offset, numberofobjects) == Eof) {
            return Eof;
        } else {
            return Error;
        }
    }

    // try to parse as a content stream
    s->reset(0);
    if (parser.parseContentStream(s) == Eof) {
        return Eof;
    }
    // handle the stream by an external handler
    s->reset(0);
    if (streamhandler) {
        streamhandler->handle(s);
    }
    return s->getStatus();
}
