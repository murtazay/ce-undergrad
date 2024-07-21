attribute   vec2    a_Position;  // attribute variable: position vector
attribute   vec3    a_Color;     // attribute variable: color vector
varying     vec4    v_Color;     // varying variable for passing color to fragment shader
uniform     float   u_Theta;     // uniform variable for theta in radians
uniform     int     u_Twist;     // uniform variable for twist flag

void main(){
    float distance = 1.0;
    if(u_Twist == 1){
        distance = sqrt(a_Position.x * a_Position.x + a_Position.y * a_Position.y);
    }
    float rot = u_Theta * distance;
    mat4 rotmat = mat4(cos(rot),    sin(rot),   0,  0,
                      -sin(rot),    cos(rot),   0,  0,
                              0,           0,   1,  0,
                              0,           0,   0,  1);
    gl_Position = gl_ModelViewProjectionMatrix * rotmat * vec4(a_Position, 0, 1) ;
    v_Color = vec4(a_Color, 1);
}
