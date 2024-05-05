extends RigidBody2D

var is_dragging = false
var drag_from = Vector2.ZERO
var direction = Vector2.ZERO
	
func _input(event):
	if event is InputEventMouseButton:
		if event.pressed:
			if not is_dragging:
				is_dragging = true
				drag_from = event.position
		else:
			is_dragging = false
			direction = drag_from - event.position
			linear_velocity = direction * 2

	if event is InputEventMouseMotion:
		if is_dragging:
			direction = drag_from - event.position
