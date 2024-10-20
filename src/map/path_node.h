#ifndef PATHNODE_H
#define PATHNODE_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class PathNode : public Node2D {
  GDCLASS(PathNode, Node2D)

protected:
	static void _bind_methods();

public:
  Vector2 position = Vector2();
  Vector<PathNode*> next;
  PathNode* previous;

  PathNode();
  PathNode(PathNode* p_previous);
  PathNode(PathNode* p_previous, Vector2 &p_position);
  PathNode(PathNode* p_previous, Vector<PathNode*> p_next, Vector2 &p_position);
};

}

#endif
