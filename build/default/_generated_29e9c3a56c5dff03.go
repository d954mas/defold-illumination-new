components {
  id: "player"
  component: "/example/player.script"
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
components {
  id: "walker"
  component: "/walker/walker.script"
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
  properties {
    id: "normal_speed"
    value: "2.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "shift_speed"
    value: "4.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "jump_power"
    value: "8.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "climbing_angle"
    value: "60.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "slope_speed_factor"
    value: "0.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "is_crouching_allowed"
    value: "true"
    type: PROPERTY_TYPE_BOOLEAN
  }
  properties {
    id: "eyes_switching"
    value: "true"
    type: PROPERTY_TYPE_BOOLEAN
  }
  property_decls {
  }
}
