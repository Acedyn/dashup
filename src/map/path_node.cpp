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

PathNode::PathNode() {}
PathNode::PathNode(PathNode* p_previous): previous(p_previous) {}
PathNode::PathNode(PathNode* p_previous, Vector2 &p_position): previous(p_previous), position(p_position) {}
PathNode::PathNode(PathNode* p_previous, Vector<PathNode*> p_next, Vector2 &p_position): previous(p_previous), next(p_next), position(p_position) {}

