#ifdef GL_ES
precision mediump float;            // set default precision to medium
#endif

attribute vec3	a_Position;         // attribute variable: position vector
attribute vec3  a_Normal;
attribute vec3  a_Color;

varying   vec3  v_Color;
varying   vec3  v_PositionWorldSpace;
varying   vec3  v_EyeDirectionCameraSpace;
varying   vec3  v_LightDirectionCameraSpace;
varying   vec3  v_NormalCameraSpace;

uniform   vec3  u_LightDirection;
uniform	  mat4	u_Model;            // uniform variable for passing model matrix
uniform   mat4  u_View;             // uniform variable for passing view matrix
uniform	  mat4	u_Projection;       // uniform variable for passing projection matrix

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position,1.0f);

    v_PositionWorldSpace = vec3(u_Model * vec4(a_Position,1.0f));

    v_EyeDirectionCameraSpace = vec3(0.0f,0.0f,0.0f) - vec3(u_View * u_Model * vec4(a_Position, 1.0f));

    v_LightDirectionCameraSpace = vec3(u_View * vec4(u_LightDirection,0.0f));

    v_NormalCameraSpace = vec3(u_View * u_Model * vec4(a_Normal, 0.0f));

    v_Color = a_Color;
}
