local CLASS = require "illumination.middleclass"

local V4 = vmath.vector4()
local VIEW_DIRECTION = vmath.vector3()
local VIEW_RIGHT = vmath.vector3()
local VIEW_UP = vmath.vector3()

local TEMP_V4 = vmath.vector4()

local V_UP = vmath.vector3(0, 1, 0)

local HASH_RGBA = hash("rgba")

local RADIUS_MAX = 128

local LIGHT_IDX = 0

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

---@class Light
local Light = CLASS("Light")

---@param lights LightsData
function Light:initialize(lights)
	self.lights = assert(lights)

	LIGHT_IDX = LIGHT_IDX + 1;
	self.light_idx = LIGHT_IDX

	self.enabled = false

	---idx in active list
	self.active_idx = -1

	self.direction = vmath.vector3(0, 0, -1)
	self.position = vmath.vector3(0, 0, 0)
	self.color = vmath.vector4(1) --r,g,b brightness

	self.radius = 5
	self.smoothness = 1
	self.specular = 0.5
	self.cutoff = 1
	self.aabb = { x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0 }
end

function Light:set_enabled(enabled)
	if self.enabled ~= enabled then
		self.enabled = enabled
	end
end

function Light:update_aabb()
	self.aabb.x1 = self.position.x - self.radius
	self.aabb.y1 = self.position.y - self.radius
	self.aabb.z1 = self.position.z - self.radius
	self.aabb.x2 = self.position.x + self.radius
	self.aabb.y2 = self.position.y + self.radius
	self.aabb.z2 = self.position.z + self.radius

end

function Light:set_position(x, y, z)
	if self.position.x ~= x or self.position.y ~= y or self.position.z ~= z then
		self.position.x, self.position.y, self.position.z = x, y, z
		self.dirty = true
		self:update_aabb()
	end
end

function Light:set_direction(x, y, z)
	if self.direction.x ~= x or self.direction.y ~= y or self.direction.z ~= z then
		self.direction.x, self.direction.y, self.direction.z = x, y, z
		self.dirty = true
	end
end

function Light:set_color(r, g, b, brightness)
	if self.color.x ~= r or self.color.y ~= g or self.color.z ~= b or self.color.w ~= brightness then
		self.color.x, self.color.y, self.color.z, self.color.w = r, g, b, brightness
		self.dirty = true
	end
end

function Light:set_radius(radius)
	assert(radius >= 0)
	if self.radius ~= radius then
		self.radius = radius
		self.dirty = true
		self:update_aabb()
	end
end

function Light:set_specular(specular)
	assert(specular >= 0 and specular <= 1)
	if self.specular ~= specular then
		self.specular = specular
		self.dirty = true
	end
end

function Light:set_smoothness(smoothness)
	assert(smoothness >= 0 and smoothness <= 1)
	if self.smoothness ~= smoothness then
		self.smoothness = smoothness
		self.dirty = true
	end
end

function Light:set_cutoff(cutoff)
	assert(cutoff >= 0 and cutoff <= 1)
	if self.cutoff ~= cutoff then
		self.cutoff = cutoff
		self.dirty = true
	end
end

function Light:is_visible()
	if not self.enabled or self.color.w == 0 then return false end

	return true
end

local light_size = 6 --6 pixels per light
local light_data = {}
function Light:write_to_buffer(x_min, x_max, y_min, y_max, z_min, z_max)
	assert(self.radius <= RADIUS_MAX, "radius > " .. RADIUS_MAX)

	local idx = (self.active_idx - 1) * light_size + 1--lua side start from 1

	light_data[1], light_data[2], light_data[3], light_data[4] = illumination.float_to_rgba(self.position.x, x_min, x_max)
	light_data[5], light_data[6], light_data[7], light_data[8] = illumination.float_to_rgba(self.position.y, y_min, y_max)
	light_data[9], light_data[10], light_data[11], light_data[12] = illumination.float_to_rgba(self.position.z, z_min, z_max)

	light_data[13], light_data[14], light_data[15] = (self.direction.x + 1) / 2, (self.direction.y + 1) / 2, (self.direction.z + 1) / 2

	light_data[16] = 0

	light_data[17], light_data[18], light_data[19], light_data[20] = self.color.x, self.color.y, self.color.z, self.color.w

	light_data[21], light_data[22], light_data[23] = self.radius / RADIUS_MAX, self.smoothness, self.specular

	light_data[24] = self.cutoff < 1 and (math.cos(self.cutoff * math.pi) + 1) / 2 or 1

	illumination.fill_stream_uint8(idx, self.lights.lights.texture.buffer, HASH_RGBA, 4, light_data)
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

local function create_lights_data_texture()
	local path = "/__lights_data.texturec"
	local tparams = {
		width = 1024,
		height = 1024,
		type = resource.TEXTURE_TYPE_2D,
		format = resource.TEXTURE_FORMAT_RGBA,
		num_mip_maps = 1
	}

	local tbuffer = buffer.create(tparams.width * tparams.height, { { name = HASH_RGBA, type = buffer.VALUE_TYPE_UINT8, count = 4 } })

	local status, error = pcall(resource.create_texture, path, tparams, tbuffer)
	if status then
		return {
			params = tparams,
			texture_id = error,
			max_idx = tparams.width * tparams.height,
			buffer = tbuffer
		}
	else
		print("can't create texture:" .. tostring(error))
	end
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
	self.lights_data = vmath.vector4(0, RADIUS_MAX, 0, 0)
	self.lights_data2 = vmath.vector4()
	self.clusters_data = vmath.vector4() --max_lights_per_cluster, x_slices, y_slices, z_slices
	self.screen_size = vmath.vector4()

	self.debug = false

	self.shadow = {
		-- Size of shadow map. Select value from: 1024/2048/4096. More is better quality.
		BUFFER_RESOLUTION = 2048,
		-- Projection resolution of shadow map to the game world. Smaller size is better shadow quality,
		-- but shadows will cast only around the screen center (or a point that camera looks at).
		-- This value also depends on camera zoom. Feel free to adjust it.
		PROJECTION_X1 = -15,
		PROJECTION_X2 = 15,
		PROJECTION_Y1 = -15,
		PROJECTION_Y2 = 15,
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
		draw_shadow_opts = { frustum = vmath.matrix4(), frustum_planes = render.FRUSTUM_PLANES_ALL },
		draw_transient = { transient = { render.BUFFER_DEPTH_BIT } },
		draw_clear = { [render.BUFFER_COLOR_BIT] = vmath.vector4(1, 1, 1, 1), [render.BUFFER_DEPTH_BIT] = 1 }
	}

	self.shadow_params.x = self.shadow.BUFFER_RESOLUTION
	self.shadow_params.y = 0.0008

	---@class LightsData
	self.lights = {
		all = {},
		texture = nil,
		clusters = {
			x_slices = 15,
			y_slices = 15,
			z_slices = 15,
			max_lights_per_cluster = 190,
			clusters = {},
			pixels_per_cluster = 0
		}
	}
	--1 pixel(rgba) lights count
	--max_lights_per_cluster * 1pixel(rgba) light idx
	self.lights.clusters.pixels_per_cluster = 1 + self.lights.clusters.max_lights_per_cluster * 1
	for i = 1, self.lights.clusters.x_slices * self.lights.clusters.y_slices * self.lights.clusters.z_slices do
		table.insert(self.lights.clusters.clusters, { lights = {}, idx = i })
	end
	self.clusters_data.x = self.lights.clusters.x_slices
	self.clusters_data.y = self.lights.clusters.y_slices
	self.clusters_data.z = self.lights.clusters.z_slices
	self.clusters_data.w = self.lights.clusters.max_lights_per_cluster




	illumination.lights_init(2048, self.lights.clusters.x_slices, self.lights.clusters.y_slices,
			self.lights.clusters.z_slices, self.lights.clusters.max_lights_per_cluster)

end

function Lights:on_resize(w, h)
	self.screen_size.x, self.screen_size.y = w, h
	for _, constant in ipairs(self.constants) do
		constant.screen_size = self.screen_size
	end
end

---@param active_list Light[]
function Lights:update_clusters(active_list, camera_aspect, camera_fov, camera_far)
	local lights = self.lights
	local clusters = lights.clusters
	local x_slices = clusters.x_slices
	local y_slices = clusters.y_slices
	local z_slices = clusters.z_slices
	local max_lights_per_cluster = clusters.max_lights_per_cluster

	for i = 1, x_slices * y_slices * z_slices do
		self.lights.clusters.clusters[i].lights = {}
	end

	--instead of using the farclip plane as the arbitrary plane to base all our calculations and division splitting off of

	local tan_Vertical_FoV_by_2 = math.tan(camera_fov * 0.5);
	local zStride = (camera_far) / clusters.z_slices;

	for i = 1, #active_list do
		local l = active_list[i]
		TEMP_V4.x, TEMP_V4.y, TEMP_V4.z, TEMP_V4.w = l.position.x, l.position.y, l.position.z, 1
		xmath.matrix_mul_v4(TEMP_V4, self.view, TEMP_V4)

		TEMP_V4.z = TEMP_V4.z * -1; --camera looks down negative z, make z axis positive to make calculations easier
		local x1 = TEMP_V4.x - l.radius
		local y1 = TEMP_V4.y - l.radius
		local z1 = TEMP_V4.z - l.radius
		local x2 = TEMP_V4.x + l.radius
		local y2 = TEMP_V4.y + l.radius
		local z2 = TEMP_V4.z + l.radius

		local h_lightFrustum = math.abs(tan_Vertical_FoV_by_2 * TEMP_V4.z * 2);
		local w_lightFrustum = math.abs(camera_aspect * h_lightFrustum);

		local xStride = w_lightFrustum / x_slices;
		local yStride = h_lightFrustum / y_slices;

		--Need to extend this by -1 and +1 to avoid edge cases where light
		--technically could fall outside the bounds we make because the planes themeselves are tilted by some angle
		-- the effect is exaggerated the steeper the angle the plane makes is
		local zStartIndex = math.floor(z1 / zStride);
		local zEndIndex = math.floor(z2 / zStride);
		local yStartIndex = math.floor((y1 + h_lightFrustum * 0.5) / yStride);
		local yEndIndex = math.floor((y2 + h_lightFrustum * 0.5) / yStride);
		local xStartIndex = math.floor((x1 + w_lightFrustum * 0.5) / xStride) - 1;
		local xEndIndex = math.floor((x2 + w_lightFrustum * 0.5) / xStride) + 1;

		local visible = not ((zStartIndex < 0 and zEndIndex < 0) or (zStartIndex >= z_slices and zEndIndex >= z_slices)) and
				not ((yStartIndex < 0 and yEndIndex < 0) or (yStartIndex >= y_slices and yEndIndex >= y_slices))

		if visible then
			zStartIndex = clamp(zStartIndex, 0, z_slices - 1);
			zEndIndex = clamp(zEndIndex, 0, z_slices - 1);

			yStartIndex = clamp(yStartIndex, 0, y_slices - 1);
			yEndIndex = clamp(yEndIndex, 0, y_slices - 1);

			xStartIndex = clamp(xStartIndex, 0, x_slices - 1);
			xEndIndex = clamp(xEndIndex, 0, x_slices - 1);

			for z = zStartIndex, zEndIndex do
				for y = yStartIndex, yEndIndex do
					for x = xStartIndex, xEndIndex do
						local id = x + y * x_slices + z * x_slices * y_slices + 1;
						local cluster = clusters.clusters[id]
						if (#cluster.lights < max_lights_per_cluster) then
							table.insert(cluster.lights, l)
						else
							print("cluster:" .. id .. " already have max lights count")
						end

					end
				end
			end
		end
	end

	self.clusters_data.z = zStride
	for _, constant in ipairs(self.constants) do
		constant.clusters_data = self.clusters_data
	end

	for i = 1, x_slices * y_slices * z_slices do
		local cluster = clusters.clusters[i]
		--		print("cluster:" .. cluster.idx .. " " .. #cluster.lights)
		self:cluster_write_to_buffer(active_list, cluster)
	end

end

function Lights:cluster_write_to_buffer(active_list, cluster)
	local total_lights = #active_list
	local idx = total_lights * light_size + (cluster.idx - 1) * self.lights.clusters.pixels_per_cluster + 1--lua side start from 1
	local data = {}
	data[1], data[2], data[3], data[4] = illumination.float_to_rgba(#cluster.lights, 0, self.lights.clusters.max_lights_per_cluster)
	local data_idx = 5
	for lidx, l in ipairs(cluster.lights) do
		data[data_idx], data[data_idx + 1], data[data_idx + 2], data[data_idx + 3] = illumination.float_to_rgba(l.active_idx - 1, 0, total_lights + 1)
		data_idx = data_idx + 4
		--print("light:" .. lidx .. " active_idx:" .. l.active_idx )
	end


	--[[for i=#cluster.lights+1,self.lights.clusters.max_lights_per_cluster do
		data[data_idx], data[data_idx + 1], data[data_idx + 2], data[data_idx + 3] = 1,0,0,1
		data_idx = data_idx + 4
	end
	for i=self.lights.clusters.max_lights_per_cluster+2, self.lights.clusters.pixels_per_cluster do
		data[data_idx], data[data_idx + 1], data[data_idx + 2], data[data_idx + 3] = 0,1,0,1
		data_idx = data_idx + 4
	end--]]


	illumination.fill_stream_uint8(idx, self.lights.texture.buffer, HASH_RGBA, 4, data)
end

function Lights:init_lights_data(data_url)
	self.lights.texture = create_lights_data_texture()
	self.lights.texture.path = go.get(data_url, "texture0")
	self.light_texture_data.x = self.lights.texture.params.width
	self.light_texture_data.y = self.lights.texture.params.height
	for _, constant in ipairs(self.constants) do
		constant.light_texture_data = self.light_texture_data
	end
end

function Lights:draw_debug()
	if self.debug then
		render.draw(self.debug_predicate)
	end
end

function Lights:draw_shadow_debug()
	if self.shadow.rt then
		render.enable_texture(0, self.shadow.rt, render.BUFFER_COLOR_BIT)
		render.draw(self.debug_shadow_predicate)
		render.disable_texture(0)
	end

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
	if (self.shadow.rt) then
		render.enable_texture(1, self.shadow.rt, render.BUFFER_COLOR_BIT) -- created in light_and_shadows.init
	end
end

function Lights:draw_finish()
	if (self.shadow.rt) then
		render.disable_texture(1)
	end
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
	self.render = assert(render_obj)

	-- all objects that have to cast shadows
	self.shadow.pred = render.predicate({ "shadow" })

	self.shadow.light_projection_base = vmath.matrix4_orthographic(-100, 100,
			-100, 100, self.shadow.NEAR, self.shadow.FAR)
	self.shadow.light_projection = vmath.matrix4_orthographic(self.shadow.PROJECTION_X1, self.shadow.PROJECTION_X2,
			self.shadow.PROJECTION_Y1, self.shadow.PROJECTION_Y2, self.shadow.NEAR, self.shadow.FAR)

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
	min_x = -100 + min_x * 200
	max_x = -100 + max_x * 200
	min_y = -100 + min_y * 200
	max_y = -100 + max_y * 200

	--print("shadow uv:x[" .. min_x .. " " .. max_x .. "] y[" .. min_y .. " " .. max_y .. "] w:" ..  max_x - min_x .. " h:" .. max_y - min_y

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
	local draw_shadows = true
	if (draw_shadows and not self.shadow.rt) then
		self.shadow.rt = create_depth_buffer(self.shadow.BUFFER_RESOLUTION, self.shadow.BUFFER_RESOLUTION)
	end
	if (not draw_shadows and self.shadow.rt) then
		render.delete_render_target(self.shadow.rt)
		self.shadow.rt = nil
	end
	if (not draw_shadows) then
		return end

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


--region Lights
function Lights:create_light()
	local l = Light(self)
	table.insert(self.lights.all, l)
	return l
end

---@param light LightsData
function Lights:remove_light(light)
	light.removed = true
end

function Lights:update_lights(camera_aspect, camera_fov, camera_far)
	--check culling and disabled lights
	local active_list = {}
	local idx = 0
	local dirty_texture = false
	--TODO use nil or use first active light coord as min/max
	local x_min, x_max, y_min, y_max, z_min, z_max

	for i = #self.lights.all, 1, -1 do
		local l = self.lights.all[i]
		if l.removed then
			table.remove(self.lights.all, i)
		else
			local visible = l:is_visible()
			if visible and self.frustum then
				visible = illumination.frustum_is_box_visible(self.frustum, l.aabb.x1, l.aabb.y1, l.aabb.z1,
						l.aabb.x2, l.aabb.y2, l.aabb.z2)
			end
			if visible then
				idx = idx + 1
				active_list[idx] = l
				if not x_min then
					x_min, x_max = l.position.x, l.position.x
					y_min, y_max = l.position.y, l.position.y
					z_min, z_max = l.position.z, l.position.z
				else
					x_min = math.min(x_min, l.position.x)
					x_max = math.max(x_max, l.position.x)
					y_min = math.min(y_min, l.position.y)
					y_max = math.max(y_max, l.position.y)
					z_min = math.min(z_min, l.position.z)
					z_max = math.max(z_max, l.position.z)
				end

				if l.active_idx ~= idx then
					l.active_idx = idx
					l.dirty = true
				end
			end
		end
	end

	if not x_min then
		x_min, x_max, y_min, y_max, z_min, z_max = 0, 0, 0, 0, 0, 0
	end
	--if min and max changed need to rewrite all light. so use some fixed value to avoid additional rewrite
	if x_min < -511 then error("x_min < 511") end
	if x_max > 512 then error("x_max > 512") end
	if y_min < -511 then error("y_min < 511") end
	if y_max > 512 then error("y_max > 512") end
	if z_min < -511 then error("z_min < 511") end
	if z_max > 512 then error("z_max > 512") end

	x_min, x_max = -511, 512
	y_min, y_max = -511, 512
	z_min, z_max = -511, 512

	if self.lights_data.x ~= idx or self.lights_data.z ~= x_min or self.lights_data.w ~= x_max then
		self.lights_data.x = idx
		self.lights_data.y = RADIUS_MAX
		self.lights_data.z = x_min
		self.lights_data.w = x_max
		for _, constant in ipairs(self.constants) do
			constant.lights_data = self.lights_data
		end
	end

	if self.lights_data2.x ~= y_min or self.lights_data2.y ~= y_max or
			self.lights_data2.z ~= z_min or self.lights_data2.w ~= z_max then
		self.lights_data2.x = y_min
		self.lights_data2.y = y_max
		self.lights_data2.z = z_min
		self.lights_data2.w = z_max
		for _, constant in ipairs(self.constants) do
			constant.lights_data2 = self.lights_data2
		end
	end

	local axis_capacity_x = x_max - x_min + 1
	local axis_capacity_y = y_max - y_min + 1
	local axis_capacity_z = z_max - z_min + 1

	--	print("axis x: " .. axis_capacity_x .. " y:" .. axis_capacity_y .. " z:" .. axis_capacity_z)
	if axis_capacity_x > 1024 then
		error("axis_capacity_x" .. axis_capacity_x .. " > 1024. accuracy may be low")
	end
	if axis_capacity_y > 1024 then
		print("axis_capacity_y" .. axis_capacity_y .. " > 1024. accuracy may be low")
	end
	if axis_capacity_z > 1024 then
		print("axis_capacity_z" .. axis_capacity_z .. " > 1024. accuracy may be low")
	end

	print("lights total::" .. #self.lights.all .. " lights active:" .. #active_list)
	for i = #active_list, 1, -1 do
		local l = active_list[i]
		--rewrite dirty lights
		if l.dirty then
			l.dirty = false
			dirty_texture = true
			l:write_to_buffer(x_min, x_max, y_min, y_max, z_min, z_max)
		end
	end

	local time = chronos.nanotime()
	self:update_clusters(active_list, camera_aspect, camera_fov, camera_far)
	--print("update clusters:" .. chronos.nanotime() - time)
	dirty_texture = true

	if dirty_texture then
		resource.set_texture(self.lights.texture.path, self.lights.texture.params, self.lights.texture.buffer)
	end

end

function Lights:set_frustum(frustum)
	self.frustum = frustum
	xmath.matrix_inv(self.frustum_inv, self.frustum)
end

function Lights:set_view(view)
	self.view = view
end

--endregion

return Lights()
