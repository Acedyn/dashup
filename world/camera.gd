extends Camera2D

@export var follow_node: Node2D
@export var scroll_speed = 80.0
@export var delay = 0.4

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var follow_position = lerp(
		global_position,
		follow_node.global_position,
		delta/delay
	)
	
	global_position.x = follow_position.x
	global_position.y = min(follow_position.y, global_position.y - scroll_speed*delta)
