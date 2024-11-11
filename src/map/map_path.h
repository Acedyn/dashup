#ifndef MAPPATH_H
#define MAPPATH_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/noise.hpp>
#include <godot_cpp/templates/vector.hpp>
#include "godot_cpp/classes/fast_noise_lite.hpp"
#include "godot_cpp/core/memory.hpp"
#include "path_node.h"

namespace godot {

class MapPath: public Node2D {
	GDCLASS(MapPath, Node2D)

protected:
	static void _bind_methods();

private:
  Vector<PathNode*> heads = Vector<PathNode*>();
  PathNode* tail = nullptr;
  Rect2 heads_bounds = Rect2();
  float grow_scale = 100.0;
  float fertility_growth = 0.001;
  float max_angle = 0.4 * Math_PI;
  Ref<Noise> path_width_texture = Ref(memnew(FastNoiseLite));

  PathNode* add_node(PathNode* p_parent, Vector2 p_offset, float fertility);
  Vector<PathNode*> fuse_nodes(Vector<PathNode*> nodes, float distance);
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
  float get_fertility_growth();
  void set_fertility_growth(const float p_fertility_growth);
  Ref<Noise> get_path_width_texture();
  void set_path_width_texture(Ref<Noise> p_path_width);
  Rect2 get_heads_bounds();
};

}

#endif
