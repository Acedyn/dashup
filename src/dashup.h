#ifndef DASHUP_H
#define DASHUP_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class DashUp : public Sprite2D {
	GDCLASS(DashUp, Sprite2D)

private:
	double time_passed;

protected:
	static void _bind_methods();

public:
	DashUp();
	~DashUp();

	void _process(double delta) override;
};

}

#endif
