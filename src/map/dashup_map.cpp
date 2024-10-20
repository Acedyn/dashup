#include "dashup_map.h"
#include "camera/dashup_camera.h"
#include "godot_cpp/variant/callable.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void DashUpMap::_bind_methods() {
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
    new_camera->connect("changed", Callable(map_path, "grow_nodes"));
  } else {
    camera_path = NodePath();
  }
}

void DashUpMap::_process(double delta) {
	time_passed += delta;

	Vector2 new_position = Vector2(10.0 + (10.0 * Math::sin(time_passed * 2.0)), 10.0 + (10.0 * Math::cos(time_passed * 1.5)));

	set_position(new_position);
}

void DashUpMap::_enter_tree() {
  // Enable for debug purpose only
	add_child(map_path);
	map_path->set_owner(get_tree()->get_edited_scene_root());
}
