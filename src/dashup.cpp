#include "dashup.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DashUp::_bind_methods() {}

DashUp::DashUp(): map(memnew(DashUpMap)), camera(memnew(DashUpCamera)) {}

void DashUp::_process(double delta) {
}

void DashUp::_enter_tree() {
  // Enable for debug purpose only
	add_child(map);
	map->set_owner(get_tree()->get_edited_scene_root());
	add_child(camera);
	camera->set_owner(get_tree()->get_edited_scene_root());
  map->set_camera_path(camera->get_path());
}
