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
    vec3 newPosition = position ;                         
  //  float sinWave =  sin(newPosition.x); 
     
  // newPosition.x +=ubo.t;                                
  vec4 positionWorld = push.modelMatrix * vec4(newPosition,1.f);

  float speed_1 = 0.5f;
  float waveLength_1 = 7.f;
  float frequency_1 = 2/waveLength_1;
  float alpha_1 = speed_1* frequency_1  ;
  float firstWave =  sin(((positionWorld.x*frequency_1 )+( ubo.t*alpha_1))); 
  float speed_2 = .5f;
  float waveLength_2 = 10.f;
  float frequency_2 = 2/waveLength_2;
  float alpha_2 = speed_2* frequency_2  ;
  float secondWave =  sin(((positionWorld.x*frequency_2 )+( ubo.t*alpha_2))); 
  float speed_3 = 0.5f;
  float waveLength_3 = 13.f;
  float frequency_3 = 2/waveLength_3;
  float alpha_3 = speed_3* frequency_3  ;
  float thirdWave =  sin(((positionWorld.x*frequency_3 )+( ubo.t*alpha_3))); 
  // float speed_8 = 0.5f;
  // float waveLength_8 = 10.f;
  // float frequency_8 = 2/waveLength_8;
  // float alpha_8 = speed_8* frequency_8  ;
  // float eighthWave =  sin(((positionWorld.x*frequency_8 )+( ubo.t*alpha_8)+positionWorld.z)); 

   float speed_4 = 0.5f;
  float waveLength_4 = 5.f;
  float frequency_4 = 2/waveLength_4;
  float alpha_4 = speed_4* frequency_4  ;
  float fourthtWave =  sin(((positionWorld.z*frequency_4 )+( ubo.t*alpha_4))); 
  float speed_5 = .25f;
  float waveLength_5 = 15.f;
  float frequency_5 = 2/waveLength_5;
  float alpha_5 = speed_5* frequency_5  ;
  float fifthWave =  sin(((positionWorld.z*frequency_5 )+( ubo.t*alpha_5))); 
  float speed_6 = .1f;
  float waveLength_6 = 10.f;
  float frequency_6 = 2/waveLength_6;
  float alpha_6 = speed_6* frequency_6  ;
  float sixthWave =  sin(((positionWorld.z*frequency_6 )+( ubo.t*alpha_6))); 
  float sinWave =  firstWave + secondWave + thirdWave +fourthtWave + fifthWave + sixthWave ;

  float xTan = atan(positionWorld.y/positionWorld.x);
  float zTan = atan(positionWorld.y/positionWorld.z);
  float normalXTan_1 =xTan*frequency_1* cos(((positionWorld.x*frequency_1 )+( ubo.t*alpha_1))); 
  float normalZTan_4 =zTan*frequency_4* cos(((positionWorld.z*frequency_4 )+( ubo.t*alpha_4))); 
  float normalXTan_2 =xTan*frequency_2* cos(((positionWorld.x*frequency_2 )+( ubo.t*alpha_2))); 
  float normalZTan_5 =zTan*frequency_5* cos(((positionWorld.z*frequency_5 )+( ubo.t*alpha_5))); 
  float normalXTan_3 =xTan*frequency_3* cos(((positionWorld.x*frequency_3 )+( ubo.t*alpha_3))); 
  float normalZTan_6 =zTan*frequency_6* cos(((positionWorld.z*frequency_6 )+( ubo.t*alpha_6))); 
  float normalXTan = normalXTan_1 +normalXTan_2+normalXTan_3;
  float normalZTan = normalZTan_4+ normalZTan_5 +normalZTan_6;
  vec3 xNormal = vec3(1.0 , -1.0,normalXTan );
  vec3 zNormal = vec3(0.0 , -1.0,normalZTan );
  vec3 vNormal = cross(xNormal,zNormal);

//positionWorld.y = sin(sqrt((positionWorld.x*positionWorld.z) +(positionWorld.y*positionWorld.z) ));
  // positionWorld.y = (fourthtWave+fifthWave+sixthWave) +(firstWave+secondWave+thirdWave);
  // positionWorld.z =sin(sqrt((positionWorld.x*positionWorld.x) +(positionWorld.y*positionWorld.y) ));
positionWorld.y = sinWave ;  

  
  vec4 finalPosition =  ubo.projection * ubo.view * positionWorld ;
  
  //       float sinWave =  sin(finalPosition.x); 

  //  finalPosition.y = finalPosition.y * sinWave; 
  gl_Position =vec4(finalPosition) ;
 vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) *normalize(vNormal));

  float lightIntensity =AMBIENT +  max(dot(normalWorldSpace, DIRECTION_TO_LIGHT),0);

  fragColor = lightIntensity * color * vec3(0.222,0.343,0.472);

  // fragNormalWorld = normalize(mat3(push.normalMatrix) * normalize( vec3(0.0,-1.0,0.0)));
  // fragPosWorld = positionWorld.xyz;
  // fragColor = lightIntensity * vec3(0.222,0.343,0.472);
   fragNormalWorld = normalize(mat3(push.normalMatrix) * normalize(vNormal));
  fragPosWorld = positionWorld.xyz;
  // fragColor = vec3(0.222,0.343,0.472);

}