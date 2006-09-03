#include "indexwritertests.h"
#include "indexwriter.h"
using namespace jstreams;

class IndexWriterTester {
private:
    IndexWriter* writer;
public:
    IndexWriterTester(IndexWriter* w) :writer(w) {}
    int optimize() {
        writer->optimize();
        return 0;
    }
    int add() {
        Indexable i("", 0, writer, 0);
        return 0;
    }
};

IndexWriterTests::IndexWriterTests(jstreams::IndexWriter* w)
    :tester (new IndexWriterTester(w)) {
}
IndexWriterTests::~IndexWriterTests() {
    delete tester;
}

int
IndexWriterTests::testAll() {
    int n = 0;
    n += tester->optimize();
    return n;
}
