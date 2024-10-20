#include "map_path.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void MapPath::_bind_methods() {}

PathNode* MapPath::add_node(Vector2 position) {
  // Initialize the map path
  PathNode* root_path_node = memnew(PathNode);
  root_path_node->set_position(position);
	add_child(root_path_node);
  // Enable for debug purpose only
	root_path_node->set_owner(get_tree()->get_edited_scene_root());
  return root_path_node;
}

void MapPath::grow_nodes() {
  UtilityFunctions::print("grow");
  for(PathNode* path_node: heads) {
    add_node(path_node->get_position() + Vector2(0, 1));
  }
}

void MapPath::_enter_tree() {
  // Initialize the heads
  heads = {add_node(Vector2())};
}
