extends RigidBody2D

var is_dragging = false
var drag_from = Vector2.ZERO
var direction = Vector2.ZERO
var initial_position = 0.0
var current_time_scale = 1.0
var tween: Tween = create_tween()

@export
var camera: Camera2D

func _process(delta: float):
	Engine.set_time_scale(current_time_scale if is_dragging else 1.0)
	if camera != null:
		camera.zoom = Vector2(
			1.0 + (1-current_time_scale)*0.2,
			1.0 + (1-current_time_scale)*0.2,
		)
	
	var globals: Globals = get_node("/root/Globals")
	globals.score = max(
		globals.score,
		int((-global_position.y - initial_position)/10)
	)
	
func _input(event):
	if event is InputEventMouseButton:
		if event.pressed:
			if not is_dragging:
				tween.kill()
				tween = create_tween()
				tween.tween_property(self, "current_time_scale", 0.2, 0.2)
				
				is_dragging = true
				drag_from = event.position
		else:
			tween.kill()
			tween = create_tween()
			tween.tween_property(self, "current_time_scale", 1.0, 0.2)
			
			is_dragging = false
			direction = drag_from - event.position
			linear_velocity = direction * 2

	if event is InputEventMouseMotion:
		if is_dragging:
			direction = drag_from - event.position

func _ready() -> void:
	initial_position = -global_position.y
