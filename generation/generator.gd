extends Node2D

@export var quandidates: Array[Resource] = []
@export var target_oppenning_size: float = 200.0
var current_opennings: Array[Bounds] = []

# Representation of a placed wall with its node and quandidate
class Wall:
	var node: Node2D
	var quandidate: Quandidate
	
	func _init(node: Node2D, quandidate: Quandidate):
		self.node = node
		self.quandidate = quandidate

# Find the exiting nodes that could be the top walls
func get_top_walls() -> Array[Wall]:
	assert(get_children().size() > 0)
	var top_walls: Array[Wall] = [
		Wall.new(get_children()[0], Quandidate.from_node(get_children()[0]))
	]
	
	for child in get_children().slice(1):
		var current_height = top_walls[0].node.global_position.y
		if is_equal_approx(child.global_position.y, current_height):
			top_walls.append(Wall.new(child, Quandidate.from_node(child)))
		elif child.global_position.y < current_height:
			top_walls = [Wall.new(child, Quandidate.from_node(child))]

	return top_walls
	
# Select the left and right wall for an openning
func select_openning_quandidate(quandidates: Array[Quandidate], openning: Bounds):
	# Weight the quandidates by comparing their slope and the target openning size
	var openning_size = openning.topLeft.distance_to(openning.topRight)
	var target_size_offset = abs(openning_size - target_oppenning_size)
	for quandidate in quandidates:
		var predicted_size_offset = quandidate.bounds.topLeft - quandidate.bounds.bottomLeft
		if quandidate.type == Quandidate.QuandidateType.LEFT:
			predicted_size_offset = quandidate.bounds.topRight - quandidate.bounds.bottomRight
			
		# We are dividing the size by two because there is two walls
		var weight_offset = predicted_size_offset/(target_size_offset/2)
		if weight_offset > 1:
			pass
			
		quandidate.weight -= weight_offset
	
# Select and place all the walls to fill a new step in the level
func place_next_walls_generation():
	var generation_quandidates: Array[Quandidate] = quandidates.duplicate(true)
	for openning in self.current_opennings:
		var left_quandidates = self.select_openning_quandidate(
			generation_quandidates.filter(func(q): return q.type != Quandidate.QuandidateType.LEFT),
			openning,
		)
		var right_quandidates = self.select_openning_quandidate(
			generation_quandidates.filter(func(q): return q.type != Quandidate.QuandidateType.RIGHT),
			openning,
		)
	
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var top_walls: Array[Wall] = self.get_top_walls()
	self.current_opennings = Bounds.from_openning(
		top_walls.map(func(x): return x.quandidate.bounds)
	)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
