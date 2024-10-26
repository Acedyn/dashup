#include "dashup_map.h"
#include "camera/dashup_camera.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/variant/callable.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void DashUpMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_camera_updated", "p_camera"), &DashUpMap::on_camera_updated);
  ClassDB::bind_method(D_METHOD("get_camera"), &DashUpMap::get_camera_path);
  ClassDB::bind_method(D_METHOD("set_camera", "p_camera"), &DashUpMap::set_camera_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "DashUpCamera"), "set_camera", "get_camera");
}

DashUpMap::DashUpMap(): time_passed(0.0), map_path(memnew(MapPath)) {}

DashUpMap::~DashUpMap() {}

NodePath DashUpMap::get_camera_path() {
  return camera_path;
}

void DashUpMap::set_camera_path(const NodePath& p_camera_path) {
  // TODO: Handle signal disconnection
  // Validate the node path
  DashUpCamera* new_camera = get_node<DashUpCamera>(p_camera_path);
  if(new_camera) {
    camera_path = p_camera_path;
    new_camera->connect("changed", Callable(this, "on_camera_updated"));
  } else {
    camera_path = NodePath();
  }
}

void DashUpMap::on_camera_updated(Camera2D* p_camera) {
	Vector2 position = p_camera->get_position();
	Vector2 viewport_size = p_camera->get_viewport_rect().size;
  if(Engine::get_singleton()->is_editor_hint()) {
    viewport_size = Vector2(1152, 648);
  }
	Rect2 camera_view = Rect2(position - viewport_size/2, viewport_size);

  while(camera_view.intersects(map_path->get_heads_bounds())) {
    map_path->grow_nodes(camera_view);
  }
}

void DashUpMap::_process(double delta) {}

void DashUpMap::_enter_tree() {
  // Enable for debug purpose only
	add_child(map_path);
	map_path->set_owner(get_tree()->get_edited_scene_root());
}
