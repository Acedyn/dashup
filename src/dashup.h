#ifndef DASHUP_H
#define DASHUP_H

#include <godot_cpp/classes/node2d.hpp>
#include "map/dashup_map.h"
#include "camera/dashup_camera.h"

namespace godot {

class DashUp : public Node2D {
	GDCLASS(DashUp, Node2D)

private:
  DashUpMap* map;
  DashUpCamera* camera;

protected:
	static void _bind_methods();

public:
	DashUp();

	void _process(double delta) override;
	void _enter_tree() override;
};

}

#endif
