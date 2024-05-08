shader_type canvas_item;
uniform float offset;
uniform float frequency;
uniform float size;
uniform float threshold;

void vertex() {
	// Called for every vertex the material is visible on.
	UV = COLOR.xy;
}

void fragment() {
	float transition = (sin(pow(UV.x, size)*frequency+offset)+1.0)/2.0;
	COLOR.xyz = mix(vec3(0.6, 0.4, 1.0), vec3(0.4, 0.6, 1.0), transition);
	if(UV.x > threshold) {
		COLOR.xyz = vec3(0.0);
	}
}

//void light() {
	// Called for every pixel for every light affecting the CanvasItem.
	// Uncomment to replace the default light processing function with this one.
//}
