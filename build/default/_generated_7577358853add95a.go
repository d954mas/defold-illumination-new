components {
  id: "body"
  component: "/walker/body.script"
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
  property_decls {
  }
}
embedded_components {
  id: "collision_crouching"
  type: "collisionobject"
  data: "collision_shape: \"\"\ntype: COLLISION_OBJECT_TYPE_KINEMATIC\nmass: 0.0\nfriction: 0.0\nrestitution: 0.0\ngroup: \"walker\"\nmask: \"default\"\nmask: \"trigger\"\nembedded_collision_shape {\n  shapes {\n    shape_type: TYPE_CAPSULE\n    position {\n      x: 0.0\n      y: 0.3\n      z: 0.0\n    }\n    rotation {\n      x: 0.0\n      y: 0.0\n      z: 0.0\n      w: 1.0\n    }\n    index: 0\n    count: 2\n  }\n  data: 0.25\n  data: 0.1\n}\nlinear_damping: 0.0\nangular_damping: 0.0\nlocked_rotation: true\nbullet: false\n"
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
  id: "collision_standing"
  type: "collisionobject"
  data: "collision_shape: \"\"\ntype: COLLISION_OBJECT_TYPE_KINEMATIC\nmass: 0.0\nfriction: 0.0\nrestitution: 0.0\ngroup: \"walker\"\nmask: \"default\"\nmask: \"trigger\"\nembedded_collision_shape {\n  shapes {\n    shape_type: TYPE_CAPSULE\n    position {\n      x: 0.0\n      y: 0.65\n      z: 0.0\n    }\n    rotation {\n      x: 0.0\n      y: 0.0\n      z: 0.0\n      w: 1.0\n    }\n    index: 0\n    count: 2\n  }\n  data: 0.25\n  data: 0.8\n}\nlinear_damping: 0.0\nangular_damping: 0.0\nlocked_rotation: true\nbullet: false\n"
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
