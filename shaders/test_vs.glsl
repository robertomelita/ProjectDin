#version 130

// valores de entrada, cada thread recibe uno distinto.
in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 texture_coord;
in vec4 vtangent;

// variables comunes para todos los shaders
uniform mat4 view, proj, model;
uniform vec3 pos_global;

// variables de salida, cada shader genera un valor distinto
/*
out vec3 light;*/
out vec3 normal; 


out vec2 st;
out vec3 normal_eye;
out vec3 position_eye;
out vec4 test_tan;
out vec3 view_dir_tan;
out vec3 light_dir_tan;
out vec3 hele;

float inverse(float m);
mat2 inverse(mat2 m);
mat3 inverse(mat3 m);
mat4 inverse(mat4 m);

void main(){
/*	vec3 light_dir = vec3(0.5f, 1.0f, 0.5f);
    light = (view * vec4(light_dir, 0.0f)).xyz;
	st = texture_coord;
	normal = (view * model * vec4(vertex_normal, 0.0)).xyz;
	gl_Position = proj * view * model * vec4(vertex_position, 1.0);
	normal_eye = vec3(view * model * vec4(vertex_normal, 0.0)); */
	gl_Position = proj * view * model * vec4 (vertex_position, 1.0);
	st = texture_coord;
	test_tan = vtangent;
	

	vec3 cam_pos_wor = (inverse (view) * vec4 (0.0, 0.0, 0.0, 1.0)).xyz;
	vec3 light_dir_wor = normalize(vec3 (-1.5f, -1.0f, -1.5f));
	

	vec3 bitangent = cross (vertex_normal, vtangent.xyz) * vtangent.w;
	
	vec3 cam_pos_loc = vec3 (inverse (model) * vec4 (cam_pos_wor, 1.0));
	vec3 light_dir_loc = vec3 (inverse (model) * vec4 (light_dir_wor, 0.0));
	vec3 view_dir_loc = normalize (cam_pos_loc - vertex_position);

    hele = (view * model * vec4(vertex_position,1.0)).xyz;

    gl_Position = proj * view * model * vec4(vertex_position, 1.0);
	
	view_dir_tan = vec3 (
		dot (vtangent.xyz, view_dir_loc),
		dot (bitangent, view_dir_loc),
		dot (vertex_normal, view_dir_loc)
	);
	light_dir_tan = vec3 (
		dot (vtangent.xyz, light_dir_loc),
		dot (bitangent, light_dir_loc),
		dot (vertex_normal, light_dir_loc)
	);
}

float inverse(float m) {
    return 1.0 / m;
}

mat2 inverse(mat2 m) {
    return mat2(m[1][1],-m[0][1], -m[1][0], m[0][0]) / (m[0][0]*m[1][1] - m[0][1]*m[1][0]);
}

mat3 inverse(mat3 m) {
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float b01 = a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 = a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
                b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
                b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

mat4 inverse(mat4 m) {
  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
        a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
        a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
        a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

      b00 = a00 * a11 - a01 * a10,
      b01 = a00 * a12 - a02 * a10,
      b02 = a00 * a13 - a03 * a10,
      b03 = a01 * a12 - a02 * a11,
      b04 = a01 * a13 - a03 * a11,
      b05 = a02 * a13 - a03 * a12,
      b06 = a20 * a31 - a21 * a30,
      b07 = a20 * a32 - a22 * a30,
      b08 = a20 * a33 - a23 * a30,
      b09 = a21 * a32 - a22 * a31,
      b10 = a21 * a33 - a23 * a31,
      b11 = a22 * a33 - a23 * a32,

      det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  return mat4(
      a11 * b11 - a12 * b10 + a13 * b09,
      a02 * b10 - a01 * b11 - a03 * b09,
      a31 * b05 - a32 * b04 + a33 * b03,
      a22 * b04 - a21 * b05 - a23 * b03,
      a12 * b08 - a10 * b11 - a13 * b07,
      a00 * b11 - a02 * b08 + a03 * b07,
      a32 * b02 - a30 * b05 - a33 * b01,
      a20 * b05 - a22 * b02 + a23 * b01,
      a10 * b10 - a11 * b08 + a13 * b06,
      a01 * b08 - a00 * b10 - a03 * b06,
      a30 * b04 - a31 * b02 + a33 * b00,
      a21 * b02 - a20 * b04 - a23 * b00,
      a11 * b07 - a10 * b09 - a12 * b06,
      a00 * b09 - a01 * b07 + a02 * b06,
      a31 * b01 - a30 * b03 - a32 * b00,
      a20 * b03 - a21 * b01 + a22 * b00) / det;
}