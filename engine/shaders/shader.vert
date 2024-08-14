#version 450                                                                             
                                                                                          
layout(location = 0) in vec3 position;

layout(location = 1) in vec3 color; 

layout(location = 2) in vec3 normal;

layout(location = 3) in vec2 uv;

layout(location = 4) in float t;




layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec3 fragUvw;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  mat4 invViewMatrix;
  vec3 cameraPosition;
  vec4 lightColor;
  float t;
} ubo;

layout(push_constant) uniform Push {
 
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;
 

vec3 positions[36] = vec3[](

    // right face

    vec3( 1.0, -1.0, -1.0),
    vec3( 1.0, -1.0,  1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0,  1.0, -1.0),
    vec3( 1.0, -1.0, -1.0),

    // left face
    
    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(-1.0, -1.0,  1.0),


    // top face

    vec3(-1.0,  1.0, -1.0),
    vec3( 1.0,  1.0, -1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3(-1.0,  1.0, -1.0),

    // bottom face

    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3( 1.0, -1.0, -1.0),
    vec3( 1.0, -1.0, -1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3( 1.0, -1.0,  1.0),


    // front face

    vec3(-1.0,  1.0, -1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3( 1.0, -1.0, -1.0),
    vec3( 1.0, -1.0, -1.0),
    vec3( 1.0,  1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),
  
    // back face
    
    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0, -1.0,  1.0),
    vec3(-1.0, -1.0,  1.0)
);                                             
                                                               
void main()                                                                                
{                         
  vec3 position =1000*positions[gl_VertexIndex];
  // position.y -=980;                                         
  vec4 positionWorld =vec4(position, 1.0);
  mat4 view = mat4(mat3(ubo.view));
  gl_Position = ubo.projection * view *  positionWorld;
  fragPosWorld = (view*positionWorld).xyz;
  fragColor = vec3(0.222,0.343,0.572);
  fragUvw = normalize(position);

}