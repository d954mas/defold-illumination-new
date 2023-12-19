components {
  id: "dummy"
  component: "/walker/dummy/dummy.script"
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
    id: "observer"
    value: "dummy"
    type: PROPERTY_TYPE_URL
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
