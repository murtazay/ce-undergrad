#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_scale;

void main() {
    gl_FragColor = texture2D(u_Sampler, v_TexCoord);
    gl_FragColor = (u_scale) * floor(gl_FragColor/u_scale);
}
