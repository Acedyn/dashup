#include "dashup_map.h"
#include "camera/dashup_camera.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/geometry2d.hpp"
#include "godot_cpp/classes/polygon2d.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void DashUpMap::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_camera_updated", "p_camera"), &DashUpMap::on_camera_updated);
  ClassDB::bind_method(D_METHOD("get_camera"), &DashUpMap::get_camera_path);
  ClassDB::bind_method(D_METHOD("set_camera", "p_camera"), &DashUpMap::set_camera_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "DashUpCamera"), "set_camera", "get_camera");
  ClassDB::bind_method(D_METHOD("get_path_width_min"), &DashUpMap::get_path_width_min);
  ClassDB::bind_method(D_METHOD("set_path_width_min", "p_path_width"), &DashUpMap::set_path_width_min);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "path_width_min"), "set_path_width_min", "get_path_width_min");
  ClassDB::bind_method(D_METHOD("get_path_width_max"), &DashUpMap::get_path_width_max);
  ClassDB::bind_method(D_METHOD("set_path_width_max", "p_path_width"), &DashUpMap::set_path_width_max);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "path_width_max"), "set_path_width_max", "get_path_width_max");
}

DashUpMap::DashUpMap() {}

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

float DashUpMap::get_path_width_min() {
  return path_width_min;
}

void DashUpMap::set_path_width_min(const float p_path_width_min) {
  path_width_min = p_path_width_min;
}

float DashUpMap::get_path_width_max() {
  return path_width_max;
}

void DashUpMap::set_path_width_max(const float p_path_width_max) {
  path_width_max = p_path_width_max;
}

void DashUpMap::on_camera_updated(Camera2D* p_camera) {
  UtilityFunctions::print("CAM UPDATED");
	Vector2 position = p_camera->get_position();
	Vector2 viewport_size = p_camera->get_viewport_rect().size;
  if(Engine::get_singleton()->is_editor_hint()) {
    viewport_size = Vector2(1152, 648);
  }
	Rect2 camera_view = Rect2(position - viewport_size/2, viewport_size);

  bool updated = false;
  while(camera_view.intersects(map_path->get_heads_bounds())) {
    updated = true;
    UtilityFunctions::print("GROW", camera_view, map_path->get_heads_bounds());
    map_path->grow_nodes(camera_view);
  }

  if(updated) {
    UtilityFunctions::print("UPDATED");
    build_walls();
  }
}

const TypedArray<PackedVector2Array> DashUpMap::post_process_wall(const Vector<PathNode*> polygons) {
  PackedVector2Array new_polyons = PackedVector2Array();
  // Get the center of the polygon
  Vector2 polygon_center = Vector2();
  for(PathNode* point: polygons) {
    polygon_center += point->get_position();
  }
  polygon_center = polygon_center / polygons.size();

  for(int i = 0; i < polygons.size(); i++) {
    // Get the point and its neighbours
    Vector2 point = polygons[i]->get_position();
    int previous_index = i - 1;
    if(i == 0) {
      previous_index = polygons.size() - 1;
    }
    Vector2 previous_point = polygons[previous_index]->get_position();
    int next_index = i + 1;
    if(i == polygons.size()-1) {
      next_index = 0;
    }
    Vector2 next_point = polygons[next_index]->get_position();

    // Shrink the point
    Vector2 direction_previous = (previous_point - point).normalized();
    Vector2 direction_next = (next_point - point).normalized();
    point += (polygon_center - point).normalized() * (polygons[i]->get_width() * (path_width_max - path_width_min));
    new_polyons.push_back(point);
  }

  return Geometry2D::get_singleton()->offset_polygon(new_polyons, -path_width_min, Geometry2D::JOIN_SQUARE);
}

Vector<Polygon2D*> DashUpMap::build_wall(Vector<PathNode*> up, Vector<PathNode*> down) {
  Vector<PathNode*> polygon = Vector<PathNode*>();

  if(down.size() == 0 && up.size() > 0) {
    // Build the left wall
    PathNode top_left = up[0]->clone();
    top_left.set_position(top_left.get_position() + Vector2(-path_width_max*10, 0));
    polygon.append(&top_left);
    for(PathNode* node: up) {
      polygon.append(node);
    }
    PathNode bottom_left = up[0]->clone();
    bottom_left.set_position(bottom_left.get_position() + Vector2(-path_width_max*10, 0));
  } else if (up.size() == 0 && down.size() > 0) {
    // Build the right wall
    PathNode top_right = down[0]->clone();
    top_right.set_position(top_right.get_position() + Vector2(+path_width_max*10, 0));
    polygon.append(&top_right);
    for(PathNode* node: down) {
      polygon.append(node);
    }
    PathNode bottom_right = down[0]->clone();
    bottom_right.set_position(bottom_right.get_position() + Vector2(+path_width_max*10, 0));
  } else {
    for(PathNode* node: down) {
      // Walls between each ramifications
      polygon.append(node);
    }
    for(PathNode* node: up) {
      polygon.append(node);
    }
  }

  // Assign and create the walls
  Vector<Polygon2D*> walls = Vector<Polygon2D*>();
  TypedArray<PackedVector2Array> new_polyons = post_process_wall(polygon);
  for(int i = 0; i < new_polyons.size(); i++) {
    PackedVector2Array new_polyon = new_polyons[i];
    Polygon2D* wall = memnew(Polygon2D);
    wall->set_polygon(new_polyon);
    add_child(wall);
    wall->set_owner(get_tree()->get_edited_scene_root());
    walls.push_back(wall);
  }
  return walls;
}

void DashUpMap::traverse_node(PathNode* node, Vector<PathNode*> up, Vector<PathNode*> down) {
  // If up list is not empty we are going up
  if(up.size() > 0) {
    // We reached a head
    if(node->get_next().size() == 0) {
      UtilityFunctions::print("BUILD WALL");
      for(Polygon2D* wall: build_wall(up, down)) {
        UtilityFunctions::print("PUSH WALL");
        walls.push_back(wall);
      }
      traverse_node(node, Vector<PathNode*>{}, Vector<PathNode*>{node});
    }
    // We always take the first next, assuming the first node should always be the one on the left
    else {
      up.push_back(node->get_next()[0]);
      traverse_node(node->get_next()[0], up, down);
    }
  }
  // Otherwise we are going down
  else {
    // We reached a ramification
    if(node->get_next().size() > 1 && node->get_next()[0] == down[down.size()-2]) {
      traverse_node(node->get_next()[1], Vector<PathNode*>{node->get_next()[1]}, down);
    } else {
      // We reached the tail
      if(node->get_previous().size() == 0) {
        UtilityFunctions::print("BUILD WALL");
        for(Polygon2D* wall: build_wall(up, down)) {
          UtilityFunctions::print("PUSH WALL");
          walls.push_back(wall);
        }
        return;
      }
      // We always take the first once, multiple previous is not handled
      else {
        down.push_back(node->get_previous()[0]);
        traverse_node(node->get_previous()[0], up, down);
      }
    }
  }
}

void DashUpMap::build_walls() {
    UtilityFunctions::print("BUILD WALLS");
  if(!map_path->get_tail()) {
    return;
  }

  // Cleanup the existing walls
  for(Polygon2D* wall: walls) {
    wall->get_parent()->remove_child(wall);
  }
  walls = Vector<Polygon2D*>();

  // Rebuild the walls
  UtilityFunctions::print("TRAVERSE");
  traverse_node(map_path->get_tail(), Vector<PathNode*>{map_path->get_tail()}, Vector<PathNode*>{});
}

void DashUpMap::_process(double delta) {}

void DashUpMap::_enter_tree() {
	add_child(map_path);
  // Enable for debug purpose only
	map_path->set_owner(get_tree()->get_edited_scene_root());
}
