#version 410

in vec3 eye_position, eye_normal;
in vec2 texCoord;
uniform sampler2D theTexture;

uniform mat4 view;
uniform vec3 Ka, Kd, Ks;
uniform float Ns;

// Light source from world - e.g. lamp
vec3 light_position_world  = vec3 (0.0f, 11.5, 20.0);
vec3 La = vec3 (0.2, 0.2, 0.2); // dark grey ambient colour, near darkness
vec3 Ld = vec3 (0.8, 0.8, 0.8); // dull white diffuse light colour
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour

out vec4 color; // final color of surface

void main () {

	// Ambient lightning multiplied by surface reflectance
	vec3 Ia = La * Ka;

	vec3 n_eye = normalize(eye_normal);

	// Diffuse lightning according to eye position

	// Raise light source to eye space
	vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));

	// Measure distance from light source to eye
	vec3 distance_to_light_eye = light_position_eye - eye_position;

	// Check direction from eye to light source
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);

	float dot_prod = dot (direction_to_light_eye, n_eye);

	// If dot product is negative, should be 0.0
	dot_prod = max(dot_prod, 0.0);

	// Defines diffuse contribution
	vec3 Id = Ld * Kd * dot_prod;

	// Calculates specular intensity
	// Normalize eye position related to the surface
	vec3 surface_to_viewer_eye = normalize(-eye_position);

	vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);

    // Calculates dot product
    float dot_prod_specular = dot(half_way_eye, n_eye);
    dot_prod_specular = max (dot (half_way_eye, n_eye), 0.0);

	float specular_factor = pow (dot_prod_specular, Ns);

    // Defines final specular intensity
	vec3 Is = Ls * Ks * specular_factor; // final specular intensity

	// Final color: texture adjusted by lightning
	color = texture(theTexture, texCoord) * vec4 (Id + Ia + Is, 1.0);
}