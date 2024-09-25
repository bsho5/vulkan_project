#version 450                                                                             
                                                                                         

layout (location = 0) in vec2 fragOffset;
                                                                        
layout ( location = 0 ) out vec4 color; 

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  vec4 lightColor;
} ubo;


layout(push_constant) uniform Push {

  mat4 modelMatrix;
  mat4 normalMatrix;

} push;



void main() {

  float dis = sqrt(dot(fragOffset, fragOffset)) ;

  if (dis >= 1.0) {

    discard;

  } else if (dis < 1.0 && dis >= 0.7) {
    
    color = (1 - (dis)) * vec4(0.1);

  } else if (dis < 0.7 && dis >= 0.5) {
    
    color = (1 - (dis)) * vec4(0.15);
  } else {

    color = vec4(0.5);

  }

 
  
}
















  
    //   float m = (newLight.z)/(newLight.x - 20);
//   float b = -1*m*20 ;
//   float z = m*directionToLight.x + b;  
//    float m2 = (newLight.z-20)/(newLight.x);
//   float b2 = 20 ;
//   float z2 = m2*directionToLight.x + b2;  
//   if(z>directionToLight.z){
// directionToLight.z = 0.0;
//   }
   
//   if(z2<directionToLight.z){
// directionToLight.z = 0.0;
//   }
// vec3 newLight =  ubo.lightPosition;
// newLight.z = newLight.z +200;
// newLight.x = newLight.x +100;
  // vec3 directionToLight2 = directionToLight-vec3(50.0,-10,100) ;
  //   directionToLight2.z =fragPosWorld.z;
  //    directionToLight2.y =fragPosWorld.y;
  //   //vec3(fragPosWorld.x, ubo.lightPosition.y - fragPosWorld.y,300);
  // float attenuation2 = 1.0 / dot(directionToLight2, directionToLight2); // distance squared

  // vec3 lightColor2 = ubo.lightColor.xyz* ubo.lightColor.w ;
  // vec3 ambientLight2 = ubo.ambientLightColor.xyz*.5;
  // vec3 diffuseLight2 = lightColor2 * max(dot(normalize(fragNormalWorld), normalize(directionToLight2)), 0);
                  
  // color = vec4(fragColor,1.0); 