#include "xapianindexreader.h"
#include "xapianindexmanager.h"
#include <xapian.h>
#include <set>
#include <sstream>
#include <assert.h>
using namespace std;
using namespace jstreams;
using namespace Xapian;

XapianIndexReader::XapianIndexReader(XapianIndexManager* m, WritableDatabase* d)
    : manager(m), db(d) {
}
XapianIndexReader::~XapianIndexReader() {
}
vector<IndexedDocument>
XapianIndexReader::query(const std::string& query) {
    std::vector<IndexedDocument> results;
    return results;
}
map<string, time_t>
XapianIndexReader::getFiles(char depth) {
    map<string, time_t> files;
    return files;
}
int
XapianIndexReader::countDocuments() {
    int count = 0;
    manager->ref();
    manager->deref();
    return count;
}
