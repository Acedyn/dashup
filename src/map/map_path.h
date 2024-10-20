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
  PathNode* add_node(Vector2 position);

public:
  void grow_nodes();
	void _enter_tree() override;
};

}

#endif
