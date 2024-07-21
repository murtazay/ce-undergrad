//precision mediump float;
attribute vec3	a_Position;	// attribute variable: position vector
attribute vec2	a_TexCoord;	// attribute variable: texture coordinate
varying   vec2	v_TexCoord;	// varying variable for passing texture coordinate to fragment shader
uniform   mat4  u_Model;	// uniform variable for modelview matrix
uniform   mat4  u_View;
uniform   mat4  u_Projection;	// uniform variable for projection matrix

void main() {

        gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1);
	v_TexCoord = a_TexCoord;
}
