
extends Node2D

@export var build_walls: bool:
	set(value):
		build_next_wall_generation()
		
@export var polygon_node_name: String = "Polygon2D1"
@export var initial_quandidates: Array[PackedScene] = []
@export var target_width: float = 500
var computed_quandidates: Dictionary = {}
var computed_quandidates_weight: float = 0.0

class LocalBounds:
	var topLeft: Vector2 = Vector2.ZERO
	var topRight: Vector2 = Vector2.ZERO
	var bottomLeft: Vector2 = Vector2.ZERO
	var bottomRight: Vector2 = Vector2.ZERO
	
class LocalQuandidate:
	var scene: PackedScene
	var slope: float = 0.0
	var transform: Transform2D = Transform2D.IDENTITY
	var local_bounds: LocalBounds
	var weight: float

func get_current_walls() -> Array[Node2D]:
	var current_walls: Array[Node2D] = [get_children()[0]]
	for child in get_children().slice(1):
		var current_height = current_walls[0].global_position.y
		if is_equal_approx(child.global_position.y, current_height):
			current_walls.append(child)
		elif child.global_position.y < current_height:
			current_walls = [child]
			
	return current_walls
	
func get_wall_bounds(wall: Node2D) -> LocalBounds:
	var polygon: Polygon2D = wall.get_node(polygon_node_name)
	var topPoints = PackedVector2Array([polygon.global_transform * polygon.polygon[0]])
	var bottomPoints = PackedVector2Array([polygon.global_transform * polygon.polygon[0]])
	
	for point in polygon.polygon.slice(1):
		point = polygon.global_transform * point
		if is_equal_approx(point.y, topPoints[0].y):
			topPoints.append(point)
		elif point.y < topPoints[0].y:
			topPoints = PackedVector2Array([point])
		if is_equal_approx(point.y, bottomPoints[0].y):
			bottomPoints.append(point)
		elif point.y > bottomPoints[0].y:
			bottomPoints = PackedVector2Array([point])
	
	var wallBounds: LocalBounds = LocalBounds.new()
	wallBounds.topLeft = topPoints[0]
	wallBounds.topRight = topPoints[0]
	wallBounds.bottomLeft = bottomPoints[0]
	wallBounds.bottomRight = bottomPoints[0]
	for point in topPoints.slice(1):
		if point.x < wallBounds.topLeft.x:
			wallBounds.topLeft = point
		elif point.x > wallBounds.topRight.x:
			wallBounds.topRight = point
			
	for point in bottomPoints.slice(1):
		if point.x < wallBounds.bottomLeft.x:
			wallBounds.bottomLeft = point
		elif point.x > wallBounds.bottomRight.x:
			wallBounds.bottomRight = point
			
	return wallBounds
	
func get_walls_opennings(walls: Array[Node2D]) -> Array[LocalBounds]:
	var sortedWallsBounds: Array[LocalBounds] = []
	for wall in walls:
		var wallBounds: LocalBounds = get_wall_bounds(wall)
		var newSortedWallsBounds: Array[LocalBounds] = []
		var inserted = false
		for sortedWallBounds in sortedWallsBounds:
			if wallBounds.topRight.x < sortedWallBounds.topLeft.x:
				newSortedWallsBounds.append(wallBounds)
				inserted = true
			newSortedWallsBounds.append(sortedWallBounds)
		
		if not inserted:
			newSortedWallsBounds.append(wallBounds)
			
		sortedWallsBounds = newSortedWallsBounds
				
	var openningsBounds: Array[LocalBounds] = []
	var currentBound = LocalBounds.new()
	for i  in len(sortedWallsBounds):
		var sortedWallBounds = sortedWallsBounds[i]
		if i == 0:
			currentBound.topLeft = sortedWallBounds.topRight
			currentBound.bottomLeft = sortedWallBounds.bottomRight
			continue
		
		currentBound.topRight = sortedWallBounds.topLeft
		currentBound.bottomRight = sortedWallBounds.bottomLeft
		openningsBounds.append(currentBound)
		currentBound = LocalBounds.new()
		currentBound.topLeft = sortedWallBounds.topRight
		currentBound.bottomLeft = sortedWallBounds.bottomRight
		
	return openningsBounds
	
func get_qandidates() -> Array[LocalQuandidate]:
	var quandidates: Array[LocalQuandidate] = []
	for initial_quandidate in initial_quandidates:
		var new_qandidate: LocalQuandidate
		new_qandidate = LocalQuandidate.new()
		new_qandidate.scene = initial_quandidate
		quandidates.append(new_qandidate)
		new_qandidate = LocalQuandidate.new()
		new_qandidate.scene = initial_quandidate
		new_qandidate.transform.y = Vector2(0.0, -1.0)
		quandidates.append(new_qandidate)
	
	return quandidates
	
func select_tile() -> Node2D:
	var quandidates: Array[LocalQuandidate] = get_qandidates()
	var index = randi_range(0, initial_quandidates.size() - 1)
	var quandidate = quandidates[index]
	var tile: Node2D = quandidate.scene.instantiate()
	add_child(tile)
	tile.owner = owner
	tile.transform = quandidate.transform
	return tile
	
func build_side_walls(openning: LocalBounds, left: bool):
	var tile = select_tile()
	
	if left:
		var tile_bounds = get_wall_bounds(tile)
		var offset = openning.topLeft - tile_bounds.bottomRight
		tile.global_position += offset
	if not left:
		tile.scale.x *= -1.0
		var tile_bounds = get_wall_bounds(tile)
		var offset = openning.topRight - tile_bounds.bottomLeft
		tile.global_position += offset
		
func build_next_wall_generation():
	var current_walls: Array[Node2D] = get_current_walls()
	var current_opennings: Array[LocalBounds] = get_walls_opennings(current_walls)
	build_side_walls(current_opennings[0], true)
	build_side_walls(current_opennings[-1], false)

func _process(_delta):
	var camera: Camera2D = get_parent().get_node("Camera2D")
	var current_walls: Array[Node2D] = get_current_walls()
	if camera.global_position.y < current_walls[0].global_position.y:
		build_next_wall_generation()
