#version 450                                                                             
                                                                                         
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUv;


                                                                        
layout ( location = 0 ) out vec4 color; 

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  vec4 lightColor;
} ubo;
layout(set = 0, binding = 1) uniform sampler2D image;
layout(set = 0, binding = 2) uniform sampler2D image2;
layout(push_constant) uniform Push {

  mat4 modelMatrix;
  mat4 normalMatrix;

} push;

                                                                              
void main()                                                                               
{                  
  // vec3 directionToLight = ubo.lightPosition - fragPosWorld;
  // float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
  // vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
  // vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  // vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

  // vec3 lightDir   = normalize(ubo.lightPosition - fragPosWorld);
  // vec3 viewDir    = normalize(vec3(170.0,-10.0,200.0) - fragPosWorld);
  // vec3 halfwayDir = normalize(lightDir + viewDir);
  // vec3 reflectDir = reflect(-lightDir, normalize(directionToLight));
  // float spec = pow(max(dot(normalize(directionToLight), reflectDir), 0.0),8.0);
  
  // vec3 specular = lightColor * spec;

  // float lightIntensity = max(dot(fragNormalWorld,  normalize(vec3(70.0, -3.0, -10.0))),0);

  //color = vec4(( specular +(fragColor*lightIntensity)) , 1.0);    
  
 vec3 imageColor = texture(image2, fragUv).rgb;
  color = vec4(imageColor,1.0);
}