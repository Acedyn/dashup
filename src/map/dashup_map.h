#ifndef DASHUPMAP_H
#define DASHUPMAP_H

#include "map_path.h"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class DashUpMap : public Node2D {
	GDCLASS(DashUpMap, Node2D)

private:
	double time_passed;
  NodePath camera_path;
  MapPath* map_path;

protected:
	static void _bind_methods();

public:
	DashUpMap();
	~DashUpMap();

  NodePath get_camera_path();
  void set_camera_path(const NodePath& p_camera_path);

	void _process(double delta) override;
	void _enter_tree() override;
};

}

#endif

