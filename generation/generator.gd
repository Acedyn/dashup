extends Node2D

@export var quandidates: Array[Resource] = []
@export var target_oppenning_size: float = 200.0
@export var openning_offset_bias: float = 1.0
@export var target_oppenning_slope: float = 0.0
@export var openning_slope_bias: float = 1.0
@export var min_openning_size: float = 50.0
@export var camera: Camera2D
var current_opennings: Array[Bounds] = []
var top_walls: TopWalls

class TopWalls:
	var left: Wall
	var right: Wall

# Find the exiting nodes that could be the top walls
func get_top_walls() -> TopWalls:
	assert(get_children().size() > 0)
	var top_walls = TopWalls.new()
	var all_top_walls: Array[Wall] = [
		Wall.new(get_children()[0], Quandidate.from_node(get_children()[0]))
	]
	
	for child in get_children().slice(1):
		var current_height = all_top_walls[0].node.global_position.y
		if is_equal_approx(child.global_position.y, current_height):
			all_top_walls.append(Wall.new(child, Quandidate.from_node(child)))
		elif child.global_position.y < current_height:
			all_top_walls = [Wall.new(child, Quandidate.from_node(child))]

	all_top_walls.sort_custom(
		func(a, b): return a.node.global_position.x < b.node.global_position.x
	)
	
	top_walls.left = all_top_walls[0]
	top_walls.right = all_top_walls[-1]
	return top_walls
	
# Select the left and right wall for an openning
func select_openning_quandidate(quandidates: Array[Quandidate], openning: Bounds) -> Quandidate:
	# Weight the quandidates by comparing their slope and the target openning size
	var openning_size = openning.topLeft.distance_to(openning.topRight)
	
	# We are dividing the size by two because there is two walls
	var target_size_offset = (target_oppenning_size - openning_size) / 2
	for quandidate in quandidates:
		var size_offset = (quandidate.openning_offset - target_size_offset) * self.openning_offset_bias
		var slope_offset = (quandidate.slope - target_oppenning_slope) * self.openning_slope_bias
		quandidate.weight -= abs(size_offset)
		quandidate.weight -= abs(slope_offset)
		pass
		
	# The minimum weight should not be under zero
	var min_weight = quandidates.reduce(func(a, i): return a if a < i.weight else i.weight, 0)
	if min_weight < 0:
		for quandidate in quandidates:
			quandidate.weight -= min_weight
		
	# Select the quandidates randomly by weight
	var total_weight = quandidates.reduce(func(a, i): return a+i.weight, 0)
	var random = randf() * total_weight
	var weight_accumulator = 0.0
	for quandidate in quandidates:
		weight_accumulator += quandidate.weight
		if random <= weight_accumulator:
			return quandidate
			
	print("ERROR: The weighted random selection failed")
	return quandidates[-1]
	
# Select and place all the walls to fill a new step in the level
func place_next_walls_generation() -> Array[Wall]:
	var new_top_walls: TopWalls = TopWalls.new()
		
	# Duplicate the quandidates so we can modify their values for this generation only
	var generation_quandidates: Array[Quandidate] = []
	for quandidate in self.quandidates:
		generation_quandidates.append(quandidate.duplicate())
		
	for openning in self.current_opennings:
		# Place the left wall of the openning
		var left_quandidate: Quandidate = self.select_openning_quandidate(
			generation_quandidates.filter(func(q): return q.type != Quandidate.QuandidateType.RIGHT),
			openning,
		)
		var left_node: Node2D = left_quandidate.scene.instantiate()
		add_child(left_node)
		left_node.global_transform *= left_quandidate.transform
		var left_node_offet = openning.topLeft - left_quandidate.get_bounds().bottomRight
		left_node.global_position += left_node_offet
		# TODO: Find a way to not have to create a quandidate from scratch every time
		new_top_walls.left = Wall.new(left_node, Quandidate.from_node(left_node))
		add_child(new_top_walls.left)
		new_top_walls.left.merge_with_bottom(self.top_walls.left, 10.0)
		
		# Place the right wall of the openning
		var right_quandidate = self.select_openning_quandidate(
			generation_quandidates.filter(func(q): return q.type != Quandidate.QuandidateType.LEFT),
			openning,
		)
		var right_node: Node2D = right_quandidate.scene.instantiate()
		add_child(right_node)
		right_node.global_transform *= right_quandidate.transform
		var right_node_offet = openning.topRight - right_quandidate.get_bounds().bottomLeft
		right_node.global_position += right_node_offet
		# TODO: Find a way to not have to create a quandidate from scratch every time
		new_top_walls.right = Wall.new(right_node, Quandidate.from_node(right_node))
		add_child(new_top_walls.right)
		new_top_walls.right.merge_with_bottom(self.top_walls.right, 10.0)
		
	self.current_opennings = Bounds.from_openning(
		[new_top_walls.left, new_top_walls.right].map(func(x): return x.quandidate.get_bounds())
	)
	
	self.top_walls = new_top_walls
	return [new_top_walls.left, new_top_walls.right]
	
func fill_screen():
	var max_height = camera.global_position.y - get_viewport_rect().get_center().y
	while self.current_opennings[0].topLeft.y > max_height:
		var previous_opennings = self.current_opennings
		var previous_top_walls = self.top_walls
		var placed_walls = self.place_next_walls_generation()
		var current_openning_size = (
			self.current_opennings[0].topRight.x - self.current_opennings[0].topLeft.x
		)
		if current_openning_size < min_openning_size:
			self.current_opennings = previous_opennings
			self.top_walls = previous_top_walls
			for placed_wall in placed_walls:
				placed_wall.node.free()
			print("WARNING: GENERATION CANCELED")
	
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	self.top_walls = self.get_top_walls()
	self.current_opennings = Bounds.from_openning(
		[top_walls.left, top_walls.right].map(func(x): return x.quandidate.get_bounds())
	)
	
	fill_screen()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	fill_screen()
