#define EXTENSION_NAME Illumination
#define LIB_NAME "Illumination"
#define MODULE_NAME "illumination"

#include <dmsdk/sdk.h>

#include "utils.h"
#include "frustum_cull.h"
#include "lights.h"

using namespace IlluminationLights;

namespace IlluminationLights {
    LightsManager g_lightsManager;
}


static int FloatToRGBALua(lua_State* L){
    DM_LUA_STACK_CHECK(L, 4);
    check_arg_count(L, 3);
    float value = lua_tonumber(L,1);
    float min = lua_tonumber(L,2);
    float max = lua_tonumber(L,3);
    if (value < min) return DM_LUA_ERROR("value < min");
    if (value > max) return DM_LUA_ERROR("value > max");
    if (min >= max) return DM_LUA_ERROR("min >= max");

    dmVMath::Vector4 vec4 = EncodeFloatRGBA(value,min,max);
    lua_pushnumber(L,vec4.getX());
    lua_pushnumber(L,vec4.getY());
    lua_pushnumber(L,vec4.getZ());
    lua_pushnumber(L,vec4.getW());

    return 4;
}

static int FillStreamUint8(lua_State* L){
    DM_LUA_STACK_CHECK(L, 0);
     check_arg_count(L, 5);
    int index = lua_tonumber(L,1)-1; //c array start from 0
    if (index<0){
        return DM_LUA_ERROR("index is <0")
    }
    dmBuffer::HBuffer buffer = dmScript::CheckBufferUnpack(L, 2);
    dmhash_t streamName = dmScript::CheckHashOrString(L,3);
    int componentsSize = lua_tonumber(L,4);
    if (!lua_istable(L, 5)) {
         return DM_LUA_ERROR("data not table");
    }

    uint8_t* values = 0x0;
    uint32_t sizeBuffer = 0;
    uint32_t components = 0;
    uint32_t stride = 0;
    dmBuffer::Result dataResult = dmBuffer::GetStream(buffer, streamName, (void**)&values, &sizeBuffer, &components, &stride);
    if (dataResult != dmBuffer::RESULT_OK) {
       return DM_LUA_ERROR("can't get stream");
    }

    if (components!=componentsSize){
         return DM_LUA_ERROR("stream have: %d components. Need %d", components, componentsSize);
    }

    int size = luaL_getn(L, 5);
    if (size % components != 0){
      return DM_LUA_ERROR("bad size:%d", size);
    }
    if (index + size/components>=sizeBuffer){
        return DM_LUA_ERROR("buffer not enough size");
    }

    values += index * stride;

    for (int i=0; i<size/components; ++i) {
        for (int j=0;j<components;++j){
            lua_rawgeti(L, 5, i*components+j+1);
            values[j] = lua_tonumber(L,-1)*255.0;
            lua_pop(L,1);
        }
        values += stride;
    }
    dmBuffer::UpdateContentVersion(buffer);
    return 0;
}

static int FrustumIsBoxVisibleLua(lua_State* L){
    DM_LUA_STACK_CHECK(L, 1);
    check_arg_count(L, 7);
    dmVMath::Matrix4* m = dmScript::CheckMatrix4(L, 1);
    dmVMath::Vector3 min(luaL_checknumber(L, 2),luaL_checknumber(L, 3),luaL_checknumber(L, 4));
    dmVMath::Vector3 max(luaL_checknumber(L, 5),luaL_checknumber(L, 6),luaL_checknumber(L, 7));

    Frustum frustum = Frustum(*m);

    const bool visible = frustum.IsBoxVisible(min, max);

    lua_pushboolean(L, visible);
    return 1;
}


// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
    {"float_to_rgba", FloatToRGBALua},
    {"fill_stream_uint8",FillStreamUint8},
    {"frustum_is_box_visible",FrustumIsBoxVisibleLua},


    {"lights_init", LuaLightsManagerInit},
    {"lights_get_texture_path", LuaLightsManagerGetTexturePath},
    {"lights_set_texture_path", LuaLightsManagerSetTexturePath},
    {"lights_set_frustum", LuaLightsManagerSetFrustumMatrix},
    {"lights_set_view", LuaLightsManagerSetViewMatrix},
    {"lights_set_camera_fov", LuaLightsManagerSetCameraFov},
    {"lights_set_camera_far", LuaLightsManagerSetCameraFar},
    {"lights_set_camera_near", LuaLightsManagerSetCameraNear},
    {"lights_set_camera_aspect", LuaLightsManagerSetCameraAspect},
    {"lights_get_texture_size", LuaLightsManagerGetTextureSize},
    {"lights_get_max_lights", LuaLightsManagerGetMaxLights},
    {"lights_get_max_radius", LuaLightsManagerGetMaxRadius},
    {"lights_get_borders_x", LuaLightsManagerGetBordersX},
    {"lights_get_borders_y", LuaLightsManagerGetBordersY},
    {"lights_get_borders_z", LuaLightsManagerGetBordersZ},
    {"lights_get_x_slice", LuaLightsManagerGetXSlice},
    {"lights_get_y_slice", LuaLightsManagerGetYSlice},
    {"lights_get_z_slice", LuaLightsManagerGetZSlice},
    {"lights_get_lights_per_cluster", LuaLightsManagerGetLightsPerCluster},
    {"lights_update", LuaLightsManagerUpdateLights},
    {"light_create", LuaLightsManagerCreateLight},
    {"light_destroy", LuaLightsManagerDestroyLight},
    {"lights_get_all_count", LuaLightsManagerGetInWorldCount},
    {"lights_get_visible_count", LuaLightsManagerGetInWorldVisibleCount},

    {0, 0}

};

static void LuaInit(lua_State *L) {
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams *params) { return dmExtension::RESULT_OK; }
static dmExtension::Result InitializeMyExtension(dmExtension::Params *params) {
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams *params) { return dmExtension::RESULT_OK; }

static dmExtension::Result FinalizeMyExtension(dmExtension::Params *params) { return dmExtension::RESULT_OK; }

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, 0, 0, FinalizeMyExtension)