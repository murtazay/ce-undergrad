#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_pad;      // uniform variable representing the amount of padding
uniform float     u_step;      // uniform variable representing the amount of step
uniform float     u_topl;
uniform float     u_topm;
uniform float     u_topr;
uniform float     u_left;
uniform float     u_mid;
uniform float     u_right;
uniform float     u_botl;
uniform float     u_botm;
uniform float     u_botr;

void main() {
    int t = -1;
    vec4 colors[9];
    // Visit all pixels in the neighbor hood
    for(float i = -u_pad; i <= u_pad; i+= u_step){
        for(float j = -u_pad; j <= u_pad; j+= u_step){
            t += 1;
            colors[t] = texture2D(u_Sampler, vec2(v_TexCoord.x+j,v_TexCoord.y+i));
        }
    }
    vec4 color = vec4(0.f);
    //multiply and add to a vec4
    color += colors[0] * u_topl;
    color += colors[1] * u_topm;
    color += colors[2] * u_topr;
    color += colors[3] * u_left;
    color += colors[4] * u_mid;
    color += colors[5] * u_right;
    color += colors[6] * u_botl;
    color += colors[7] * u_botm;
    color += colors[8] * u_botr;
    // Blur
    if(u_topl == 1.f && u_topm == 1.f && u_topr == 1.f &&
       u_left == 1.f && u_mid == 1.f && u_right == 1.f &&
       u_botl == 1.f && u_botm == 1.f && u_botr == 1.f){
        color = color/9.f;
    }
    // Gaussian
    if(u_topl == 1.f && u_topm == 2.f && u_topr == 1.f &&
       u_left == 2.f && u_mid == 4.f && u_right == 2.f &&
       u_botl == 1.f && u_botm == 2.f && u_botr == 1.f){
        color = color/16.f;
    }
    //set it
    gl_FragColor = vec4(color.rgb,1.f);
}

