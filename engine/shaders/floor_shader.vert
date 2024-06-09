#version 450                                                                             
                                                                                          
layout(location = 0) in vec3 position;

layout(location = 1) in vec3 color; 

layout(location = 2) in vec3 normal;

layout(location = 3) in vec2 uv;

layout(location = 4) in float t;



layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  vec4 lightColor;
  float t;
} ubo;

layout(push_constant) uniform Push {
 
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;
 

                                                
                                                               
void main()                                                                                
{        
    vec3 newPosition = position ;                         
  //  float sinWave =  sin(newPosition.x); 
     
  // newPosition.x +=ubo.t;                                
  vec4 positionWorld = push.modelMatrix * vec4(newPosition,1.f);

  float speed_1 = 1.f;
  float waveLength_1 = 10.f;
  float frequency_1 = 2/waveLength_1;
  float alpha_1 = speed_1* frequency_1  ;
  float firstWave =  sin(((positionWorld.x*frequency_1 )+( ubo.t*alpha_1))); 
  float speed_2 = 2.f;
  float waveLength_2 = 5.f;
  float frequency_2 = 2/waveLength_2;
  float alpha_2 = speed_2* frequency_2  ;
  float secondWave =  sin(((positionWorld.x*frequency_2 )+( ubo.t*alpha_2))); 
  float speed_3 = 0.5f;
  float waveLength_3 = 20.f;
  float frequency_3 = 2/waveLength_3;
  float alpha_3 = speed_3* frequency_3  ;
  float thirdWave =  sin(((positionWorld.x*frequency_3 )+( ubo.t*alpha_3))); 
  float sinWave = firstWave + secondWave + thirdWave;
  
  positionWorld.y = sinWave;

  
  vec4 finalPosition =  ubo.projectionViewMatrix * positionWorld ;
  
  //       float sinWave =  sin(finalPosition.x); 

  //  finalPosition.y = finalPosition.y * sinWave; 
  gl_Position =vec4(finalPosition) ;
 
  vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

  vec3 directionToLight = ubo.lightPosition - positionWorld.xyz;
  float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared

  vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
  vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

  fragColor =(diffuseLight + ambientLight) * color;

}