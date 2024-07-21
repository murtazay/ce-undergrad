attribute   vec2 a_Position;  // attribute variable: position vector
varying     vec4 v_Color;     // varying variable for passing color to fragment shader

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * vec4(a_Position, 0, 1) ;
    v_Color = vec4(1.0, 1.0, 0.0, 1);
}
