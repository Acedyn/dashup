#ifndef MAPWALLS_H
#define MAPWALLS_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class MapWall: public Node2D {
	GDCLASS(MapWall, Node2D)

protected:
	static void _bind_methods();

private:
  float grow_scale = 100.0;

public:
	void _enter_tree() override;
};

}

#endif

