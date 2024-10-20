#include "map_path.h"
#include "godot_cpp/core/math.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void MapPath::_bind_methods() {
  ClassDB::bind_method(D_METHOD("grow_nodes"), &MapPath::grow_nodes);
  ClassDB::bind_method(D_METHOD("get_grow_scale"), &MapPath::get_grow_scale);
  ClassDB::bind_method(D_METHOD("set_grow_scale", "p_grow_scale"), &MapPath::set_grow_scale);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "grow_scale"), "set_grow_scale", "get_grow_scale");
}

PathNode* MapPath::add_node(PathNode* p_parent) {
  // Initialize the new node
  PathNode* root_path_node = memnew(PathNode);
  if(p_parent) {
    root_path_node->previous = p_parent;
    root_path_node->set_position(p_parent->get_position() + (Vector2(0, 1)) * -grow_scale);
  }
	add_child(root_path_node);

  // Enable for debug purpose only
	root_path_node->set_owner(get_tree()->get_edited_scene_root());
  return root_path_node;
}

void MapPath::grow_nodes() {
  Vector<PathNode*> new_heads = Vector<PathNode*>();
  for(PathNode* path_node: heads) {
    new_heads.push_back(add_node(path_node));
  }

  heads = new_heads;
}

void MapPath::compute_bounds() {
  if(!heads.size()) {
    heads_bounds = Rect2();
    return;
  }

  Vector2 tl = heads[0]->get_position();
  Vector2 tr = heads[0]->get_position();
  Vector2 bl = heads[0]->get_position();

  for(PathNode* head: heads) {
    Vector2 position = head->get_position();
    if(position.x < tl.x && position.y < tl.y) {
      tl = position;
    } if (position.x < tr.x && position.y > tr.y) {
      tr = position;
    } if (position.x > bl.x && position.y < bl.y) {
      bl = position;
    }
  }
  
  heads_bounds = Rect2(
    Vector2((tl.x+tr.x)/2, (tl.y+bl.y)/2),
    Vector2(Math::abs(tl.x-tr.x), Math::abs(tl.y-bl.y))
  );
}

float MapPath::get_grow_scale() {
  return grow_scale;
}

void MapPath::set_grow_scale(float p_grow_scale) {
  grow_scale = p_grow_scale;
}

void MapPath::_enter_tree() {
  // Initialize the heads
  heads = {add_node(nullptr)};
}
