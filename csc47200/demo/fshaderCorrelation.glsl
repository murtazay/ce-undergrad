#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform sampler2D u_Template;   // uniform for template image
uniform float     u_XPad;
uniform float     u_YPad;
uniform float     u_XStep;
uniform float     u_YStep;
uniform float     u_TemplateWidth;
uniform float     u_TemplateHeight;
uniform float     u_TemplateXStep;
uniform float     u_TemplateYStep;

void main() {
    vec4 color = vec4(0.f);
    vec4 TMean = vec4(0.f);
    vec4 s;
    vec4 t;
    float xbound = u_TemplateWidth/2.f;
    float ybound = u_TemplateHeight/2.f;
    float c      = 0.f;
    // Calculate average of template
    for(float i = -xbound; i <= xbound; i += u_TemplateXStep){
        for(float j = -ybound; j <= ybound; j += u_TemplateYStep){
            TMean += texture2D(u_Template, vec2(xbound + i, ybound + j));
            c++;
        }
    }
    TMean = TMean / c;
    // Run template through texture(sampler)
    float i = -xbound;
    float m = -u_XPad;
    while(i <= xbound){
        float j = -ybound;
        float n = -u_YPad;
        while(j <= ybound){
            s = texture2D(u_Sampler,vec2(v_TexCoord.x+m, v_TexCoord.y+n))  - TMean;
            t = texture2D(u_Template, vec2(xbound + i, ybound + j)) - TMean;
            color += normalize(s*t);
            j += u_TemplateYStep;
            n += u_YStep;
        }
        i += u_TemplateXStep;
        m += u_XStep;
    }
    gl_FragColor = vec4(color.rgb, 1.f);
}

