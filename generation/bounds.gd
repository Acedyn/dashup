extends Node
class_name Bounds

@export var topLeft: Vector2 = Vector2.ZERO
@export var topRight: Vector2 = Vector2.ZERO
@export var bottomLeft: Vector2 = Vector2.ZERO
@export var bottomRight: Vector2 = Vector2.ZERO

# Compute the bounds of using the given node
static func from_polygon(polygon_node: Polygon2D) -> Bounds:
	var bounds: Bounds = Bounds.new()
	
	# Get the polygon points
	var polygon_points = polygon_node.polygon
	assert(polygon_points.size() > 0)
	
	# First get the top points, make sure the top part is
	# at least composed of two points
	var topPoints = PackedVector2Array([polygon_node.polygon[0]])
	var bottomPoints = PackedVector2Array([polygon_node.polygon[0]])
	for point in polygon_node.polygon.slice(1):
		if is_equal_approx(point.y, topPoints[0].y):
			topPoints.append(point)
		elif point.y < topPoints[0].y:
			topPoints = PackedVector2Array([point])
		if is_equal_approx(point.y, bottomPoints[0].y):
			bottomPoints.append(point)
		elif point.y > bottomPoints[0].y:
			bottomPoints = PackedVector2Array([point])
			
	assert(topPoints.size() >= 2)
	assert(bottomPoints.size() >= 2)
	
	# Initialize the top and botom points
	bounds.topLeft = topPoints[0]
	bounds.topRight = topPoints[0]
	bounds.bottomLeft = bottomPoints[0]
	bounds.bottomRight =  bottomPoints[0]
	
	# Iterate and find the points
	for point in topPoints.slice(1):
		if point.x < bounds.topLeft.x:
			bounds.topLeft = point
		elif point.x > bounds.topRight.x:
			bounds.topRight = point
			
	for point in bottomPoints.slice(1):
		if point.x < bounds.bottomLeft.x:
			bounds.bottomLeft = point
		elif point.x > bounds.bottomRight.x:
			bounds.bottomRight = point
			
	bounds.apply_transform(polygon_node.global_transform)
	return bounds
	
# Compute the opennings bounds between other bounds
static func from_openning(wall_bounds: Array) -> Array[Bounds]:
	assert(wall_bounds.size() >= 2)
	var opennings: Array[Bounds] = []
	wall_bounds.sort_custom(func(a, b): return a.topLeft < b.topRight)
	
	var current_openning: Bounds = Bounds.new()
	for i  in len(wall_bounds):
		var bounds = wall_bounds[i]
		if i == 0:
			current_openning.topLeft = bounds.topRight
			current_openning.bottomLeft = bounds.bottomRight
			continue
		
		current_openning.topRight = bounds.topLeft
		current_openning.bottomRight = bounds.bottomLeft
		opennings.append(current_openning)
		current_openning = Bounds.new()
		current_openning.topLeft = bounds.topRight
		current_openning.bottomLeft = bounds.bottomRight
		
	return opennings

# Apply the transform on the given points
func apply_transform(transform: Transform2D):
	self.topLeft = transform * self.topLeft
	self.topRight = transform * self.topRight
	self.bottomLeft = transform * self.bottomLeft
	self.bottomRight = transform * self.bottomRight
