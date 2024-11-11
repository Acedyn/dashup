#include "path_node.h"
#include <godot_cpp/core/math.hpp>

using namespace godot;

void PathNode::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_fertility"), &PathNode::get_fertility);
  ClassDB::bind_method(D_METHOD("set_fertility", "p_fertility"), &PathNode::set_fertility);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fertility"), "set_fertility", "get_fertility");
  ClassDB::bind_method(D_METHOD("get_width"), &PathNode::get_width);
  ClassDB::bind_method(D_METHOD("set_width", "p_width"), &PathNode::set_width);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "width"), "set_width", "get_width");
}

float PathNode::get_fertility() {
  return fertility;
}

void PathNode::set_fertility(float p_fertility) {
  fertility = Math::clamp<float>(p_fertility, 0, 1);
}

float PathNode::get_width() {
  return width;
}

void PathNode::set_width(float p_width) {
  width = p_width;
}

Vector<PathNode*> PathNode::get_next() {
  return next;
}

Vector<PathNode*> PathNode::get_previous() {
  return previous;
}


PathNode PathNode::clone() {
  // TODO: The use of this method will create memory leaks
  PathNode* path_node = memnew(PathNode);
  path_node->set_transform(get_transform());
  path_node->set_width(get_width());
  path_node->set_fertility(get_fertility());
  return *path_node;
}

Vector2 PathNode::get_average_position(Vector<PathNode*> nodes) {
  if(nodes.size() == 0) {
    return get_position();
  }

  Vector2 average_position = Vector2();
  for(PathNode* node: nodes) {
    average_position += node->get_position();
  }

  return average_position / nodes.size();
}

Vector2 PathNode::get_previous_position() {
  return get_average_position(previous);
}

Vector2 PathNode::get_next_position() {
  return get_average_position(next);
}

PathNode::PathNode() {}
PathNode::PathNode(Vector<PathNode*> p_previous): previous(p_previous) {}
PathNode::PathNode(Vector<PathNode*> p_previous, Vector<PathNode*> p_next): previous(p_previous), next(p_next) {}

