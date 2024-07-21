#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_xsize;      // uniform variable representing the amount of x-padding
uniform float     u_ysize;      // uniform variable representing the amount of y-padding
uniform float     u_xstep;      // uniform variable representing the amount of x-step
uniform float     u_ystep;      // uniform variable representing the amount of y-step

void main() {
    float t = 0.f;
    vec4 color = vec4(0.0f);
    // Visit all pixels in the neighbor hood
    for(float i = -u_ysize; i <= u_ysize; i+= u_ystep){
        for(float j = -u_xsize; j <= u_xsize; j+= u_xstep){
            color += texture2D(u_Sampler, vec2(v_TexCoord.x+j,v_TexCoord.y+i));
            t += 1.f;
        }
    }
    // divide by t
    color = color/t;
    //set it
    gl_FragColor = vec4(color.rgb,1.f);
}

