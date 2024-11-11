#ifndef PATHNODE_H
#define PATHNODE_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class PathNode : public Node2D {
  GDCLASS(PathNode, Node2D)

protected:
	static void _bind_methods();
  float fertility = 0.0;
  float width = 0.5;

private:
  Vector2 get_average_position(Vector<PathNode*> nodes);

public:
  bool fused = false;
  Vector<PathNode*> next = *memnew(Vector<PathNode*>);
  Vector<PathNode*> previous = *memnew(Vector<PathNode*>);

  float get_fertility();
  void set_fertility(float p_fertility);
  float get_width();
  void set_width(float p_width);
  Vector2 get_previous_position();
  Vector2 get_next_position();
  Vector<PathNode*> get_next();
  Vector<PathNode*> get_previous();
  PathNode clone();

  PathNode();
  PathNode(Vector<PathNode*> p_previous);
  PathNode(Vector<PathNode*> p_previous, Vector<PathNode*> p_next);
};

}

#endif
