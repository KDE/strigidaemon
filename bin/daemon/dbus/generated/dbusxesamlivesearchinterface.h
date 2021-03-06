// generated by makecode.pl
#ifndef DBUSXESAMLIVESEARCHINTERFACE_H
#define DBUSXESAMLIVESEARCHINTERFACE_H
#include "xesam/xesamlivesearchinterface.h"
#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>
#include <exception>
class DBusObjectInterface;
class DBusXesamLiveSearchInterface : public XesamLiveSearchInterface {
private:
    std::string object;
    DBusConnection* const conn;
    DBusObjectInterface* const iface;
    void GetHitsResponse(void* msg, const char* err,             const std::vector<std::vector<Strigi::Variant> >& hits);
    void GetHitCountResponse(void* msg, const char* err, uint32_t count);
    void GetHitDataResponse(void* msg, const char* err,             const std::vector<std::vector<Strigi::Variant> >& hit_data);
    void StateChanged(const std::vector<std::string>& state_info);
    void SearchDone(const std::string& search);
    void HitsModified(const std::string& search,         const std::vector<uint32_t>& hit_ids);
    void HitsRemoved(const std::string& search,         const std::vector<uint32_t>& hit_ids);
    void HitsAdded(const std::string& search, const uint32_t count);
public:
    DBusXesamLiveSearchInterface(const std::string& objectname, DBusConnection* c, XesamLiveSearchInterface* x);
    ~DBusXesamLiveSearchInterface();
    DBusObjectInterface* interface() { return iface; }
};
#endif
