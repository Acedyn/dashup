@tool
extends Resource
class_name Quandidate

enum QuandidateType {LEFT, RIGHT}
	
@export var analyse: bool = false:
	set(value):
		if Engine.is_editor_hint():
			analyse_mesh_from_scene()
		
@export var scene: PackedScene
@export var polygon_node_name: String = "Polygon2D1"
# How often this quandidate should be selected
@export var weight: float = 1.0
# How the quandidate will affect the openning, positive slope should enlarge
# the openning wile negative slope should narrow it
@export var slope: float = 0.0
# How the quandidate will affect the openning, positive offset should enlarge
# the openning wile negative offset should narrow it
@export var openning_offset: float = 0.0
# Transform to apply on the polygon mesh before analysis
@export var transform: Transform2D = Transform2D.IDENTITY
# The type will define where this quandidate can be placed for an openning
@export var type: QuandidateType = QuandidateType.LEFT
# Since gdscript does not support exported custom classes, we store the bounds into an array
@export var internal_bounds: Array[Vector2] = []

# Initialize a quandidate from a node and analyse its structure
static func from_node(node: Node2D, polygon_node_name: String = "Polygon2D1") -> Quandidate:
	var quandidate = Quandidate.new()
	quandidate.scene = load(node.scene_file_path)
	quandidate.polygon_node_name = polygon_node_name
	quandidate.analyse_mesh_from_node(node.get_node(polygon_node_name))
	return quandidate

# Compute the bounds of the scene and save it in the ressource
func analyse_mesh_from_scene():
	assert(scene != null)
	var chunk_node = scene.instantiate()
	var polygon_node = chunk_node.get_node(polygon_node_name)
	self.analyse_mesh_from_node(polygon_node)
	
# Compute the bounds of an already instansiated scene and save it in the ressource
func analyse_mesh_from_node(polygon_node: Node2D):
	assert(polygon_node.is_class("Polygon2D"))
	self.set_bounds(Bounds.from_polygon(polygon_node, self.transform))
	
	# There is a different treatment if the quandidate is a left or right quandidate
	# because the idea is that an openning_offet and a slope that enlarge the openning
	# should be positive and ones that narrow it should be negative
	if self.type == QuandidateType.LEFT:
		self.openning_offset = self.get_bounds().bottomRight.x - self.get_bounds().topRight.x
		self.slope = acos(
			(self.get_bounds().topRight - self.get_bounds().bottomRight)
			.normalized()
			.dot(Vector2(0.0, -1.0))
		) * 180/PI
		if self.openning_offset > 0:
			self.slope = 0 - self.slope
			
	elif self.type == QuandidateType.RIGHT:
		self.openning_offset = self.get_bounds().topLeft.x - self.get_bounds().bottomLeft.x
		self.slope = acos(
			(self.get_bounds().topLeft - self.get_bounds().bottomLeft)
			.normalized()
			.dot(Vector2(0.0, -1.0))
		) * 180/PI
	
		if self.openning_offset < 0:
			self.slope = 0 - self.slope

# Apply the transform on the slope and the bounds
func apply_transform(transform: Transform2D):
	self.bounds.apply_transform(transform)
	self.slope *= transform.x.x

# Getter since gdscript does not support exported values for custom classes
func get_bounds() -> Bounds:
	var bounds: Bounds = Bounds.new()
	bounds.topLeft = self.internal_bounds[0]
	bounds.topRight = self.internal_bounds[1]
	bounds.bottomLeft = self.internal_bounds[2]
	bounds.bottomRight = self.internal_bounds[3]
	return bounds
	
# Setter since gdscript does not support exported values for custom classes
func set_bounds(bounds: Bounds):
	self.internal_bounds = []
	self.internal_bounds.append(bounds.topLeft)
	self.internal_bounds.append(bounds.topRight)
	self.internal_bounds.append(bounds.bottomLeft)
	self.internal_bounds.append(bounds.bottomRight)
