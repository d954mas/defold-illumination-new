local CLASS = require "illumination.middleclass"

local V4 = vmath.vector4()
local VIEW_DIRECTION = vmath.vector3()
local VIEW_RIGHT = vmath.vector3()
local VIEW_UP = vmath.vector3()

local TEMP_V4 = vmath.vector4()

local V_UP = vmath.vector3(0, 1, 0)

local POINTS_CUBE = {
	vmath.vector4(-1.0, -1.0, 1.0, 1.0),
	vmath.vector4(-1.0, 1.0, 1.0, 1.0),
	vmath.vector4(1.0, 1.0, 1.0, 1.0),
	vmath.vector4(1.0, -1.0, 1.0, 1.0),
	vmath.vector4(-1.0, -1.0, -1.0, 1.0),
	vmath.vector4(-1.0, 1.0, -1.0, 1.0),
	vmath.vector4(1.0, 1.0, -1.0, 1.0),
	vmath.vector4(1.0, -1.0, -1.0, 1.0)
};

local POINTS_CUBE_RESULT = {
	vmath.vector4(-1.0, -1.0, 1.0, 1.0),
	vmath.vector4(-1.0, 1.0, 1.0, 1.0),
	vmath.vector4(1.0, 1.0, 1.0, 1.0),
	vmath.vector4(1.0, -1.0, 1.0, 1.0),
	vmath.vector4(-1.0, -1.0, -1.0, 1.0),
	vmath.vector4(-1.0, 1.0, -1.0, 1.0),
	vmath.vector4(1.0, 1.0, -1.0, 1.0),
	vmath.vector4(1.0, -1.0, -1.0, 1.0)
};

local V1 = vmath.vector3(0)
local V2 = vmath.vector3(0)

local HASH_DRAW_LINE = hash("draw_line")
local MSD_DRAW_LINE_COLOR = vmath.vector4(1)
local MSD_DRAW_LINE_COLOR_AABB = vmath.vector4(1, 0, 0, 1)

local MSD_DRAW_LINE = {
	start_point = V1,
	end_point = V2,
	color = MSD_DRAW_LINE_COLOR
}

local function draw_aabb3d(x1, y1, z1, x2, y2, z2, color)
	MSD_DRAW_LINE_COLOR.x = color.x
	MSD_DRAW_LINE_COLOR.y = color.y
	MSD_DRAW_LINE_COLOR.z = color.z
	MSD_DRAW_LINE_COLOR.w = color.w

	--bottom
	V1.x, V1.y, V1.z = x1, y1, z1
	V2.x, V2.y, V2.z = x1, y1, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x1, y1, z1
	V2.x, V2.y, V2.z = x2, y1, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x2, y1, z2
	V2.x, V2.y, V2.z = x1, y1, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x2, y1, z2
	V2.x, V2.y, V2.z = x2, y1, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	--top
	V1.x, V1.y, V1.z = x1, y2, z1
	V2.x, V2.y, V2.z = x1, y2, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x1, y2, z1
	V2.x, V2.y, V2.z = x2, y2, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x2, y2, z2
	V2.x, V2.y, V2.z = x1, y2, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = x2, y2, z2
	V2.x, V2.y, V2.z = x2, y2, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	--edges

	V1.x, V1.y, V1.z = x1, y1, z1
	V2.x, V2.y, V2.z = x1, y2, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)
	V1.x, V1.y, V1.z = x1, y1, z2
	V2.x, V2.y, V2.z = x1, y2, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)
	V1.x, V1.y, V1.z = x2, y1, z1
	V2.x, V2.y, V2.z = x2, y2, z1
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)
	V1.x, V1.y, V1.z = x2, y1, z2
	V2.x, V2.y, V2.z = x2, y2, z2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

end

local function draw_cube(points, color)
	MSD_DRAW_LINE_COLOR.x = color.x
	MSD_DRAW_LINE_COLOR.y = color.y
	MSD_DRAW_LINE_COLOR.z = color.z
	MSD_DRAW_LINE_COLOR.w = color.w

	local p1 = points[1]
	local p2 = points[2]
	local p3 = points[3]
	local p4 = points[4]
	local p5 = points[5]
	local p6 = points[6]
	local p7 = points[7]
	local p8 = points[8]

	--far
	V1.x, V1.y, V1.z = p1.x, p1.y, p1.z
	V2.x, V2.y, V2.z = p2.x, p2.y, p2.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p2.x, p2.y, p2.z
	V2.x, V2.y, V2.z = p3.x, p3.y, p3.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p3.x, p3.y, p3.z
	V2.x, V2.y, V2.z = p4.x, p4.y, p4.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p4.x, p4.y, p4.z
	V2.x, V2.y, V2.z = p1.x, p1.y, p1.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)


	--near
	V1.x, V1.y, V1.z = p5.x, p5.y, p5.z
	V2.x, V2.y, V2.z = p6.x, p6.y, p6.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p6.x, p6.y, p6.z
	V2.x, V2.y, V2.z = p7.x, p7.y, p7.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p7.x, p7.y, p7.z
	V2.x, V2.y, V2.z = p8.x, p8.y, p8.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p8.x, p8.y, p8.z
	V2.x, V2.y, V2.z = p5.x, p5.y, p5.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	--edges
	V1.x, V1.y, V1.z = p1.x, p1.y, p1.z
	V2.x, V2.y, V2.z = p5.x, p5.y, p5.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p2.x, p2.y, p2.z
	V2.x, V2.y, V2.z = p6.x, p6.y, p6.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p3.x, p3.y, p3.z
	V2.x, V2.y, V2.z = p7.x, p7.y, p7.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	V1.x, V1.y, V1.z = p4.x, p4.y, p4.z
	V2.x, V2.y, V2.z = p8.x, p8.y, p8.z
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)

	--center
	MSD_DRAW_LINE.color.y = 1
	V1.x, V1.y, V1.z = (p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2
	V2.x, V2.y, V2.z = (p3.x + p4.x) / 2, (p3.y + p4.y) / 2, (p3.z + p4.z) / 2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)
	V1.x, V1.y, V1.z = (p2.x + p3.x) / 2, (p2.y + p3.y) / 2, (p2.z + p3.z) / 2
	V2.x, V2.y, V2.z = (p1.x + p4.x) / 2, (p1.y + p4.y) / 2, (p1.z + p4.z) / 2
	msg.post("@render:", HASH_DRAW_LINE, MSD_DRAW_LINE)
end

local function clamp(x, min, max)
	return x < min and min or (x > max and max or x)
end

local function create_depth_buffer(w, h)
	local color_params = {
		-- format     = render.FORMAT_RGBA,
		format = render.FORMAT_RGBA,
		width = w,
		height = h,
		min_filter = render.FILTER_NEAREST,
		mag_filter = render.FILTER_NEAREST,
		u_wrap = render.WRAP_CLAMP_TO_EDGE,
		v_wrap = render.WRAP_CLAMP_TO_EDGE }

	local depth_params = {
		format = render.FORMAT_DEPTH,
		width = w,
		height = h,
		min_filter = render.FILTER_NEAREST,
		mag_filter = render.FILTER_NEAREST,
		u_wrap = render.WRAP_CLAMP_TO_EDGE,
		v_wrap = render.WRAP_CLAMP_TO_EDGE }

	return render.render_target("shadow_buffer", { [render.BUFFER_COLOR_BIT] = color_params, [render.BUFFER_DEPTH_BIT] = depth_params })
end

local function create_empty_shadow_buffer()
	local color_params = {
		-- format     = render.FORMAT_RGBA,
		format = render.FORMAT_RGBA,
		width = 1,
		height = 1,
		min_filter = render.FILTER_NEAREST,
		mag_filter = render.FILTER_NEAREST,
		u_wrap = render.WRAP_CLAMP_TO_EDGE,
		v_wrap = render.WRAP_CLAMP_TO_EDGE }

	return render.render_target("empty_shadow_buffer", { [render.BUFFER_COLOR_BIT] = color_params })
end

---@class Lights
local Lights = CLASS("lights")

function Lights:initialize()
	self.constants = {}
	self.shadow_params = vmath.vector4()
	self.ambient_color = vmath.vector4()
	self.sunlight_color = vmath.vector4()
	self.shadow_color = vmath.vector4()
	self.fog = vmath.vector4()
	self.fog_color = vmath.vector4()
	self.frustum = nil
	self.frustum_inv = vmath.matrix4()
	self.view = vmath.matrix4()

	self.light_texture_data = vmath.vector4()
	self.lights_data = vmath.vector4(0, 0, 0, 0)
	self.lights_data2 = vmath.vector4()
	self.clusters_data = vmath.vector4() -- x_slices, y_slices, z_slices, max_lights_per_cluster
	self.screen_size = vmath.vector4()

	self.debug = false
	self.enable_lights = false
	self.enable_shadow = false

	self.shadow = {
		-- Size of shadow map. Select value from: 1024/2048/4096. More is better quality.
		BUFFER_RESOLUTION = 2048,
		-- MIN AND MAX VALUE FOR PROJECTION
		SHADOW_MAX_DISTANCE = 10,
		NEAR = 0.001,
		FAR = 30,

		pred = nil,
		light_projection = nil,
		light_projection_base = nil,
		bias_matrix = vmath.matrix4(),
		light_matrix = vmath.matrix4(),
		constants = render.constant_buffer(),
		sun_position = vmath.vector3(-10, 0, 0), --delta to root position
		root_position = vmath.vector3(0), --player position
		light_position = vmath.vector3(0), --root_position + sun_position
		light_transform = vmath.matrix4(),
		rt = nil,
		rt_no_shadow = nil,
		no_shadow_clear_color = { [render.BUFFER_COLOR_BIT] = vmath.vector4(1),
								  [render.BUFFER_DEPTH_BIT] = 1, [render.BUFFER_STENCIL_BIT] = 0 },
		draw_shadow_opts = { frustum = vmath.matrix4(), frustum_planes = render.FRUSTUM_PLANES_ALL },
		draw_transient = { transient = { render.BUFFER_DEPTH_BIT } },
		draw_clear = { [render.BUFFER_COLOR_BIT] = vmath.vector4(1, 1, 1, 1), [render.BUFFER_DEPTH_BIT] = 1 }
	}

	self.shadow_params.x = self.shadow.BUFFER_RESOLUTION
	self.shadow_params.y = 0.008

	---@class LightsData
	self.lights = {
		in_world = {},
	}
end

function Lights:init(shadow_texture_resource, data_texture_resource)
	assert(shadow_texture_resource)
	assert(data_texture_resource)
	self.light_texture_data.x, self.light_texture_data.y = illumination.lights_get_texture_size()

	self.lights_data.x = illumination.lights_get_max_lights()
	self.lights_data.y = illumination.lights_get_max_radius()
	self.lights_data.z, self.lights_data.w = illumination.lights_get_borders_x()

	self.lights_data2.x, self.lights_data2.y = illumination.lights_get_borders_y()
	self.lights_data2.z, self.lights_data2.w = illumination.lights_get_borders_z()

	self.clusters_data.x = illumination.lights_get_x_slice()
	self.clusters_data.y = illumination.lights_get_y_slice()
	self.clusters_data.z = illumination.lights_get_z_slice_for_shader()
	self.clusters_data.w = illumination.lights_get_lights_per_cluster()

	for _, constant in ipairs(self.constants) do
		constant.light_texture_data = self.light_texture_data
		constant.lights_data = self.lights_data
		constant.lights_data2 = self.lights_data2
		constant.clusters_data = self.clusters_data
	end

	self.data_texture_resource = data_texture_resource
	self.data_texture_empty_params = {
		width = 1,
		height = 1,
		type = resource.TEXTURE_TYPE_2D,
		format = resource.TEXTURE_FORMAT_RGBA,
		num_mip_maps = 1
	}
	self.data_texture_empty_buffer = buffer.create(1, { { name = hash("rgba"), type = buffer.VALUE_TYPE_UINT8, count = 4 } })
	local stream = buffer.get_stream(self.data_texture_empty_buffer, hash("rgba"))
	stream[1], stream[2], stream[3], stream[4] = 0, 0, 0, 0

	illumination.lights_set_texture_path(data_texture_resource)

	self:set_enable_shadows(true)
	self:set_enable_lights(true)
end

function Lights:on_resize(w, h)
	self.screen_size.x, self.screen_size.y = w, h
	for _, constant in ipairs(self.constants) do
		constant.screen_size = self.screen_size
	end
end

function Lights:set_enable_shadows(enable)
	if self.enable_shadow ~= enable then
		self.enable_shadow = enable
	end
end

function Lights:set_enable_lights(enable)
	if self.enable_lights ~= enable then
		self.enable_lights = enable
		self:update_lights_texture()
	end
end

function Lights:update_lights_texture()
	if not self.enable_lights then
		resource.set_texture(self.data_texture_resource, self.data_texture_empty_params, self.data_texture_empty_buffer)
	end
end

function Lights:draw_debug()
	if self.debug then
		render.draw(self.debug_predicate)
	end
end

function Lights:draw_shadow_debug()
	render.enable_texture(0, self.shadow.rt or self.shadow.rt_no_shadow, render.BUFFER_COLOR_BIT)
	render.draw(self.debug_shadow_predicate)
	render.disable_texture(0)
end
function Lights:draw_data_lights_debug()
	render.draw(self.debug_data_lights_predicate)
end

function Lights:draw_debug_planes()
	for idx, point in ipairs(POINTS_CUBE) do
		local result = POINTS_CUBE_RESULT[idx]
		xmath.matrix_mul_v4(result, self.frustum_inv, point)
		xmath.div(result, result, result.w)
		result.w = 1
	end

	draw_cube(POINTS_CUBE_RESULT, vmath.vector4(1, 0, 0, 1))
end

function Lights:draw_begin()
	render.enable_texture(1, self.shadow.rt or self.shadow.rt_no_shadow, render.BUFFER_COLOR_BIT)
end

function Lights:draw_finish()
	render.disable_texture(1)
end

function Lights:set_debug(debug)
	self.debug = debug
end

function Lights:add_constants(constant)
	table.insert(self.constants, constant)
	constant.sunlight_color = self.sunlight_color
	constant.shadow_color = self.shadow_color
	constant.ambient_color = self.ambient_color

	constant.fog = self.fog
	constant.fog_color = self.fog_color
	constant.shadow_params = self.shadow_params
	constant.lights_data = self.lights_data
	constant.lights_data2 = self.lights_data2
	constant.clusters_data = self.clusters_data
	constant.light_texture_data = self.light_texture_data
	constant.screen_size = self.screen_size

	V4.x = self.shadow.sun_position.x
	V4.y = self.shadow.sun_position.y
	V4.z = self.shadow.sun_position.z
	V4.w = 0
	constant.sun_position = V4
end

---@param render Render
function Lights:set_render(render_obj)
	illumination.lights_init(1024, 15, 15, 15, 256)

	self.render = assert(render_obj)

	-- all objects that have to cast shadows
	self.shadow.pred = render.predicate({ "shadow" })

	self.shadow.light_projection_base = vmath.matrix4_orthographic(-1, 1,
			-1, 1, self.shadow.NEAR, self.shadow.FAR)
	self.shadow.light_projection = vmath.matrix4_orthographic(-1, 1, -1, 1, self.shadow.NEAR, self.shadow.FAR)

	self.shadow.bias_matrix.c0 = vmath.vector4(0.5, 0.0, 0.0, 0.0)
	self.shadow.bias_matrix.c1 = vmath.vector4(0.0, 0.5, 0.0, 0.0)
	self.shadow.bias_matrix.c2 = vmath.vector4(0.0, 0.0, 0.5, 0.0)
	self.shadow.bias_matrix.c3 = vmath.vector4(0.5, 0.5, 0.5, 1.0)

	self.debug_predicate = render.predicate({ "illumination_debug" })
	self.debug_shadow_predicate = render.predicate({ "shadow_debug" })
	self.debug_data_lights_predicate = render.predicate({ "data_lights_debug" })

	self:reset()
end

function Lights:reset()
	if (not self.render) then return end
	self:set_sunlight_color(1, 1, 1)
	self:set_sunlight_color_intensity(0.4)
	self:set_shadow_color(0.5, 0.5, 0.5)
	self:set_shadow_color_intensity(1)

	self:set_ambient_color(1, 1, 1)
	self:set_ambient_color_intensity(0.6)

	self:set_fog(5, 15, 0.9)
	self:set_fog_color(0.85, 0.8, 0.9)

	self:set_sun_position(-4, 10, 0)
end

function Lights:set_ambient_color(r, g, b)
	self.ambient_color.x, self.ambient_color.y, self.ambient_color.z = r, g, b
	for _, constant in ipairs(self.constants) do
		constant.ambient_color = self.ambient_color
	end
end

function Lights:set_ambient_color_intensity(intensity)
	self.ambient_color.w = intensity
	for _, constant in ipairs(self.constants) do
		constant.ambient_color = self.ambient_color
	end
end

function Lights:set_sunlight_color(r, g, b)
	self.sunlight_color.x, self.sunlight_color.y, self.sunlight_color.z = r, g, b
	for _, constant in ipairs(self.constants) do
		constant.sunlight_color = self.sunlight_color
	end
end

function Lights:set_sunlight_color_intensity(intensity)
	self.sunlight_color.w = intensity
	for _, constant in ipairs(self.constants) do
		constant.sunlight_color = self.sunlight_color
	end
end

function Lights:set_shadow_color(r, g, b)
	self.shadow_color.x, self.shadow_color.y, self.shadow_color.z = 1 - r, 1 - g, 1 - b
	for _, constant in ipairs(self.constants) do
		constant.shadow_color = self.shadow_color
	end
end

function Lights:set_shadow_color_intensity(intensity)
	self.shadow_color.w = intensity
	for _, constant in ipairs(self.constants) do
		constant.shadow_color = self.shadow_color
	end
end

function Lights:set_fog(min, max, intensity)
	self.fog.x, self.fog.y, self.fog.w = min, max, intensity
	for _, constant in ipairs(self.constants) do
		constant.fog = self.fog
	end
end

function Lights:set_fog_color(r, g, b)
	self.fog_color.x, self.fog_color.y, self.fog_color.z = r, g, b
	for _, constant in ipairs(self.constants) do
		constant.fog_color = self.fog_color
	end
end

function Lights:set_sun_position(x, y, z)
	self.shadow.sun_position.x = x
	self.shadow.sun_position.y = y
	self.shadow.sun_position.z = z

	xmath.add(self.shadow.light_position, self.shadow.root_position, self.shadow.sun_position)

	V4.x = self.shadow.sun_position.x
	V4.y = self.shadow.sun_position.y
	V4.z = self.shadow.sun_position.z
	V4.w = 0
	for _, constant in ipairs(self.constants) do
		constant.sun_position = V4
	end
end

function Lights:set_camera(x, y, z)

	self.shadow.root_position.x = x
	self.shadow.root_position.y = y
	self.shadow.root_position.z = z

	xmath.add(self.shadow.light_position, self.shadow.root_position, self.shadow.sun_position)

	xmath.sub(VIEW_DIRECTION, self.shadow.root_position, self.shadow.light_position)
	xmath.normalize(VIEW_DIRECTION, VIEW_DIRECTION)
	xmath.cross(VIEW_RIGHT, VIEW_DIRECTION, V_UP)
	xmath.normalize(VIEW_RIGHT, VIEW_RIGHT)
	xmath.cross(VIEW_UP, VIEW_RIGHT, VIEW_DIRECTION)
	xmath.normalize(VIEW_UP, VIEW_UP)

	xmath.matrix_look_at(self.shadow.light_transform, self.shadow.light_position, self.shadow.root_position, VIEW_UP)
	xmath.matrix_mul(self.shadow.light_matrix, self.shadow.bias_matrix, self.shadow.light_projection_base)
	xmath.matrix_mul(self.shadow.light_matrix, self.shadow.light_matrix, self.shadow.light_transform)

	for idx, point in ipairs(POINTS_CUBE) do
		local result = POINTS_CUBE_RESULT[idx]
		xmath.matrix_mul_v4(result, self.frustum_inv, point)
		xmath.div(result, result, result.w)
		result.w = 1
	end

	local min_x, max_x = math.huge, -math.huge
	local min_y, max_y = math.huge, -math.huge
	for _, point in ipairs(POINTS_CUBE_RESULT) do
		xmath.matrix_mul_v4(TEMP_V4, self.shadow.light_matrix, point)
		local px, py = TEMP_V4.x / TEMP_V4.w, TEMP_V4.y / TEMP_V4.w
		if px < min_x then min_x = px end
		if px > max_x then max_x = px end
		if py < min_y then min_y = py end
		if py > max_y then max_y = py end
	end

	min_x = -1 + min_x * 2
	max_x = -1 + max_x * 2
	min_y = -1 + min_y * 2
	max_y = -1 + max_y * 2

	print("shadow uv:x[" .. min_x .. " " .. max_x .. "] y[" .. min_y .. " " .. max_y .. "] w:" .. max_x - min_x .. " h:" .. max_y - min_y)


	--min_x = self.shadow.PROJECTION_X1
	--max_x = self.shadow.PROJECTION_X2
	--min_y = self.shadow.PROJECTION_Y1
	--max_y = self.shadow.PROJECTION_Y2


	xmath.matrix4_orthographic(self.shadow.light_projection, min_x, max_x,
			min_y, max_y, self.shadow.NEAR, self.shadow.FAR)

	xmath.matrix_mul(self.shadow.light_matrix, self.shadow.bias_matrix, self.shadow.light_projection)
	xmath.matrix_mul(self.shadow.light_matrix, self.shadow.light_matrix, self.shadow.light_transform)

	--local mtx_light = self.shadow.bias_matrix * self.shadow.light_projection * self.shadow.light_transform
	for _, constant in ipairs(self.constants) do
		constant.mtx_light = self.shadow.light_matrix
	end
end

function Lights:render_shadows()
	if (self.enable_shadow) then
		if not self.shadow.rt then
			self.shadow.rt = create_depth_buffer(self.shadow.BUFFER_RESOLUTION, self.shadow.BUFFER_RESOLUTION)
		end
		if self.shadow.rt_no_shadow then
			render.delete_render_target(self.shadow.rt_no_shadow)
			self.shadow.rt_no_shadow = nil
		end
	else
		if not self.shadow.rt_no_shadow then
			self.shadow.rt_no_shadow = create_empty_shadow_buffer()
			render.set_render_target(self.shadow.rt_no_shadow)
			render.set_viewport(0, 0, 1, 1)
			render.clear(self.shadow.no_shadow_clear_color)
			render.set_render_target(render.RENDER_TARGET_DEFAULT)
		end
		if self.shadow.rt then
			render.delete_render_target(self.shadow.rt)
			self.shadow.rt = nil
		end
	end

	if (not self.enable_shadow) then return end

	local light_projection = self.shadow.light_projection
	render.set_projection(light_projection)
	render.set_view(self.shadow.light_transform)
	render.set_viewport(0, 0, self.shadow.BUFFER_RESOLUTION, self.shadow.BUFFER_RESOLUTION)

	render.set_depth_mask(true)
	render.set_depth_func(render.COMPARE_FUNC_LEQUAL)
	render.enable_state(render.STATE_DEPTH_TEST)
	render.enable_state(render.STATE_CULL_FACE)
	render.disable_state(render.STATE_BLEND)
	render.disable_state(render.STATE_STENCIL_TEST)

	render.set_render_target(self.shadow.rt, self.shadow.draw_transient)
	render.clear(self.shadow.draw_clear)
	xmath.matrix_mul(self.shadow.draw_shadow_opts.frustum, light_projection, self.shadow.light_transform)
	render.enable_material("shadow")
	render.draw(self.shadow.pred, self.shadow.draw_shadow_opts)
	render.disable_material()
	render.set_render_target(render.RENDER_TARGET_DEFAULT)
end

function Lights:create_light()
	local l = illumination.light_create()
	table.insert(self.lights.in_world, l)
	return l
end

function Lights:remove_light(light)
	illumination.light_destroy(light)
	for i = 1, #self.lights.in_world do
		if self.lights.in_world[i] == light then
			table.remove(self.lights.in_world, i)
			break
		end
	end
end

function Lights:update_lights()
	if not self.enable_lights then return end

	local new_z = illumination.lights_get_z_slice_for_shader()
	if self.clusters_data.z ~= new_z then
		self.clusters_data.z = new_z
		for _, constant in ipairs(self.constants) do
			constant.clusters_data = self.clusters_data
		end
	end

	illumination.lights_update()
end

function Lights:set_frustum(frustum)
	self.frustum = frustum
	xmath.matrix_inv(self.frustum_inv, self.frustum)
end

function Lights:set_view(view)
	self.view = view
end

function Lights:dispose()
	for _, l in ipairs(self.lights.in_world) do
		illumination.light_destroy(l)
	end
	self.lights.in_world = {}
end

--endregion

return Lights()
