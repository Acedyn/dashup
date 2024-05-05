@tool
extends Polygon2D

@export var radius = 5.0:
	set(value):
		radius = value
		update_circle()
@export var vertex_count:int:
	set(value):
		vertex_count = value
		update_circle()

func update_circle():
	var new_polygons = PackedVector2Array()
	for vertex in vertex_count:
		var vertex_angle = deg_to_rad(360/vertex_count * (vertex+1))
		var vertex_position = Vector2(sin(vertex_angle), cos(vertex_angle)) * radius
		new_polygons.append(vertex_position)

	polygon = new_polygons
