#ifndef INDEXREADERTESTS_H
#define INDEXREADERTESTS_H

#include "verify.h"

namespace jstreams {
    class IndexReader;
}

class IndexReaderTester;
class IndexReaderTests {
private:
    IndexReaderTester* tester;
public:
    IndexReaderTests(jstreams::IndexReader* w);
    ~IndexReaderTests();
    int testAll();
};

#endif
