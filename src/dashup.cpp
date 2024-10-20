#include "dashup.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DashUp::_bind_methods() {}

DashUp::DashUp() {
	time_passed = 0.0;
}

DashUp::~DashUp() {
	// Add your cleanup here.
}

void DashUp::_process(double delta) {
	time_passed += delta;

	Vector2 new_position = Vector2(10.0 + (10.0 * Math::sin(time_passed * 2.0)), 10.0 + (10.0 * Math::cos(time_passed * 1.5)));

	set_position(new_position);
}
