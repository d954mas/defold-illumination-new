#define EXTENSION_NAME Illumination
#define LIB_NAME "Illumination"
#define MODULE_NAME "illumination"

#include <dmsdk/sdk.h>

#include "utils.h"

static float Fract(float f){
    return f - floor(f);
}

static dmVMath::Vector4 EncodeFloatRGBA(float v, float min, float max){
    //if (v<MIN_BORDER){MIN_BORDER = v;}
    //if (v>MAX_BORDER){MAX_BORDER = v;}
    assert(v>=min);
    assert(v<max);
    assert(max>min);
    v = (v- min)/(max-min);
    assert(v>=0.0);
    assert(v<=1.0);
    dmVMath::Vector4 enc = dmVMath::Vector4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc.setX(Fract(enc.getX()));
    enc.setY(Fract(enc.getY()));
    enc.setZ(Fract(enc.getZ()));
    enc.setW(Fract(enc.getW()));
    //enc = enc - dmVMath::Vector4(enc.getY()*1.0/255.0,enc.getZ()*1.0/255.0,enc.getW()*1.0/255.0,0.0);
    return enc;
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


// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
    {"float_to_rgba", FloatToRGBALua},
    {"fill_stream_uint8",FillStreamUint8},

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