@tool
extends Resource
class_name Quandidate

enum QuandidateType {LEFT, RIGHT}
	
@export var analyse: bool = false:
	set(value):
		analyse_mesh_from_scene()
		
@export var scene: PackedScene
@export var polygon_node_name: String = "Polygon2D1"
@export var weight: float = 1.0
@export var slope: float = 0.0
@export var transform: Transform2D = Transform2D.IDENTITY
@export var type: QuandidateType = QuandidateType.LEFT
var bounds: Bounds

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
	self.bounds = Bounds.from_polygon(polygon_node)
	self.bounds.apply_transform(self.transform)
	self.slope = acos(
		(bounds.topRight - bounds.bottomRight)
		.normalized()
		.dot(Vector2(0.0, -1.0))
	) * 180/PI * self.transform.x.x

# Apply the transform on the slope and the bounds
func apply_transform(transform: Transform2D):
	self.bounds.apply_transform(transform)
	self.slope *= transform.x.x
