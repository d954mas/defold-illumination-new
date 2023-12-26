components {
  id: "illumination"
  component: "/illumination/illumination_script.script"
  position {
    x: 0.0
    y: 0.0
    z: 0.0
  }
  rotation {
    x: 0.0
    y: 0.0
    z: 0.0
    w: 1.0
  }
}
components {
  id: "illumination_textures"
  component: "/illumination/illumination_textures.script"
  position {
    x: 0.0
    y: 0.0
    z: 0.0
  }
  rotation {
    x: 0.0
    y: 0.0
    z: 0.0
    w: 1.0
  }
}
embedded_components {
  id: "data"
  type: "mesh"
  data: "material: \"/illumination/assets/materials/data_not_draw.material\"\n"
  "vertices: \"/illumination/shadow_debug.buffer\"\n"
  "textures: \"/illumination/textures/data.png\"\n"
  "primitive_type: PRIMITIVE_TRIANGLES\n"
  "position_stream: \"position\"\n"
  ""
  position {
    x: 0.0
    y: 0.0
    z: 0.0
  }
  rotation {
    x: 0.0
    y: 0.0
    z: 0.0
    w: 1.0
  }
}
