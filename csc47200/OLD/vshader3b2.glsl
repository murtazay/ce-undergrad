#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

attribute	vec3	a_Position;	// attribute variable: position vector
uniform	mat4	u_Modelview;	// uniform variable for passing modelview matrix
uniform	mat4	u_Projection;	// uniform variable for passing projection matrix

void main() {

	gl_Position = u_Projection * u_Modelview * vec4(a_Position, 1);

}
