#version 450                                                                             
                                                                                          
layout(location = 0) in vec3 position;

layout(location = 1) in vec3 color; 

layout(location = 2) in vec3 normal;

layout(location = 3) in vec2 uv;

layout(location = 4) in float t;




layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUv;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  vec3 cameraPosition;
  vec4 lightColor;
} ubo;

layout(push_constant) uniform Push {
 
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;
 

                                                
                                                               
void main()                                                                                
{                         
  vec3 position1 =position;
  // position1.x= floor(gl_VertexIndex / 2.0);
  // position1.z = mod(gl_VertexIndex,2.0)        ;                                            
  vec4 positionWorld = push.modelMatrix *vec4(position1, 1.0);
  gl_Position = ubo.projection*ubo.view *  positionWorld;
  // fragNormalWorld = normalize(mat3(push.normalMatrix) * normalize(vec3(0.0,-1.0,0.0)));
  //fragPosWorld = positionWorld.xyz;
  fragColor = vec3(0.222,0.343,0.572);
  fragUv = uv;

}