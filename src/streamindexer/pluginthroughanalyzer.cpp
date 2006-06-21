#include "pluginthroughanalyzer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
//#include <ltdl.h>
#include <dlfcn.h>

using namespace std;
using namespace jstreams;

list<const PluginThroughAnalyzer::Factory*> PluginThroughAnalyzer::factories;

PluginThroughAnalyzer::FactoryCleaner PluginThroughAnalyzer::cleaner;
PluginThroughAnalyzer::FactoryCleaner::~FactoryCleaner() {
    list<void*>::const_iterator i;
    for (i = loadedModules.begin(); i != loadedModules.end(); ++i) {
        dlclose(*i);
    }
}

PluginThroughAnalyzer::Factory::Factory(createThroughAnalyzer_t c,
        destroyThroughAnalyzer_t d) : create(c), destroy(d) {
    PluginThroughAnalyzer::factories.push_back(this);
}
            
PluginThroughAnalyzer::Factory::~Factory() {
    std::list<const Factory*>::iterator i;
    for (i = PluginThroughAnalyzer::factories.begin();
        i != PluginThroughAnalyzer::factories.end(); ++i) {
        if (*i == this) break;
    }
    PluginThroughAnalyzer::factories.erase(i);
}

PluginThroughAnalyzer::PluginThroughAnalyzer() {
    std::list<const Factory*>::iterator i;
    for (i = factories.begin(); i != factories.end(); ++i) {
        StreamThroughAnalyzer* a = (*i)->create();
        analyzers.push_back(a);
    }
}
PluginThroughAnalyzer::~PluginThroughAnalyzer() {
    vector<StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        delete *i;
    }
}
void
PluginThroughAnalyzer::loadPlugin(const string& lib) {
    void* handle = dlopen (lib.c_str(), RTLD_NOW);
    if (handle) {
        cleaner.addModule(handle);
        printf("loaded %s\n", lib.c_str());
    } else {
        printf("%s\n", dlerror());
    }
}
void
PluginThroughAnalyzer::loadPlugins(const char*d) {
    DIR *dir = opendir(d);
    if (dir == 0) {
        // TODO handle error
        return;
    }
    struct dirent* ent = readdir(dir);
    while(ent) {
        //printf("%s\n", ent->d_name);
        ssize_t len = strlen(ent->d_name);
        if (strncmp(ent->d_name, "strigita_", 9) == 0
                && strcmp(ent->d_name+len-3, ".so") == 0) {
            string plugin = d;
            if (plugin[plugin.length()-1] != '/') plugin += '/';
            plugin += ent->d_name;
            // check that the file is a regular file
            struct stat s;
            if (stat(plugin.c_str(), &s) == 0 && (S_IFREG & s.st_mode)) {
                loadPlugin(plugin);
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}
jstreams::InputStream *
PluginThroughAnalyzer::connectInputStream(jstreams::InputStream *in) {
    vector<StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        in = (*i)->connectInputStream(in);
    }
    return in;
}
void
PluginThroughAnalyzer::setIndexable(jstreams::Indexable* idx) {
    vector<StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        (*i)->setIndexable(idx);
    }
}
