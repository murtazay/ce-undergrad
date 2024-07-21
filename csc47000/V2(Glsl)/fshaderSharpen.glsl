#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_size;
uniform float     u_step;
uniform float     u_factor;

void main() {
    float t = 0.f;
    vec4 color = vec4(0.0f);
    for(float i = -u_size; i <= u_size; i+= u_step){
        for(float j = -u_size; j <= u_size; j+= u_step){
            color += texture2D(u_Sampler, vec2(v_TexCoord.x+j,v_TexCoord.y+i));
            t += 1.f;
        }
    }
    color = vec4(color.rgb/t,1.f);
    vec4 original = texture2D(u_Sampler,v_TexCoord);
    vec4 tmp = u_factor * (original - color);
    tmp += original;
    clamp(tmp,0.f,1.f);
    gl_FragColor = vec4(tmp.rgb,1.f);
}

