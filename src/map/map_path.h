#ifndef MAPPATH_H
#define MAPPATH_H

#include <godot_cpp/classes/Node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include "path_node.h"

namespace godot {

class MapPath: public Node {
	GDCLASS(MapPath, Node)

protected:
	static void _bind_methods();

private:
  Vector<PathNode*> heads;
  Rect2 heads_bounds;
  float grow_scale = 1.0;

  PathNode* add_node(PathNode* p_parent);
  void compute_bounds();

public:
  void grow_nodes();
	void _enter_tree() override;
  float get_grow_scale();
  void set_grow_scale(const float p_grow_scale);
};

}

#endif
