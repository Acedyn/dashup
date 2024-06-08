extends Node
class_name Wall

@export var node: Node2D
@export var quandidate: Quandidate

func _init(node: Node2D, quandidate: Quandidate):
	self.node = node
	self.quandidate = quandidate
	
# Merging will create a bevel betwen the two connecting corners
func merge_with_bottom(bottom: Wall, bevel_size: float):
	var top_polygon_node: Polygon2D = self.node.get_node(self.quandidate.polygon_node_name)
	var bottom_polygon_node: Polygon2D = bottom.node.get_node(bottom.quandidate.polygon_node_name)
	
	var new_top_points_map: Array[Array] = []
	var new_bottom_points_map: Array[Array] = []
	
	# Create a copy if the points into an array of array to insert points later
	for top_point in top_polygon_node.polygon:
		new_top_points_map.append([top_point])
	for bottom_point in bottom_polygon_node.polygon:
		new_bottom_points_map.append([bottom_point])
	
	for top_index in top_polygon_node.polygon.size():
		var top_point = top_polygon_node.global_transform * top_polygon_node.polygon[top_index]
		var added_top_points: PackedVector2Array = PackedVector2Array()
		
		for bottom_index in bottom_polygon_node.polygon.size():
			var bottom_point = bottom_polygon_node.global_transform * bottom_polygon_node.polygon[bottom_index]
			
			if not top_point.is_equal_approx(bottom_point):
				continue
				
			var top_previous = (top_index - 1) % top_polygon_node.polygon.size()
			var top_next = (top_index + 1) % top_polygon_node.polygon.size()
			var top_previous_pos = top_polygon_node.global_transform * top_polygon_node.polygon[top_previous]
			var top_next_pos = top_polygon_node.global_transform * top_polygon_node.polygon[top_next]
			var bottom_previous = (bottom_index - 1) % bottom_polygon_node.polygon.size()
			var bottom_next = (bottom_index + 1) % bottom_polygon_node.polygon.size()
			var bottom_previous_pos = bottom_polygon_node.global_transform * bottom_polygon_node.polygon[bottom_previous]
			var bottom_next_pos = bottom_polygon_node.global_transform * bottom_polygon_node.polygon[bottom_next]
			
			var top_slope = top_next
			var top_slope_pos = top_next_pos
			var top_flat = top_previous
			var top_flat_pos = top_previous_pos
			if top_previous_pos.y < top_next_pos.y:
				top_slope = top_previous
				top_flat = top_next
				top_slope_pos = top_previous_pos
				top_flat_pos = top_next_pos
				
			var bottom_slope = bottom_next
			var bottom_flat = bottom_previous
			var bottom_slope_pos = bottom_next_pos
			var bottom_flat_pos = bottom_previous_pos
			if bottom_previous_pos.y > bottom_next_pos.y:
				bottom_slope = bottom_previous
				bottom_flat = bottom_next
				bottom_slope_pos = bottom_previous_pos
				bottom_flat_pos = bottom_next_pos
			
			var direction_top = (top_slope_pos - top_point).normalized()
			var perpendicular_top = Vector2(-direction_top.y, direction_top.x)
			var direction_bottom = (bottom_slope_pos - bottom_point).normalized()
			var perpendicular_bottom = Vector2(direction_bottom.y, -direction_bottom.x)
			
			if (top_slope_pos + (bottom_slope_pos - top_slope_pos)/2).x < top_point.x:
				perpendicular_top = Vector2(direction_top.y, -direction_top.x)
				perpendicular_bottom = Vector2(-direction_bottom.y, direction_bottom.x)
			
			var intersection = Geometry2D.line_intersects_line(
				top_point + direction_top*bevel_size,
				perpendicular_top*10000,
				bottom_point + direction_bottom*bevel_size,
				perpendicular_bottom*1000,
			)
			
			if intersection == null or intersection.distance_to(top_point) > 1000:
				continue
				
			var radius = intersection.distance_to(top_point+direction_top*bevel_size)
			var top_point_offset = top_point - (intersection+(top_point-intersection).normalized()*radius)
			if top_slope == top_previous:
				new_top_points_map[top_index] = [
					#top_polygon_node.global_transform.inverse() * (top_point + direction_top*bevel_size),
					#top_polygon_node.global_transform.inverse() * (intersection+(top_point-intersection).normalized()*radius),
					top_polygon_node.global_transform.inverse() * ((top_point + direction_top*bevel_size) + top_point_offset),
					top_polygon_node.global_transform.inverse() * top_point,
				]
			else:
				new_top_points_map[top_index] = [
					#top_polygon_node.global_transform.inverse() * (intersection+(top_point-intersection).normalized()*radius),
					#top_polygon_node.global_transform.inverse() * (top_point + direction_top*bevel_size),
					top_polygon_node.global_transform.inverse() * top_point,
					top_polygon_node.global_transform.inverse() * ((top_point + direction_top*bevel_size) + top_point_offset),
				]
			
			if bottom_slope == bottom_previous:
				new_bottom_points_map[bottom_index] = [
					#bottom_polygon_node.global_transform.inverse() * (bottom_point + direction_bottom*bevel_size),
					#bottom_polygon_node.global_transform.inverse() * (intersection+(top_point-intersection).normalized()*radius),
					bottom_polygon_node.global_transform.inverse() * ((bottom_point + direction_bottom*bevel_size) + top_point_offset),
					bottom_polygon_node.global_transform.inverse() * top_point,
				]
			else:
				new_bottom_points_map[bottom_index] = [
					#bottom_polygon_node.global_transform.inverse() * (intersection+(top_point-intersection).normalized()*radius),
					#bottom_polygon_node.global_transform.inverse() * (bottom_point + direction_bottom*bevel_size),
					bottom_polygon_node.global_transform.inverse() * top_point,
					bottom_polygon_node.global_transform.inverse() * ((bottom_point + direction_bottom*bevel_size) + top_point_offset),
				]
			
			break

	var new_top_points: PackedVector2Array = PackedVector2Array()
	var new_bottom_points: PackedVector2Array = PackedVector2Array()
	
	for new_top_map_points in new_top_points_map:
		for new_top_point in new_top_map_points:
			new_top_points.append(new_top_point)
	for new_bottom_map_points in new_bottom_points_map:
		for new_bottom_point in new_bottom_map_points:
			new_bottom_points.append(new_bottom_point)
			
	top_polygon_node.polygon = new_top_points
	bottom_polygon_node.polygon = new_bottom_points
