#ifndef DASHUPMAP_H
#define DASHUPMAP_H

#include "godot_cpp/templates/vector.hpp"
#include "map/path_node.h"
#include "map_path.h"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class DashUpMap : public Node2D {
	GDCLASS(DashUpMap, Node2D)

private:
  float path_width_min = 50;
  float path_width_max = 100;
  Texture2D* path_width_texture = nullptr;
  NodePath camera_path = NodePath();
  MapPath* map_path = memnew(MapPath);
  Vector<Polygon2D*> walls = Vector<Polygon2D*>();

	void build_walls();
	Vector<Polygon2D*> build_wall(Vector<PathNode*> up, Vector<PathNode*> down);
	const TypedArray<PackedVector2Array> post_process_wall(const Vector<PathNode*> polygons);
	void traverse_node(PathNode* node, Vector<PathNode*> up, Vector<PathNode*> down);

protected:
	static void _bind_methods();

public:
	DashUpMap();
	~DashUpMap();

  NodePath get_camera_path();
  void set_camera_path(const NodePath& p_camera_path);
  float get_path_width_min();
  void set_path_width_min(const float p_path_width);
  float get_path_width_max();
  void set_path_width_max(const float p_path_width);
  Texture2D* get_path_width_texture();
  void set_path_width_texture(Texture2D* p_path_width);
  void on_camera_updated(Camera2D* p_camera);

	void _process(double delta) override;
	void _enter_tree() override;
};

}

#endif

