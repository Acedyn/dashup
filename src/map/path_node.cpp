#include "path_node.h"

using namespace godot;

PathNode::PathNode() {}
PathNode::PathNode(PathNode* p_previous): previous(p_previous) {}
PathNode::PathNode(PathNode* p_previous, Vector2 &p_position): previous(p_previous), position(p_position) {}
PathNode::PathNode(PathNode* p_previous, Vector<PathNode*> p_next, Vector2 &p_position): previous(p_previous), next(p_next), position(p_position) {}

void PathNode::_bind_methods() {}
