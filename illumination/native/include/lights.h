#ifndef illumination_lights_h
#define illumination_lights_h

#define LIGHT_META "IlluminationLights.Light"


#include <dmsdk/sdk.h>
#include "utils.h"


namespace IlluminationLights {



struct Light {
    int index;
    bool enabled;
    dmVMath::Vector3 position;
    dmVMath::Vector3 direction;
    dmVMath::Vector4 color; //r,g,b brightness
    float radius;
    float smoothness;
    float specular;
    float cutoff;
    float aabb[6];
    bool dirty;
};

//not lightuserdata type it base userdata
//user data for Light object
struct LuaLightUserData {
    Light* light;
    bool valid; //make userdata invalid when user delete light
};

struct LightCluster {
    int numLights;
    Light** lights;
};



//region Light
inline void LightUpdateAABB(Light* light){
    float radius = light->radius;
    float x = light->position.getX();
    float y = light->position.getY();
    float z = light->position.getZ();
    light->aabb[0] = x - radius;
    light->aabb[1] = y - radius;
    light->aabb[2] = z - radius;
    light->aabb[3] = x + radius;
    light->aabb[4] = y + radius;
    light->aabb[5] = z + radius;
}

//region Light
inline void LightReset(Light* light){
    light->position = dmVMath::Vector3(0.0f, 0.0f, 0.0f);
    light->direction = dmVMath::Vector3(0.0f, 0.0f, -1.0f);
    light->color = dmVMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    light->radius = 1.0f;
    light->smoothness = 0.5f;
    light->specular = 0.5f;
    light->cutoff = 1.0f;
    light->dirty = true; // Default dirty flag
    LightUpdateAABB(light);
}

//endregion

//region light Lua

inline LuaLightUserData* LightUserdataCheck(lua_State* L, int index){
    LuaLightUserData* userData = (LuaLightUserData*)luaL_checkudata(L, index, LIGHT_META);
     if (!userData->valid) {luaL_error(L,"Light userdata not valid");}
     return userData;
}

static int LightToString(lua_State* L) {
    // Check if the first argument is a userdata of the expected type
    LuaLightUserData* userData = LightUserdataCheck(L,1);
    if (!userData->valid) {
        lua_pushfstring(L, "Light[%d]. Invalid userdata",userData->light->index);
        return 1; // Number of results returned to Lua
    }

    Light* light = userData->light;

    // Use lua_pushfstring to create and push a formatted string onto the Lua stack
    lua_pushfstring(L, "Light[%d]: Position(%.2f, %.2f, %.2f)",
                    light->index,
                    light->position.getX(),
                    light->position.getY(),
                    light->position.getZ());
    return 1; // Number of results returned to Lua
}

static int LightUserdataGC(lua_State* L) {
    LuaLightUserData* userData = (LuaLightUserData*)lua_touserdata(L, 1);
    if(userData->valid){
        dmLogError("Light[%d] memory leak. Userdata was garbage collected without illumination_lights.light_destroy(light)",userData->light->index);
    }
    return 0;
}



// Array of functions to register
static const luaL_Reg functions[] = {
    {"__gc", LightUserdataGC},
    {"__tostring", LightToString},
    {NULL, NULL}  // Sentinel element
};
//endregion

class LightsManager {
public:
    bool inited = false;
    Light* lights = NULL;
    LightCluster* clusters = NULL;
    dmArray<Light*> lightsPool;
    dmArray<Light*> lightsInWorld;
    int numLights,maxLightsPerCluster;
    int xSlice, ySlice, zSlice;

    LightsManager(){

    }
    ~LightsManager(){
        delete[] lights;
        if (clusters != NULL) {
            for (int i = 0; i < xSlice * ySlice * zSlice; ++i) {
                delete[] clusters[i].lights;
            }
        }
        delete[] clusters;
    }

    void init(int numLights, int xSlice, int ySlice, int zSlice, int maxLightsPerCluster){
        assert(!inited);
        assert(numLights>0);
        assert(xSlice>0);
        assert(ySlice>0);
        assert(zSlice>0);
        assert(maxLightsPerCluster>0);
        inited = true;
        this->numLights = numLights;
        this->xSlice = xSlice;
        this->ySlice = ySlice;
        this->zSlice = zSlice;
        this->maxLightsPerCluster = maxLightsPerCluster;
        lights = new Light[numLights];
        lightsPool.SetCapacity(numLights);
        lightsInWorld.SetCapacity(numLights);

        for (int i = 0; i < numLights; ++i) {
            Light* light = &lights[i];
             // Initialize default values for each light
            light->index = numLights-1-i;
            light->enabled = false;
            lightsPool.Push(light);
        }

         int totalClusters = xSlice * ySlice * zSlice;
         clusters = new LightCluster[totalClusters];

        for (int i = 0; i < totalClusters; ++i) {
            LightCluster* cluster = &clusters[i];
            cluster->lights = new Light*[maxLightsPerCluster];
            cluster->numLights = 0;
        }


    }
private:
    LightsManager(const LightsManager&);
};

extern LightsManager g_lightsManager;// add g_lightsManager in extension.cpp


static int LightsManagerInitLua(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 5);
    if(g_lightsManager.inited){
        dmLogError("LightsManager already inited");
        return 0;
    }

    int numLights = luaL_checkinteger(L, 1);
    int xSlice = luaL_checkinteger(L, 2);
    int ySlice = luaL_checkinteger(L, 3);
    int zSlice = luaL_checkinteger(L, 4);
    int maxLightsPerCluster = luaL_checkinteger(L, 5);

    if (numLights < 0) { return DM_LUA_ERROR("numLights must be non-negative");}
    if (xSlice < 0) { return DM_LUA_ERROR("xSlice must be non-negative");}
    if (ySlice < 0) { return DM_LUA_ERROR("ySlice must be non-negative");}
    if (zSlice < 0) { return DM_LUA_ERROR("zSlice must be non-negative");}
    if (maxLightsPerCluster < 0) { return DM_LUA_ERROR("maxLightsPerCluster must be non-negative");}

    g_lightsManager.init(numLights, xSlice, ySlice, zSlice, maxLightsPerCluster);

    dmLogInfo("LightsManager inited.Lights:%d xSlice:%d ySlice:%d zSlice:%d maxLightsPerCluster:%d" ,
        numLights, xSlice, ySlice, zSlice, maxLightsPerCluster);
    return 0;
}

static int LightsManagerCreateLight(lua_State* L){
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }

    if(g_lightsManager.lightsPool.Empty()){
        return DM_LUA_ERROR("LightsManager lightsPool is empty");
    }

    Light* light = g_lightsManager.lightsPool[g_lightsManager.lightsPool.Size()-1];
    g_lightsManager.lightsPool.Pop();


   LuaLightUserData* user_data = (LuaLightUserData*)lua_newuserdata(L, sizeof(LuaLightUserData));
   user_data->light = light;
   user_data->valid = true;


    if (luaL_newmetatable(L, LIGHT_META)) {
        luaL_register (L, NULL,functions);
        lua_pushvalue(L, -1);
        lua_setfield(L, -1, "__index");
    }
    lua_setmetatable(L, -2);


    return 1;
}
static int LightsManagerDestroyLight(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }

    LuaLightUserData* userData = LightUserdataCheck(L,1);

    userData->valid = false;

    LightReset(userData->light);
    g_lightsManager.lightsPool.Push(userData->light);

    return 0;
}

}  // namespace IlluminationLights


#endif
