#ifndef MAPPATH_H
#define MAPPATH_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include "path_node.h"

namespace godot {

class MapPath: public Node2D {
	GDCLASS(MapPath, Node2D)

protected:
	static void _bind_methods();

private:
  Vector<PathNode*> heads;
  PathNode* tail;
  Rect2 heads_bounds = Rect2();
  float grow_scale = 100.0;
  float max_angle = 0.4 * Math_PI;

  PathNode* add_node(PathNode* p_parent, Vector2 p_offset);
  void compute_bounds();

public:
  void grow_nodes(Rect2 camera_view);
	void _enter_tree() override;
	void _draw() override;
  float get_grow_scale();
  PathNode* get_tail();
  void set_grow_scale(const float p_grow_scale);
  float get_max_angle();
  void set_max_angle(const float p_max_angle);
  Rect2 get_heads_bounds();
};

}

#endif
