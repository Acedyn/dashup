#include "register_types.h"

#include "dashup.h"
#include "map/dashup_map.h"
#include "map/map_path.h"
#include "map/path_node.h"
#include "camera/dashup_camera.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_dashup_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(DashUpCamera);
	GDREGISTER_CLASS(DashUp);
	GDREGISTER_CLASS(DashUpMap);
	GDREGISTER_CLASS(MapPath);
	GDREGISTER_CLASS(PathNode);
}

void uninitialize_dashup_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT dashup_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_dashup_module);
	init_obj.register_terminator(uninitialize_dashup_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
