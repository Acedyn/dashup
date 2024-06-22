@tool
extends Node

@export
var generate_soft_body: bool:
	set(value):
		clear_soft_body_nodes()
		generate_soft_body_nodes()

@export
var radius: float = 25
@export
var shape_point_count: int = 32
@export
var squeleton_point_count: int = 16

func clear_soft_body_nodes():
	for node in get_children():
		remove_child(node)

func generate_soft_body_nodes():
	# Create the shape
	var polygon_node = Polygon2D.new()
	add_child(polygon_node)
	polygon_node.name = "soft_body_shape"
	polygon_node.owner = owner

	var center_rigid_body: RigidBody2D = create_rigid_body(radius/4.0)
	center_rigid_body.name = "rbd_center"
	var rigid_bodies: Array[RigidBody2D] = []
	
	var polygon_points: PackedVector2Array = PackedVector2Array()
	for i in shape_point_count:
		var new_point = Vector2()
		var normalized_index = float(i)/float(shape_point_count)
		new_point.x = sin(normalized_index*(2*PI)) * radius
		new_point.y = cos(normalized_index*(2*PI)) * radius
		polygon_points.append(new_point)
	polygon_node.polygon = polygon_points
		
	var squeleton_radius = (2*PI*radius/squeleton_point_count)/2
	squeleton_radius = (2*PI*(radius-squeleton_radius)/squeleton_point_count)/2
	for i in squeleton_point_count:
		var new_point = Vector2()
		var normalized_index = float(i)/float(squeleton_point_count)
		new_point.x = sin(normalized_index*(2*PI)) * (radius-squeleton_radius)
		new_point.y = cos(normalized_index*(2*PI)) * (radius-squeleton_radius)
		var rigid_body = create_rigid_body(squeleton_radius, new_point)
		rigid_body.name = "rbd_" + str(i)
		rigid_bodies.append(rigid_body)
		
	var skeleton_node = Skeleton2D.new()
	add_child(skeleton_node)
	skeleton_node.owner = owner

	for i in len(rigid_bodies):
		var rigid_bodie: RigidBody2D = rigid_bodies[i]
		var normalized_index = float(i)/float(len(rigid_bodies))
		var damped_spring = DampedSpringJoint2D.new()
		rigid_bodie.add_child(damped_spring)
		damped_spring.reparent(self)
		damped_spring.owner = owner
		damped_spring.node_a = center_rigid_body.get_path()
		damped_spring.node_b = rigid_bodie.get_path()
		damped_spring.length = radius-squeleton_radius
		damped_spring.rest_length = radius-squeleton_radius
		damped_spring.name = "spring_center_" + rigid_bodie.name + "_" + str(i)
		damped_spring.rotation = PI - normalized_index*(2*PI)
		damped_spring.stiffness = 128
		damped_spring.damping = 32
		
		for j in len(rigid_bodies):
			if j == i:
				continue
	
			var around_damped_spring = DampedSpringJoint2D.new()
			var target_rigid_bodie: RigidBody2D = rigid_bodies[j]
			target_rigid_bodie.add_child(around_damped_spring)
			around_damped_spring.reparent(self)
			around_damped_spring.owner = owner
			around_damped_spring.node_a = target_rigid_bodie.get_path()
			around_damped_spring.node_b = rigid_bodie.get_path()
			around_damped_spring.length = target_rigid_bodie.position.distance_to(rigid_bodie.position)
			around_damped_spring.rest_length = target_rigid_bodie.position.distance_to(rigid_bodie.position)
			around_damped_spring.name = "spring_" + rigid_bodie.name + "_" + str(i)
			around_damped_spring.stiffness = 128
			around_damped_spring.damping = 32
			var direction = (rigid_bodie.position - target_rigid_bodie.position).normalized()
			around_damped_spring.rotation = Vector2(0.0, 1.0).angle_to(direction)
			
		var bone_node = Bone2D.new()
		skeleton_node.add_child(bone_node)
		bone_node.owner = owner
		bone_node.position = rigid_bodie.position
			
func create_rigid_body(radius: float, pos=Vector2.ZERO):
	# Create the rigidbody
	var rigidbody_node = RigidBody2D.new()
	add_child(rigidbody_node)
	rigidbody_node.name = "0"
	rigidbody_node.owner = owner
	rigidbody_node.position = pos
	
	var collision_node = CollisionShape2D.new()
	rigidbody_node.add_child(collision_node)
	var collision_node_shape = CircleShape2D.new()
	collision_node_shape.radius = radius
	collision_node.shape = collision_node_shape
	collision_node.name = "0"
	collision_node.owner = owner

	return rigidbody_node
