#ifndef illumination_lights_h
#define illumination_lights_h

#include <dmsdk/sdk.h>
#include "utils.h"
#include "frustum_cull.h"
#include <cmath> // For sqrt, log2, and pow functions
#include <sstream>  // For std::ostringstream
#include <string>   // For std::string
#include <cstring>  // For std::strcpy


#define LIGHT_META "IlluminationLights.Light"
#define LIGHT_PIXELS 6 // pixels per light
#define LIGHT_RADIUS_MAX 64.0 // stored as integer and fractal part in different pixels

#define LIGHT_MIN_POSITION -131072.0f
#define LIGHT_MAX_POSITION 131071.0f

#define M_PI  3.14159265358979323846  /* pi */





namespace IlluminationLights {

//region TEXTURE
static const dmhash_t HASH_RGBA = dmHashString64("rgba");
static const dmhash_t HASH_EMPTY = dmHashString64("empty");
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

inline float Fract(float f){
    return f - floor(f);
}

//encode from [0;1)
inline dmVMath::Vector4 EncodeFloatRGBA(float v, float min, float max){
    //if (v<MIN_BORDER){MIN_BORDER = v;}
    //if (v>MAX_BORDER){MAX_BORDER = v;}
    assert(v>=min);
    assert(v<max);
    assert(max>min);
    v = (v- min)/(max-min);

    dmVMath::Vector4 enc = dmVMath::Vector4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc.setX(Fract(enc.getX()));
    enc.setY(Fract(enc.getY()));
    enc.setZ(Fract(enc.getZ()));
    enc.setW(Fract(enc.getW()));

    return enc;
}

inline void EncodeIntToRGBA(int value, uint8_t* output) {
    output[0] = static_cast<uint8_t>((value >> 24) & 0xFF); // Red
    output[1] = static_cast<uint8_t>((value >> 16) & 0xFF); // Green
    output[2] = static_cast<uint8_t>((value >> 8) & 0xFF);  // Blue
    output[3] = static_cast<uint8_t>(value & 0xFF);         // Alpha
}

inline void EncodeFloatPositionToRGBA(float value, uint8_t* output) {
     assert(value >= -131072.0f && value <= 131071.0f);

    // Extract the integer and fractional parts of the float
    int intValue = static_cast<int>(value);
    float fractionalPart = value - intValue;

    // Adjust for negative values
    if (fractionalPart < 0.0f) {
        intValue -= 1;
        fractionalPart += 1.0f;
    }

    // Shift the integer part to ensure it is positive and fits within 18 bits
    uint32_t uintValue = static_cast<uint32_t>(intValue + 131072); // 262144 = 2^18 / 2

    // Ensure the integer part fits within 18 bits (6 bits per channel)
    assert(uintValue <= 262143); // 262143 = 2^18 - 1

    // Encode the integer part into the RGB channels (6 bits per channel)
    output[0] = round((uintValue >> 12) & 0x3F)/63.0*255; // Red channel (bits 17-12)
    output[1] = round((uintValue >> 6) & 0x3F)/63.0*255;  // Green channel (bits 11-6)
    output[2] = round(uintValue & 0x3F)/63.0*255;         // Blue channel (bits 5-0)

    // Encode the fractional part into the Alpha channel (8 bits)
    output[3] = static_cast<uint8_t>(round(fractionalPart * 255.0f));
}


struct Light {
    int index;
    uint8_t encodedIndex[4];
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
    uint8_t* clusterStart;
    uint8_t* currentLightStart;
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
    light->smoothness = 1.0f;
    light->specular = 0.5f;
    light->cutoff = 1.0f;
    light->dirty = true;
    LightUpdateAABB(light);
}

//region Light
inline void LightSetPosition(Light* light, float x, float y, float z) {
    if (x < LIGHT_MIN_POSITION || x > LIGHT_MAX_POSITION) {
      dmLogWarning("Light X position out of bounds. Clamping to [%f, %f].", LIGHT_MIN_POSITION, LIGHT_MAX_POSITION);
      x = fmax(LIGHT_MIN_POSITION, fmin(x, LIGHT_MAX_POSITION));
    }

    if (y < LIGHT_MIN_POSITION || y > LIGHT_MAX_POSITION) {
      dmLogWarning("Light Y position out of bounds. Clamping to [%f, %f].", LIGHT_MIN_POSITION, LIGHT_MAX_POSITION);
      y = fmax(LIGHT_MIN_POSITION, fmin(y, LIGHT_MAX_POSITION));
    }

    if (z < LIGHT_MIN_POSITION || z > LIGHT_MAX_POSITION) {
      dmLogWarning("Light Z position out of bounds. Clamping to [%f, %f].", LIGHT_MIN_POSITION, LIGHT_MAX_POSITION);
      z = fmax(LIGHT_MIN_POSITION, fmin(z, LIGHT_MAX_POSITION));
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
        dmLogWarning("Light radius out of bounds. Clamping to [0, %f].", LIGHT_RADIUS_MAX);
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

inline void LightWriteToBuffer(Light* light, uint8_t* values,  uint32_t stride) {
    EncodeFloatPositionToRGBA(light->position.getX(), values);
    values+=stride;
    EncodeFloatPositionToRGBA(light->position.getY(), values);
    values+=stride;
    EncodeFloatPositionToRGBA(light->position.getZ(), values);
    values+=stride;

    values[0] = (light->direction.getX() + 1)/2*255;
    values[1] = (light->direction.getY() + 1)/2*255;
    values[2] = (light->direction.getZ() + 1)/2*255;
    // values[3] = 0 //empty field use it to handle radius fractional part
    values[3] = (light->radius - static_cast<int>(light->radius))*255;
    values+=stride;

    values[0] = light->color.getX()*255;
    values[1] = light->color.getY()*255;
    values[2] = light->color.getZ()*255;
    values[3] = light->color.getW()*255;
    values+=stride;


    values[0] = light->radius/ LIGHT_RADIUS_MAX * 255;
    values[1] = light->smoothness * 255;
    values[2] = light->specular * 255;
    values[3] = light->cutoff < 1 ? (cos(light->cutoff * M_PI) + 1) / 2 * 255 : 255;
    values+=stride;
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
    Frustum frustum = Frustum(dmVMath::Matrix4());
    dmVMath::Matrix4 view;

    dmArray<Light*> lightsPool;
    dmArray<Light*> lightsInWorld;
    dmArray<Light*> lightsVisibleInWorld;
    int numLights,maxLightsPerCluster,pixelsPerCluster;
    uint8_t* encodedClusterLights;//precalculate num light in cluster and store in rgba
    int totalClusters;
    int xSlice, ySlice, zSlice;
    int debugVisibleLights = 0;

    float halfY,ylengthPerCluster,xlengthPerCluster,zlengthPerCluster;
    dmVMath::Vector3* normalXClusters = NULL;
    dmVMath::Vector3* normalYClusters = NULL;

    float cameraAspect, cameraFov, cameraFar, cameraNear;

    dmBuffer::HBuffer textureBuffer = 0x0;
    int textureWidth, textureHeight;
    int textureParamsRef = LUA_NOREF;
    dmhash_t texturePath = HASH_EMPTY;


    LightsManager(){
        cameraAspect = 1;
        cameraFov = 1;
        cameraFar = 1;
        cameraNear =0.01;

    }
    ~LightsManager(){
        delete[] lights;
        delete[] clusters;
        delete[] normalXClusters;
        delete[] normalYClusters;
        //need L to unref
        //luaL_unref(L, LUA_REGISTRYINDEX, textureParamsRef);
        //for some reasons engine crash when destroy texture buffer. Maybe it destroy in other place when app is closed.
        //dmBuffer::Destroy(textureBuffer);
    }

    void init(lua_State* L, int numLights, int xSlice, int ySlice, int zSlice, int maxLightsPerCluster){
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
        lightsVisibleInWorld.SetCapacity(numLights);

        for (int i = 0; i < numLights; ++i) {
            Light* light = &lights[i];
             // Initialize default values for each light
            light->index = numLights-1-i;
            dmVMath::Vector4 encodedIndex = EncodeFloatRGBA(light->index,0,numLights+1)*255;
            light->encodedIndex[0] = encodedIndex.getX();
            light->encodedIndex[1] = encodedIndex.getY();
            light->encodedIndex[2] = encodedIndex.getZ();
            light->encodedIndex[3] = encodedIndex.getW();
            light->enabled = false;
            LightReset(light);
            lightsPool.Push(light);
        }

        totalClusters = xSlice * ySlice * zSlice;
        clusters = new LightCluster[totalClusters];

        encodedClusterLights = new uint8_t[maxLightsPerCluster*4];
        uint8_t* encodedClusterLightsIterator = encodedClusterLights;
        for (int i = 0; i < maxLightsPerCluster; ++i) {
            dmVMath::Vector4 lights = EncodeFloatRGBA(i,0,maxLightsPerCluster+1)*255;
            encodedClusterLightsIterator[0] = lights.getX();
            encodedClusterLightsIterator[1] = lights.getY();
            encodedClusterLightsIterator[2] = lights.getZ();
            encodedClusterLightsIterator[3] = lights.getW();
            encodedClusterLightsIterator+=4;
        }

        int pixels = LIGHT_PIXELS * numLights + totalClusters * pixelsPerCluster;
        FindTextureDimensions(pixels, textureWidth, textureHeight);
        dmLogInfo("Total pixels:%d.Lights texture: %d x %d", pixels,textureWidth, textureHeight);

        /*
        //not worked when init in render.
        //resource not exited yet
        //create params for texture use it later when changed texture
        lua_getglobal(L, "resource");

        lua_getfield(L, -1, "TEXTURE_TYPE_2D");
        int textureType2D = lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "TEXTURE_FORMAT_RGBA");
        int textureFormatRGBA = lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_pop(L, 1);
        */
        //
      int textureType2D = 0;
      int textureFormatRGBA = 3;

        // Create params
        lua_newtable(L);

        // Set width
        lua_pushstring(L, "width");
        lua_pushinteger(L, textureWidth);
        lua_settable(L, -3);

        // Set height
        lua_pushstring(L, "height");
        lua_pushinteger(L, textureHeight);
        lua_settable(L, -3);

        // Set type
        lua_pushstring(L, "type");
        lua_pushinteger(L, textureType2D);
        lua_settable(L, -3);

        // Set format
        lua_pushstring(L, "format");
        lua_pushinteger(L, textureFormatRGBA);
        lua_settable(L, -3);

        // Set num_mip_maps
        lua_pushstring(L, "num_mip_maps");
        lua_pushinteger(L, 1);
        lua_settable(L, -3);

        // Store userdata in the registry and save the reference
        textureParamsRef = luaL_ref(L, LUA_REGISTRYINDEX);

        dmBuffer::Result r = dmBuffer::Create(textureWidth * textureHeight, rgba_buffer_decl,1, &textureBuffer);
        if (r != dmBuffer::RESULT_OK) {
            dmLogError("Failed to create lights texture buffer");
            return;
        }

        uint8_t* values = 0x0;
        uint32_t stride = 0;
        dmBuffer::Result dataResult = dmBuffer::GetStream(textureBuffer, HASH_RGBA, (void**)&values, 0x0, 0x0, &stride);
        if (dataResult != dmBuffer::RESULT_OK) {
            luaL_error(L,"can't get stream for lights texture");
        }

        uint8_t* clusterValues = values + numLights * LIGHT_PIXELS * stride;
        for (int i = 0; i < totalClusters; ++i) {
            LightCluster* cluster = &clusters[i];
            cluster->numLights = 0;
            cluster->clusterStart = clusterValues;
            cluster->currentLightStart = clusterValues+4;
            clusterValues += pixelsPerCluster * stride;
        }
    }
private:
    LightsManager(const LightsManager&);
};

inline dmVMath::Vector3 getNormalComponents(float angle){
    float bigHypot = sqrt(1 + angle*angle);
    float normSide1 = 1.0 / bigHypot;
    float normSide2 = -angle*normSide1;
    return dmVMath::Vector3(normSide1,normSide2,0);
}

inline void LightsManagerUpdateLights(lua_State* L,LightsManager* lightsManager){
    assert(lightsManager->inited);
    if(lightsManager->texturePath==HASH_EMPTY){
        luaL_error(L,"LightsManager texture path not set");
    }

    // Update lights
   lightsManager->lightsVisibleInWorld.SetSize(0);

    for (int i = 0; i < lightsManager->lightsInWorld.Size(); ++i) {
        Light* light = lightsManager->lightsInWorld[i];
        bool addToScene = LightIsAddLightToScene(light);
        if (addToScene) {
            addToScene = lightsManager->frustum.IsBoxVisible(dmVMath::Vector3(light->aabb[0], light->aabb[1], light->aabb[2]),
                                                            dmVMath::Vector3(light->aabb[3], light->aabb[4], light->aabb[5]));
        }
        if(addToScene){
            lightsManager->lightsVisibleInWorld.Push(light);
        }
    }

    //https://github.com/LanLou123/WebGL-Clustered-Deferred-Forward-Plus-Rendering/blob/master/src/renderers/base.js
    lightsManager->debugVisibleLights = 0;

    for (int i = 0; i < lightsManager->lightsVisibleInWorld.Size(); ++i) {
        Light* l = lightsManager->lightsVisibleInWorld[i];

        float lightRadius = l->radius;
        dmVMath::Vector4 viewPos  = lightsManager->view * dmVMath::Vector4(l->position.getX(),l->position.getY(),
           l->position.getZ(),1);
        //viewPos /= viewPos.getW();

        dmVMath::Vector3 lightPos = dmVMath::Vector3(viewPos.getX(),viewPos.getY(),-viewPos.getZ());

        int xminidx = lightsManager->xSlice;
        int xmaxidx = lightsManager->xSlice;
        int yminidx = lightsManager->ySlice;
        int ymaxidx = lightsManager->ySlice;
        float minposz = lightPos.getZ() - lightsManager->cameraNear - lightRadius;
        float maxposz = lightPos.getZ() - lightsManager->cameraNear + lightRadius;
        int zminidx  = floor(minposz / lightsManager->zlengthPerCluster);
        int zmaxidx   = floor(maxposz  / lightsManager->zlengthPerCluster)+1;

        if(zminidx >  lightsManager->zSlice-1 || zmaxidx < 0) {
            continue;
        }
        zminidx = fmax(0, zminidx);
        zmaxidx = fmin(lightsManager->zSlice, zmaxidx);


        for(int j = 0; j <= lightsManager->xSlice; ++j) {
            if (dmVMath::Dot(lightPos, lightsManager->normalXClusters[j]) < lightRadius) {
                xminidx = j - 1;
                break;
            }
        }

        for(int j = xminidx + 2; j <= lightsManager->xSlice; ++j) {
            if (dmVMath::Dot(lightPos, lightsManager->normalXClusters[j]) < -lightRadius) {
                xmaxidx = j;
                break;
            }
        }

        for(int j = 0; j <= lightsManager->ySlice; ++j) {
            if (dmVMath::Dot(lightPos, lightsManager->normalYClusters[j]) < lightRadius) {
                yminidx =  j - 1;
                break;
            }
        }

        for(int j = yminidx + 2; j <= lightsManager->ySlice; ++j) {
            if (dmVMath::Dot(lightPos, lightsManager->normalYClusters[j]) < -lightRadius) {
                ymaxidx = j;
                break;
            }
        }

       // dmLogInfo("x[%d %d] y[%d %d] z[%d %d]",xminidx,xmaxidx,yminidx,ymaxidx,zminidx,zmaxidx);


        xminidx = fmax(0,xminidx);
        xmaxidx = fmin(xmaxidx,lightsManager->xSlice);
        yminidx = fmax(0,yminidx);
        ymaxidx = fmin(ymaxidx,lightsManager->ySlice);

        if(xminidx==xmaxidx || yminidx==ymaxidx){
            continue;
        }
        lightsManager->debugVisibleLights++;

        for (int z = zminidx; z < zmaxidx; ++z) {
            int zOffset = z * lightsManager->xSlice * lightsManager->ySlice;
            for (int y = yminidx; y < ymaxidx; ++y) {
                int yOffset = y * lightsManager->xSlice;
                int zyOffset = zOffset + yOffset;
                for (int x = xminidx; x < xmaxidx; ++x) {
                    int id = zyOffset + x;
                    LightCluster& cluster = lightsManager->clusters[id];

                    if (cluster.numLights < lightsManager->maxLightsPerCluster) {
                        memcpy(cluster.currentLightStart, l->encodedIndex, 4);
                        cluster.numLights++;
                        cluster.currentLightStart +=4;
                    } else {
                        dmLogWarning("Cluster %d already has the maximum number of lights", id);
                    }
                }
            }
        }


    }


    //dmLogInfo("Lights all:%d. Visible:%d",lightsManager->lightsInWorld.Size(), lightsManager->lightsVisibleInWorld.Size());

    uint8_t* values = 0x0;
    uint32_t stride = 0;
    dmBuffer::Result dataResult = dmBuffer::GetStream(lightsManager->textureBuffer, HASH_RGBA, (void**)&values, 0x0, 0x0, &stride);
    if (dataResult != dmBuffer::RESULT_OK) {
        luaL_error(L,"can't get stream for lights texture");
    }

    for (int i = 0; i < lightsManager->lightsVisibleInWorld.Size(); ++i) {
        Light* light = lightsManager->lightsVisibleInWorld[i];
        if(light->dirty){
            LightWriteToBuffer(light, values+light->index*LIGHT_PIXELS*stride, stride);
            light->dirty = false;
        }
    }

    for(int i=0;i<lightsManager->totalClusters;++i){
        //write num lights of cluster
        LightCluster& cluster = lightsManager->clusters[i];
        memcpy(cluster.clusterStart, lightsManager->encodedClusterLights+cluster.numLights*4, 4);
        //reset after write data
        cluster.numLights = 0;
        cluster.currentLightStart = cluster.clusterStart+4;
    }

     dmBuffer::UpdateContentVersion(lightsManager->textureBuffer);


    // Update clusters

    //Update texture
    // Step 1: Push the 'resource.set_texture' function onto the stack
    lua_getglobal(L, "resource");
    lua_getfield(L, -1, "set_texture");
    lua_remove(L, -2); // Remove 'resource' table from the stack

    // Step 2: Push the arguments
    dmScript::PushHash(L, lightsManager->texturePath);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lightsManager->textureParamsRef);
    dmScript::LuaHBuffer luabuf(lightsManager->textureBuffer, dmScript::OWNER_C);
    PushBuffer(L, luabuf);

    // Step 3: Call the function
    if (lua_pcall(L, 3, 0, 0)!= 0) {
        // Handle error
        const char* error_msg = lua_tostring(L, -1);
        lua_pop(L, 1); // Pop error message
        luaL_error(L, "can't set light texture. %s",error_msg);
    }
}

inline void LightsManagerCalculateClusters(LightsManager* lightsManager){
    lightsManager->halfY  = tan(lightsManager->cameraFov * 0.5);
    lightsManager->ylengthPerCluster  =  lightsManager->halfY*2.0 / lightsManager->ySlice;
    lightsManager->xlengthPerCluster  =  lightsManager->halfY*2.0 / lightsManager->ySlice * lightsManager->cameraAspect;
    lightsManager->zlengthPerCluster  =  (lightsManager->cameraFar - lightsManager->cameraNear) / lightsManager->zSlice;

    delete[] lightsManager->normalXClusters;
    delete[] lightsManager->normalYClusters;
    lightsManager->normalXClusters = new dmVMath::Vector3[lightsManager->xSlice+1];
    lightsManager->normalYClusters = new dmVMath::Vector3[lightsManager->ySlice+1];

    float ystart = -lightsManager->halfY;
    float xstart = -lightsManager->halfY * lightsManager->cameraAspect;

    for(int i = 0; i <= lightsManager->xSlice; ++i) {
        dmVMath::Vector3 norm2 = getNormalComponents(xstart + lightsManager->xlengthPerCluster * i);
        lightsManager->normalXClusters[i] = dmVMath::Vector3(norm2.getX(), 0, norm2.getY());
    }

    for(int i = 0; i <= lightsManager->ySlice; ++i) {
        dmVMath::Vector3 norm2 = getNormalComponents(ystart + lightsManager->ylengthPerCluster * i);
        lightsManager->normalYClusters[i] = dmVMath::Vector3(0, norm2.getX(), norm2.getY());
    }

}

inline void LightsManagerSetCameraFov(LightsManager* lightsManager,float cameraFov){
    assert(lightsManager->inited);
    if(lightsManager->cameraFov!=cameraFov){
        lightsManager->cameraFov =  cameraFov;
        LightsManagerCalculateClusters(lightsManager);
    }
}

inline void LightsManagerSetCameraAspect(LightsManager* lightsManager,float cameraAspect){
    assert(lightsManager->inited);
    if(lightsManager->cameraAspect!=cameraAspect){
        lightsManager->cameraAspect =  cameraAspect;
        LightsManagerCalculateClusters(lightsManager);
    }
}

inline void LightsManagerSetCameraNear(LightsManager* lightsManager,float cameraNear){
    assert(lightsManager->inited);
    if(lightsManager->cameraNear!=cameraNear){
        lightsManager->cameraNear =  cameraNear;
        LightsManagerCalculateClusters(lightsManager);
    }
}

inline void LightsManagerSetCameraFar(LightsManager* lightsManager,float cameraFar){
    assert(lightsManager->inited);
    if(lightsManager->cameraFar!=cameraFar){
        lightsManager->cameraFar =  cameraFar;
        LightsManagerCalculateClusters(lightsManager);
    }
}


extern LightsManager g_lightsManager;// add g_lightsManager in extension.cpp


static int LuaLightsManagerInit(lua_State* L){
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

    g_lightsManager.init(L, numLights, xSlice, ySlice, zSlice, maxLightsPerCluster);

    dmLogInfo("LightsManager inited.Lights:%d xSlice:%d ySlice:%d zSlice:%d maxLightsPerCluster:%d" ,
        numLights, xSlice, ySlice, zSlice, maxLightsPerCluster);
    return 0;
}

static int LuaLightsManagerCreateLight(lua_State* L){
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

    g_lightsManager.lightsInWorld.Push(light);


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
static int LuaLightsManagerDestroyLight(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }

    LuaLightUserData* userData = (LuaLightUserData*)luaL_checkudata(L, 1, LIGHT_META);
    if (!userData->valid) {return 0;}

    userData->valid = false;

    LightReset(userData->light);
    g_lightsManager.lightsPool.Push(userData->light);

    return 0;
}
static int LuaLightsManagerUpdateLights(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 0);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }

    LightsManagerUpdateLights(L,&g_lightsManager);

    return 0;
}


static int LuaLightsManagerGetTexturePath(lua_State* L){
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }
    dmScript::PushHash(L, g_lightsManager.texturePath);

    return 1;
}

static int LuaLightsManagerSetTexturePath(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }
    g_lightsManager.texturePath = dmScript::CheckHash(L,1);

    return 0;
}

static int LuaLightsManagerGetTextureSize(lua_State* L){
    DM_LUA_STACK_CHECK(L, 2);
    check_arg_count(L, 0);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }
    lua_pushnumber(L, g_lightsManager.textureWidth);
    lua_pushnumber(L, g_lightsManager.textureHeight);

    return 2;
}


static int LuaLightsManagerSetFrustumMatrix(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }
    g_lightsManager.frustum.SetMatrix(*dmScript::CheckMatrix4(L,1));
    return 0;
}

static int LuaLightsManagerSetViewMatrix(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    if(!g_lightsManager.inited){
        return DM_LUA_ERROR("LightsManager not inited");
    }
    g_lightsManager.view = *dmScript::CheckMatrix4(L,1);
    return 0;
}

static int LuaLightsManagerSetCameraAspect(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LightsManagerSetCameraAspect(&g_lightsManager,luaL_checknumber(L, 1));
    return 0;
}

static int LuaLightsManagerSetCameraFov(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LightsManagerSetCameraFov(&g_lightsManager,luaL_checknumber(L, 1));
    return 0;
}

static int LuaLightsManagerSetCameraFar(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LightsManagerSetCameraFar(&g_lightsManager,luaL_checknumber(L, 1));
    return 0;
}

static int LuaLightsManagerSetCameraNear(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    check_arg_count(L, 1);
    LightsManagerSetCameraNear(&g_lightsManager,luaL_checknumber(L, 1));
    return 0;
}

static int LuaLightsManagerGetMaxLights(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.numLights);
    return 1;
}
static int LuaLightsManagerGetMaxRadius(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,LIGHT_RADIUS_MAX);
    return 1;
}
static int LuaLightsManagerGetBordersX(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 2);
    check_arg_count(L, 0);
    lua_pushnumber(L,LIGHT_MIN_POSITION);
    lua_pushnumber(L,LIGHT_MAX_POSITION);
    return 2;
}
static int LuaLightsManagerGetBordersY(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 2);
    check_arg_count(L, 0);
    lua_pushnumber(L,LIGHT_MIN_POSITION);
    lua_pushnumber(L,LIGHT_MAX_POSITION);
    return 2;
}

static int LuaLightsManagerGetBordersZ(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 2);
    check_arg_count(L, 0);
    lua_pushnumber(L,LIGHT_MIN_POSITION);
    lua_pushnumber(L,LIGHT_MAX_POSITION);
    return 2;
}

static int LuaLightsManagerGetXSlice(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.xSlice);
    return 1;
}

static int LuaLightsManagerGetYSlice(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.ySlice);
    return 1;
}

static int LuaLightsManagerGetZSlice(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.zSlice);
    return 1;
}


static int LuaLightsManagerGetLightsPerCluster(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.maxLightsPerCluster);
    return 1;
}
static int LuaLightsManagerGetInWorldCount(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.lightsInWorld.Size());
    return 1;
}
static int LuaLightsManagerGetInWorldVisibleCount(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 0);
    lua_pushnumber(L,g_lightsManager.debugVisibleLights);
    return 1;
}


}  // namespace IlluminationLights


#endif
