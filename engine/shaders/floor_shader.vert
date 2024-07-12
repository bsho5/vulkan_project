#version 450

// in                                                                                        
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color; 
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in float t;

// out 
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
//layout(location = 3) out vec3 normalOut;


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
 

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(200.0, -3.0, -5.0));
const float AMBIENT = 0.02;
                                      
                                                               
void main()                                                                                
{        
 
 
}


















    //positionWorld.y = sin(sqrt((positionWorld.x*positionWorld.z) +(positionWorld.y*positionWorld.z) ));
  // positionWorld.y = (fourthtWave+fifthWave+sixthWave) +(firstWave+secondWave+thirdWave);
  // positionWorld.z =sin(sqrt((positionWorld.x*positionWorld.x) +(positionWorld.y*positionWorld.y) ));
  //       float sinWave =  sin(finalPosition.x); 

  //  finalPosition.y = finalPosition.y * sinWave; 
  // DIRECTION_TO_LIGHT = finalPosition.xyz-vec3(170.0f, -5.5f, 320.f);
  // fragNormalWorld = normalize(mat3(push.normalMatrix) * normalize( vec3(0.0,-1.0,0.0)));
  // fragPosWorld = positionWorld.xyz;
  // fragColor = lightIntensity * vec3(0.222,0.343,0.472);
  // fragColor = vec3(0.222,0.343,0.472);