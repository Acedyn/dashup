#ifndef DASHUPCAMERA_H
#define DASHUPCAMERA_H

#include <godot_cpp/classes/camera2d.hpp>

namespace godot {

class DashUpCamera: public Camera2D {
	GDCLASS(DashUpCamera, Camera2D)

protected:
	static void _bind_methods();
	void _notification(int p_what);
};

}

#endif

