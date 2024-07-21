#version 130
#ifdef GL_ES
precision mediump float;	// set default precision to medium
#endif

varying   vec3  v_Color;
varying   vec3  v_PositionWorldSpace;
varying   vec3  v_EyeDirectionCameraSpace;
varying   vec3  v_LightDirectionCameraSpace;
flat in   vec3  v_NormalCameraSpace;

void main() {
    vec3 LightColor  = vec3(1.0f,1.0f,1.0f);
    float LightPower = 0.5f;

    vec3 MaterialDiffuseColor = v_Color;
    vec3 MaterialAmbientColor = vec3(0.2f,0.2f,0.2f) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor= vec3(0.3f,0.3f,0.3f);

    vec3 n = normalize(v_NormalCameraSpace);
    vec3 l = normalize(v_LightDirectionCameraSpace);
    float cosTheta = clamp(dot(n,l), 0.0f, 1.0f);

    vec3 E = normalize(v_EyeDirectionCameraSpace);
    vec3 R = reflect(-l, n);

    float cosAlpha = clamp(dot(E,R), 0.0f, 1.0f);

    gl_FragColor = vec4(MaterialAmbientColor +
                        LightPower * cosTheta * MaterialDiffuseColor * LightColor +
                        LightPower * pow(cosAlpha, 5.0f) * MaterialSpecularColor * LightColor, 1.0f);
}
