#ifndef illumination_lights_h
#define illumination_lights_h

#include <dmsdk/sdk.h>
#include "utils.h"
#include <cmath> // For sqrt, log2, and pow functions

#define LIGHT_META "IlluminationLights.Light"
#define LIGHT_PIXELS 6 // pixels per light
#define LIGHT_RADIUS_MAX 64 // store in r value of pixel. Mb store as rgba value for better precision?
#define LIGHT_MIN_POSITION -511
#define LIGHT_MAX_POSITION 512
#define LIGHT_AXIS_CAPACITY 1024 //[-511 512]





namespace IlluminationLights {

//region TEXTURE
static const dmhash_t HASH_RGBA = dmHashString64("rgba");
static const dmBuffer::StreamDeclaration rgba_buffer_decl[] = {
    {HASH_RGBA, dmBuffer::VALUE_TYPE_UINT8, 4},
};


// Function to find the smallest power of two greater than or equal to n
int SmallestPowerOfTwo(int n) {
    if (n <= 0) return 1;

    int logVal = (int)std::ceil(std::log2(n));
    return (int)std::pow(2, logVal);
}

// Function to find the width and height for the texture
void FindTextureDimensions(int totalPixels, int& width, int& height) {
    if (totalPixels <= 0) {
        width = height = 1;
        return;
    }

    // Start with a square texture estimate
    int sqrtPixels = (int)std::sqrt(totalPixels);
    int initialDimension = SmallestPowerOfTwo(sqrtPixels);

    width = initialDimension;
    height = initialDimension;

    //check if one side can be smaller. Texture can be not square
    if (width * (height / 2) >= totalPixels) {
        height /= 2;
    }
}
//endregion



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
    light->dirty = true;
    LightUpdateAABB(light);
}

//region Light
inline void LightSetPosition(Light* light, float x, float y, float z) {
  if (x < LIGHT_MIN_POSITION || x > LIGHT_MAX_POSITION ||
        y < LIGHT_MIN_POSITION || y > LIGHT_MAX_POSITION ||
        z < LIGHT_MIN_POSITION || z > LIGHT_MAX_POSITION) {
        dmLogWarning("Light position out of bounds. Clamping to [%d, %d].", LIGHT_MIN_POSITION, LIGHT_MAX_POSITION);
        x = fmax((float)LIGHT_MIN_POSITION, fmin(x, (float)LIGHT_MAX_POSITION));
        y = fmax((float)LIGHT_MIN_POSITION, fmin(y, (float)LIGHT_MAX_POSITION));
        z = fmax((float)LIGHT_MIN_POSITION, fmin(z, (float)LIGHT_MAX_POSITION));
    }

    if (light->position.getX()!=x || light->position.getY()!=y || light->position.getZ()!=z) {
        light->position = dmVMath::Vector3(x,y,z);
        LightUpdateAABB(light);
        light->dirty = true;
    }
}

inline void LightSetDirection(Light* light, float x, float y, float z) {
    dmVMath::Vector3 newDir = Vectormath::Aos::normalize(dmVMath::Vector3(x, y, z));
    if (light->direction.getX() != newDir.getX() || light->direction.getY() != newDir.getY() || light->direction.getZ() != newDir.getZ()) {
        light->direction = newDir;
        light->dirty = true;
    }
}

inline void LightSetColor(Light* light, float r, float g, float b, float brightness) {
    if (light->color.getX() != r || light->color.getY() != g || light->color.getZ() != b || light->color.getW() != brightness) {
        light->color = dmVMath::Vector4(r, g, b, brightness);
        light->dirty = true;
    }
}

inline void LightSetRadius(Light* light, float newRadius) {
    if (newRadius < 0.0f || newRadius > LIGHT_RADIUS_MAX) {
        dmLogWarning("Light radius out of bounds. Clamping to [0, %d].", LIGHT_RADIUS_MAX);
        newRadius = fmax(0.0f, fmin(newRadius, (float)LIGHT_RADIUS_MAX));
    }

    if (light->radius != newRadius) {
        light->radius = newRadius;
        LightUpdateAABB(light);
        light->dirty = true;
    }
}

inline void LightSetSmoothness(Light* light, float newSmoothness) {
    newSmoothness = fmax(0.0f, fmin(newSmoothness, 1.0f));
    if (light->smoothness != newSmoothness) {
        light->smoothness = newSmoothness;
        light->dirty = true;
    }
}

inline void LightSetCutoff(Light* light, float newCutoff) {
    newCutoff = fmax(0.0f, fmin(newCutoff, 1.0f));
    if (light->cutoff != newCutoff) {
        light->cutoff = newCutoff;
        light->dirty = true;
    }
}

inline void LightSetSpecular(Light* light, float newSpecular) {
    newSpecular = fmax(0.0f, fmin(newSpecular, 1.0f));
    if (light->specular != newSpecular) {
        light->specular = newSpecular;
        light->dirty = true;
    }
}

inline void LightSetEnabled(Light* light, bool enabled) {
    light->enabled = enabled;
}

inline bool LightIsAddLightToScene(Light* light) {
	return light->enabled && light->color.getW() > 0.0f;
}


//endregion

//region light Lua

inline LuaLightUserData* LightUserdataCheck(lua_State* L, int index){
    LuaLightUserData* userData = (LuaLightUserData*)luaL_checkudata(L, index, LIGHT_META);
     if (!userData->valid) {luaL_error(L,"Light userdata not valid");}
     return userData;
}

static int LuaLightToString(lua_State* L) {
    LuaLightUserData* userData = (LuaLightUserData*)luaL_checkudata(L, 1, LIGHT_META);
    if (!userData->valid) {
        lua_pushfstring(L, "Light[%d]. Invalid userdata",userData->light->index);
        return 1;
    }

    Light* light = userData->light;

    lua_pushfstring(L, "Light[%d]: Position(%.2f, %.2f, %.2f)",
                    light->index,
                    light->position.getX(),
                    light->position.getY(),
                    light->position.getZ());
    return 1;
}

static int LuaLightUserdataGC(lua_State* L) {
    LuaLightUserData* userData = (LuaLightUserData*)lua_touserdata(L, 1);
    if(userData->valid){
        dmLogError("Light[%d] memory leak. Userdata was garbage collected without illumination_lights.light_destroy(light)",userData->light->index);
    }
    return 0;
}

static int LuaLightSetPosition(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 4);
    LuaLightUserData* userData = LightUserdataCheck(L,1);
    LightSetPosition(userData->light,
                     luaL_checknumber(L, 2),
                     luaL_checknumber(L, 3),
                     luaL_checknumber(L, 4));

    return 0;
}

static int LuaLightGetPosition(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L,1);
    Vectormath::Aos::Vector3 *out = dmScript::CheckVector3(L, 2);
    *out = userData->light->position;

    return 0;
}

static int LuaLightSetDirection(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 4);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetDirection(userData->light,
                      luaL_checknumber(L, 2),
                      luaL_checknumber(L, 3),
                      luaL_checknumber(L, 4));

    return 0;
}

static int LuaLightGetDirection(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    Vectormath::Aos::Vector3 *out = dmScript::CheckVector3(L, 2);
    *out = userData->light->direction;

    return 0;
}

static int LuaLightSetColor(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 5);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetColor(userData->light,
                  luaL_checknumber(L, 2),
                  luaL_checknumber(L, 3),
                  luaL_checknumber(L, 4),
                  luaL_checknumber(L, 5));

    return 0;
}

static int LuaLightGetColor(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    Vectormath::Aos::Vector4 *out = dmScript::CheckVector4(L, 2);
    *out = userData->light->color;

    return 0;
}

static int LuaLightSetRadius(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetRadius(userData->light, luaL_checknumber(L, 2));

    return 0;
}

static int LuaLightGetRadius(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    lua_pushnumber(L, userData->light->radius);

    return 1;
}

static int LuaLightSetSmoothness(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetSmoothness(userData->light, luaL_checknumber(L, 2));

    return 0;
}

static int LuaLightGetSmoothness(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    lua_pushnumber(L, userData->light->smoothness);

    return 1;
}

static int LuaLightSetCutoff(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetCutoff(userData->light, luaL_checknumber(L, 2));

    return 0;
}

static int LuaLightGetCutoff(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    lua_pushnumber(L, userData->light->cutoff);

    return 1;
}

static int LuaLightSetSpecular(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    LightSetSpecular(userData->light, luaL_checknumber(L, 2));

    return 0;
}

static int LuaLightGetSpecular(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    lua_pushnumber(L, userData->light->specular);

    return 1;
}


static int LuaLightSetEnabled(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 2);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);
    bool enabled = lua_toboolean(L, 2);

    LightSetEnabled(userData->light, enabled);

    return 0;
}

static int LuaLightIsEnabled(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 1);
    LuaLightUserData* userData = LightUserdataCheck(L, 1);

    lua_pushboolean(L, userData->light->enabled);

    return 1;
}

static const luaL_Reg functions[] = {
    {"set_position", LuaLightSetPosition},
    {"get_position", LuaLightGetPosition},
    {"set_direction", LuaLightSetDirection},
    {"get_direction", LuaLightGetDirection},
    {"set_color", LuaLightSetColor},
    {"get_color", LuaLightGetColor},
    {"set_radius", LuaLightSetRadius},
    {"get_radius", LuaLightGetRadius},
    {"set_smoothness", LuaLightSetSmoothness},
    {"get_smoothness", LuaLightGetSmoothness},
    {"set_cutoff", LuaLightSetCutoff},
    {"get_cutoff", LuaLightGetCutoff},
    {"set_specular", LuaLightSetSpecular},
    {"get_specular", LuaLightGetSpecular},
    {"set_enabled", LuaLightSetEnabled},
    {"is_enabled", LuaLightIsEnabled},
    {"__gc", LuaLightUserdataGC},
    {"__tostring", LuaLightToString},
    {NULL, NULL}
};
//endregion

class LightsManager {
public:
    bool inited = false;
    Light* lights = NULL;
    LightCluster* clusters = NULL;

    dmArray<Light*> lightsPool;
    dmArray<Light*> lightsInWorld;
    int numLights,maxLightsPerCluster,pixelsPerCluster;
    int totalClusters;
    int xSlice, ySlice, zSlice;

    dmBuffer::HBuffer textureBuffer = 0x0;
    int textureWidth, textureHeight;

    LightsManager(){

    }
    ~LightsManager(){
        delete[] lights;
        if (clusters != NULL) {
            for (int i = 0; i < totalClusters; ++i) {
                delete[] clusters[i].lights;
            }
        }
        delete[] clusters;
        //for some reasons engine crash when destroy texture buffer. Maybe it destroy in other place when app is closed.
        //dmBuffer::Destroy(textureBuffer);
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
        //1 pixel(rgba) lights count
        //max_lights_per_cluster * 1pixel(rgba) light idx
        pixelsPerCluster = 1 + maxLightsPerCluster;
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

        totalClusters = xSlice * ySlice * zSlice;
        clusters = new LightCluster[totalClusters];

        for (int i = 0; i < totalClusters; ++i) {
            LightCluster* cluster = &clusters[i];
            cluster->lights = new Light*[maxLightsPerCluster];
            cluster->numLights = 0;
        }

        int pixels = LIGHT_PIXELS * numLights + totalClusters * pixelsPerCluster;
        FindTextureDimensions(pixels, textureWidth, textureHeight);
        dmLogInfo("Total pixels:%d.Lights texture: %d x %d", pixels,textureWidth, textureHeight);
        dmBuffer::Result r = dmBuffer::Create(textureWidth * textureHeight, rgba_buffer_decl,1, &textureBuffer);
        if (r != dmBuffer::RESULT_OK) {
            dmLogError("Failed to create lights texture buffer");
            return;
        }
        //dmBuffer::Destroy(textureBuffer);
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
