#ifndef PLAYLISTINTERFACE_H
#define PLAYLISTINTERFACE_H

#include "modulebase.h"
struct PlayListItem;
namespace Extension
{

class PlayListInterface : public ModuleBase
{
public:
    using ModuleBase::ModuleBase;
    virtual void setup();
private:
    static int add(lua_State *L);
    static int curitem(lua_State *L);
    static int get(lua_State *L);

    static QVariantMap getItemInfo(const PlayListItem *item);
};

}
#endif // PLAYLISTINTERFACE_H
