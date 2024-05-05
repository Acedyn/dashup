extends Polygon2D

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var static_body = StaticBody2D.new()
	get_parent().add_child.call_deferred(static_body)
	var collision_polygon = CollisionPolygon2D.new()
	collision_polygon.polygon = polygon
	static_body.add_child.call_deferred(collision_polygon)
