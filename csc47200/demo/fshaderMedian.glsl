#version 130
#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_pad;
uniform int       u_size;
uniform float     u_step;

// SORT function
void CSWAP(inout vec4 a,inout vec4 b){
    vec4 t = min(a,b);
    b = a + b - t;
    a = t;
}

void main() {
    int k = 0;
    // for 3x3
    if(u_size == 9){
        vec4 colors[9];
        for(float i = -u_pad; i <= u_pad; i+= u_step){
            for(float j = -u_pad; j <= u_pad; j+= u_step){
                colors[k] = texture2D(u_Sampler,vec2(v_TexCoord.x+j,v_TexCoord.y+i));
                k++;
            }
        }
        for(int i = 0; i < 9; ++i){
            for(int j = 0; j < 7; ++j){
                CSWAP(colors[j],colors[j+1]);
            }
        }

        gl_FragColor = colors[4];
    }
    // for 5x5
    else if(u_size == 25){
        vec4 colors[25];
        for(float i = -u_pad; i <= u_pad; i+= u_step){
            for(float j = -u_pad; j <= u_pad; j+= u_step){
                colors[k] = texture2D(u_Sampler,vec2(v_TexCoord.x+j,v_TexCoord.y+i));
                k++;
            }
        }
        for(int i = 0; i < 25; ++i){
            for(int j = 0; j < 23; ++j){
                CSWAP(colors[j],colors[j+1]);
            }
        }
        gl_FragColor = colors[12];
    }
    // for 7x7 Doesn't work on my machine
    else{
        vec4 colors[49];
        for(float i = -u_pad; i <= u_pad; i+= u_step){
            for(float j = -u_pad; j <= u_pad; j+= u_step){
                colors[k] = texture2D(u_Sampler,vec2(v_TexCoord.x+j,v_TexCoord.y+i));
                k++;
            }
        }
        for(int i = 0; i < 49; ++i){
            for(int j = 0; j < 47; ++j){
                CSWAP(colors[j],colors[j+1]);
            }
        }
        gl_FragColor = colors[24];
    }
}
