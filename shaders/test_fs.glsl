#version 130
#define SPOTLIGHT_CUT 0.999f

//in vec3 normal;
in vec2 st;
//in vec3 light;
//in vec3 normal_eye;

in vec3 view_dir_tan;
in vec3 light_dir_tan;
in vec4 test_tan;

out vec4 frag_colour;
//uniform sampler2D basic_texture;
uniform float ConstA;
uniform float ConstD;
uniform float ConstS;
uniform sampler2D texsamp_rgb;
uniform sampler2D texsamp_normal;

#define SPECULAR_EXP 200

vec4 la = vec4(1.0, 1.0, 1.0, 1.0);
vec4 ld = vec4(1.0, 1.0, 1.0, 1.0);
vec4 ls = vec4(1.0, 1.0, 1.0, 1.0);

void main() {
	// material objeto

	vec4 ka = vec4(ConstA, ConstA, ConstA, 1.0);
	vec4 kd = vec4(ConstD, ConstD, ConstD, 1.0);
	vec4 ks = vec4(ConstS, ConstS, ConstS, 1.0);

	vec3 normal_tan = texture (texsamp_normal, st).rgb;
	vec3 texel      = texture (texsamp_rgb, st).rgb;
	normal_tan = normalize (normal_tan * 2.0 - 1.0);

    vec3 Ia = la.xyz * ka.xyz * texel.xyz;
	/*
	vec3 Id = dot(light, normal) * kd.xyz * ld.xyz * texel.xyz;

	vec3 r = -(light - (dot(light,normal)*2*normal));

	float dot_prod_specular = dot (normalize(r) , normalize (normal_eye));
	dot_prod_specular = max (dot_prod_specular, 0.0);
	float specular_factor = pow (dot_prod_specular, SPECULAR_EXP);

	vec3 Is = ls.xyz * ks.xyz * specular_factor * texel.xyz; */

	// diffuse light equation done in tangent space
	vec3 direction_to_light_tan = normalize (light_dir_tan);
	float dot_prod = dot (direction_to_light_tan, normal_tan);
	dot_prod = max (dot_prod, 0.0);
	//vec3 Id = vec3 (0.7, 0.7, 0.7) * vec3 (1.0, 0.5, 0.0) * dot_prod;
	vec3 Id = ld.rgb * kd.rgb * texel * dot_prod;

	// specular light equation done in tangent space
	vec3 reflection_tan = reflect (normalize (light_dir_tan), normal_tan);
	float dot_prod_specular = dot (reflection_tan, normalize (view_dir_tan));
	dot_prod_specular = max (dot_prod_specular, 0.0);
	float specular_factor = pow (dot_prod_specular, SPECULAR_EXP);
	//vec3 Is = vec3 (1.0, 1.0, 1.0) * vec3 (0.5, 0.5, 0.5) * specular_factor;
	vec3 Is = ls.rgb * ks.rgb * specular_factor;

	frag_colour = vec4(Ia+Id+Is,1.0);
}
 