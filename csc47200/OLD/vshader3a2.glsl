attribute   vec2        a_Position;    // attribute variable: position vector
attribute   vec2        a_TexCoord;    // attribute variable: texture coordinate
uniform     mat4        u_ModelMatrix;
uniform     float       u_Theta;       // uniform variable for theta
uniform     int         u_Twist;

void main(){
    if( u_Twist == 0){
        gl_Position = gl_ModelViewProjectionMatrix * u_ModelMatrix * vec4(a_Position,0,1);
    }
    else{
        float distance = sqrt((a_Position.x * a_Position.x) + (a_Position.y * a_Position.y));
        float rotation = u_Theta * distance;
        mat4  rotationMatrix = mat4( cos(rotation), sin(rotation),  0,  0,
                                    -sin(rotation), cos(rotation),  0,  0,
                                                 0,             0,  1,  0,
                                                 0,             0,  0,  1);
        gl_Position = gl_ModelViewProjectionMatrix * rotationMatrix * vec4(a_Position,0,1);
    }
}
