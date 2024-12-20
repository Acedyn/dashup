#include "path_node.h"
#include <godot_cpp/core/math.hpp>

using namespace godot;

void PathNode::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_fertility"), &PathNode::get_fertility);
  ClassDB::bind_method(D_METHOD("set_fertility", "p_fertility"), &PathNode::set_fertility);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fertility"), "set_fertility", "get_fertility");
}

float PathNode::get_fertility() {
  return fertility;
}

void PathNode::set_fertility(float p_fertility) {
  fertility = Math::clamp<float>(p_fertility, 0, 1);
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
PathNode::PathNode(Vector<PathNode*> p_previous, Vector2 &p_position): previous(p_previous), position(p_position) {}
PathNode::PathNode(Vector<PathNode*> p_previous, Vector<PathNode*> p_next, Vector2 &p_position): previous(p_previous), next(p_next), position(p_position) {}

