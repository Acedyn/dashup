#include "map_path.h"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void MapPath::_bind_methods() {
  ClassDB::bind_method(D_METHOD("grow_nodes"), &MapPath::grow_nodes);
  ClassDB::bind_method(D_METHOD("get_grow_scale"), &MapPath::get_grow_scale);
  ClassDB::bind_method(D_METHOD("set_grow_scale", "p_grow_scale"), &MapPath::set_grow_scale);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "grow_scale"), "set_grow_scale", "get_grow_scale");
  ClassDB::bind_method(D_METHOD("get_max_angle"), &MapPath::get_max_angle);
  ClassDB::bind_method(D_METHOD("set_max_angle", "p_max_angle"), &MapPath::set_max_angle);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_angle"), "set_max_angle", "get_max_angle");
}

PathNode* MapPath::add_node(PathNode* p_parent, Vector2 p_offset=Vector2(0 , -1)) {
  // Initialize the new node
  PathNode* new_path_node = memnew(PathNode);
  if(p_parent && UtilityFunctions::is_instance_valid(p_parent)) {
    // Get the existing angle if there is one
    if(p_parent->previous && UtilityFunctions::is_instance_valid(p_parent->previous)) {
      Vector2 previous_direction = p_parent->get_position() - p_parent->previous->get_position();
      p_offset = p_offset.rotated(-previous_direction.normalized().angle_to(Vector2(0, -1)));
    }

    p_offset = p_offset.normalized();
    float rotation_angle = p_offset.angle_to(Vector2(0, -1));
    if (Math::abs(rotation_angle) > max_angle) {
      p_offset = p_offset.rotated(max_angle - rotation_angle);
    }
    p_offset = Vector2(p_offset.x * (1 / p_offset.y), 1);
    new_path_node->previous = p_parent;
    new_path_node->set_position(p_parent->get_position() + p_offset * -grow_scale);
  }
	add_child(new_path_node);

  // Enable for debug purpose only
	new_path_node->set_owner(get_tree()->get_edited_scene_root());
  return new_path_node;
}

void MapPath::grow_nodes(Rect2 camera_view) {
  Vector<PathNode*> new_heads = Vector<PathNode*>();
  for(PathNode* path_node: heads) {
    if(camera_view.encloses(Rect2(path_node->get_position(), Vector2(0, 0)))) {
      continue;
    }
    if(path_node->get_fertility() > UtilityFunctions::randf_range(0, 1)) {
      new_heads.push_back(add_node(path_node, Vector2(0.5, -1)));
      new_heads.push_back(add_node(path_node, Vector2(-0.5, -1)));
    } else {
      new_heads.push_back(add_node(path_node));
    }
  }

  heads = new_heads;
  compute_bounds();
}

void MapPath::compute_bounds() {
  if(!heads.size()) {
    heads_bounds = Rect2();
    return;
  }

  float top = heads[0]->get_position().y;
  float bottom = heads[0]->get_position().y;
  float left = heads[0]->get_position().x;
  float right = heads[0]->get_position().x;

  for(PathNode* head: heads) {
    Vector2 position = head->get_position();
    top = Math::min(top, position.y);
    bottom = Math::max(bottom, position.y);
    left = Math::min(left, position.x);
    right = Math::max(right, position.x);
  }
  
  heads_bounds = Rect2(Vector2(left, top), Vector2(Math::abs(left-right), Math::abs(top-bottom)));
}

float MapPath::get_grow_scale() {
  return grow_scale;
}

void MapPath::set_grow_scale(float p_grow_scale) {
  grow_scale = p_grow_scale;
}

float MapPath::get_max_angle() {
  return Math::rad_to_deg(max_angle);
}

void MapPath::set_max_angle(float p_max_angle) {
  max_angle = Math::deg_to_rad(p_max_angle);
}

Rect2 MapPath::get_heads_bounds() {
  return heads_bounds;
}

void MapPath::_enter_tree() {
  // Initialize the heads
  heads = {add_node(nullptr)};
  compute_bounds();
}

void MapPath::_draw() {
  draw_rect(heads_bounds, Color::from_hsv(1, 1, 0.5));
}
