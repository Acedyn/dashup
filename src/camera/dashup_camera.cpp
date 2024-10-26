#include "dashup_camera.h"
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void DashUpCamera::_bind_methods() {
  ADD_SIGNAL(MethodInfo("changed", PropertyInfo(Variant::OBJECT, "camera")));
}

void DashUpCamera::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSFORM_CHANGED: {
      emit_signal("changed", this);
		} break;
	}
}
