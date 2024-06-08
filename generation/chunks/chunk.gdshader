shader_type canvas_item;

// Global
uniform vec4 bounds;
uniform float testMix;
uniform vec3 colorA: source_color;
uniform vec3 colorB: source_color;
// Step
uniform float stepSize;
uniform float stepPow;
uniform float stepIn;
uniform float stepOut;
uniform float stepSmoothness;
// Wave
uniform float waveOffset;
uniform float waveAmplitude;
uniform float waveFrequency;

varying vec2 global_pos;

void vertex() {
	// Normalize the UVs
	UV = vec2(
		UV.x/320./2.+0.5,
		UV.y/48./2.+0.5
	);
	// Compute the global position
	global_pos = (MODEL_MATRIX * vec4(VERTEX, 0., 1.)).xy;
}

float compute_waves(float uvX, float globalY) {
	return uvX + (((sin((globalY)/waveFrequency) + 1.) / 2.)+waveOffset)*waveAmplitude*(1.0-uvX);
}
float compute_step(float uvX) {
	float smoothed =  pow(smoothstep(stepIn, stepOut, uvX), stepPow);
	float stepped = floor(smoothed*stepSize)/stepSize;
	return stepped;
}
float compute_smoothedstep(float uvX) {
	float smoothed =  pow(smoothstep(stepIn, stepOut, uvX), stepPow);
	float stepped = floor(smoothed*stepSize)/stepSize;
	return min(smoothed, stepped+stepSmoothness);
}

void fragment() {
	float lineSteps = compute_step(UV.x);
	float waves = compute_waves(UV.x, global_pos.y);
	float lineSmoothedsteps = compute_smoothedstep(waves);
	COLOR.xyz = mix(colorA, colorB, lineSmoothedsteps);
	COLOR.a = lineSmoothedsteps;
}

//void light() {
	// Called for every pixel for every light affecting the CanvasItem.
	// Uncomment to replace the default light processing function with this one.
//}
