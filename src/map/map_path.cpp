#include "map_path.h"
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
  ClassDB::bind_method(D_METHOD("get_fertility_growth"), &MapPath::get_fertility_growth);
  ClassDB::bind_method(D_METHOD("set_fertility_growth", "p_fertility_growth"), &MapPath::set_fertility_growth);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fertility_growth"), "set_fertility_growth", "get_fertility_growth");
  ClassDB::bind_method(D_METHOD("get_max_angle"), &MapPath::get_max_angle);
  ClassDB::bind_method(D_METHOD("set_max_angle", "p_max_angle"), &MapPath::set_max_angle);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_angle"), "set_max_angle", "get_max_angle");
  ClassDB::bind_method(D_METHOD("get_path_width_texture"), &MapPath::get_path_width_texture);
  ClassDB::bind_method(D_METHOD("set_path_width_texture", "p_path_width"), &MapPath::set_path_width_texture);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "path_width_texture", PROPERTY_HINT_RESOURCE_TYPE, "Noise"), "set_path_width_texture", "get_path_width_texture");
}

Ref<Noise> MapPath::get_path_width_texture() {
  return path_width_texture;
}

void MapPath::set_path_width_texture(Ref<Noise> p_path_width_texture) {
  path_width_texture = p_path_width_texture;
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


float MapPath::get_fertility_growth() {
  return fertility_growth;
}

void MapPath::set_fertility_growth(const float p_fertility_growth) {
  fertility_growth = p_fertility_growth;
}

PathNode* MapPath::add_node(PathNode* p_parent, Vector2 p_offset=Vector2(0 , -1), float fertility=0) {
  // Initialize the new node
  PathNode* new_path_node = memnew(PathNode);
  if(p_parent && UtilityFunctions::is_instance_valid(p_parent)) {
    // Get the existing angle if there is one
    if(p_parent->previous.size() > 0) {
      Vector2 previous_direction = p_parent->get_position() - p_parent->get_previous_position();
      p_offset = p_offset.rotated(-previous_direction.normalized().angle_to(Vector2(0, -1)));
    }

    p_offset = p_offset.normalized();
    float rotation_angle = p_offset.angle_to(Vector2(0, -1));
    if (Math::abs(rotation_angle) > max_angle) {
      p_offset = p_offset.rotated(max_angle - rotation_angle);
    }
    p_offset = Vector2(p_offset.x * (1 / p_offset.y), 1);
    new_path_node->previous.push_back(p_parent);
    new_path_node->set_position(p_parent->get_position() + p_offset * -grow_scale);
    new_path_node->set_fertility(fertility);
    if(path_width_texture.is_valid()) {
      new_path_node->set_width(
        path_width_texture->get_noise_2d(
          new_path_node->get_position().x,
          new_path_node->get_position().y
        ) / 2 + 0.5
      );
    }
    p_parent->next.push_back(new_path_node);
  }
	add_child(new_path_node);

  if(!tail) {
    tail = new_path_node;
  }

  // Enable for debug purpose only
	new_path_node->set_owner(get_tree()->get_edited_scene_root());
  return new_path_node;
}

void MapPath::grow_nodes(Rect2 camera_view) {
  Vector<PathNode*> new_heads = Vector<PathNode*>();
  for(PathNode* path_node: heads) {
    if(!camera_view.intersects(Rect2(path_node->get_position(), Vector2(0, 0)))) {
      camera_view = camera_view.grow_individual(
        camera_view.size.x/2,
        camera_view.size.y/2,
        camera_view.size.x/2,
        camera_view.size.y/2
      );

      if(camera_view.intersects(Rect2(path_node->get_position(), Vector2(0, 0)))) {
        new_heads.push_back(path_node);
      }
      continue;
    }
    if(path_node->get_fertility() > UtilityFunctions::randf_range(0, 1)) {
      // Its very important to build the left node before the right one
      // because it is required for the wall build
      new_heads.push_back(add_node(path_node, Vector2(-0.5, -1)));
      new_heads.push_back(add_node(path_node, Vector2(0.5, -1)));
    } else {
      new_heads.push_back(add_node(path_node, Vector2(0 , -1), path_node->get_fertility()+fertility_growth));
    }
  }

  if(new_heads.size()) {
    heads = fuse_nodes(new_heads, grow_scale/2);
    compute_bounds();
  }
}

Vector<PathNode*> MapPath::fuse_nodes(Vector<PathNode*> nodes, float distance) {
  Vector<PathNode*> fused_nodes = Vector<PathNode*>();
  Vector<PathNode*> removed_nodes = Vector<PathNode*>();

  for(int i = 0; i < nodes.size(); i++) {
    UtilityFunctions::print("FUSING");
    PathNode* node = nodes[i];
    if(node->fused) {
      continue;
    }

    int fused_count = 1;
    Vector2 fused_position = node->get_position();

    for(int j = i+1; j < nodes.size(); j++) {
      PathNode* potential_neighbour = nodes[j];
      if(node->get_position().distance_to(potential_neighbour->get_position()) > distance) {
        continue;
      }

      UtilityFunctions::print("FUSED");
      fused_count++;
      fused_position += potential_neighbour->get_position();
      potential_neighbour->fused = true;
      removed_nodes.push_back(potential_neighbour);

      for(PathNode* neighbour_previous: potential_neighbour->get_previous()) {
        node->get_previous().push_back(neighbour_previous);
        neighbour_previous->get_next().push_back(node);
        UtilityFunctions::print("REMOVE");
        neighbour_previous->get_next().remove_at(
          neighbour_previous->get_next().find(potential_neighbour)
        );
        UtilityFunctions::print("REMOVED");
      }
    }

    node->set_position(fused_position/fused_count);
    fused_nodes.push_back(node);
  }

  for(PathNode* removed_node: removed_nodes) {
    removed_node->get_parent()->remove_child(removed_node);
  }

  return fused_nodes;
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

Rect2 MapPath::get_heads_bounds() {
  return heads_bounds;
}

PathNode* MapPath::get_tail() {
  return tail;
}

void MapPath::_enter_tree() {
  // Initialize the heads
  heads = {add_node(nullptr)};
  compute_bounds();
}

void MapPath::_draw() {
  draw_rect(heads_bounds, Color::from_hsv(1, 1, 0.5));
}
