#version 450

// in                                                                                        
// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color; 
// layout(location = 2) in vec3 normal;
// layout(location = 3) in vec2 uv;
// layout(location = 4) in float t;

// out 

layout (location = 0) out vec2 fragOffset;


const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);



struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
};
layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  vec4 lightColor;
  float t;
} ubo;

layout(push_constant) uniform Push {
 
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;
 

const float radius = 30.0;               
                                                               
void main()                                                                                
{     

  fragOffset = OFFSETS[gl_VertexIndex];
  vec3 cameraRightWorld = {ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]};
  vec3 cameraUpWorld = {ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]};

  vec3 positionWorld =
    vec3(ubo.lightPosition.x,-800,ubo.lightPosition.z)
    + radius * fragOffset.x * cameraRightWorld
    + radius * fragOffset.y * cameraUpWorld;

  gl_Position = ubo.projection * ubo.view * vec4(positionWorld, 1.0);

 
}


















