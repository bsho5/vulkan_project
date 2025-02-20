
#version 450                                                                             
                                                                                         
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

                                                                        
layout ( location = 0 ) out vec4 color; 

layout(set = 0, binding = 1) uniform samplerCube skyboxTexture;

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

void main()                                                                               
{

vec3 lightDir = ((ubo.lightPosition) - (fragPosWorld)); 										   // light Direction

// diffuse lighting
float attenuation = 1.0 / dot(lightDir, lightDir); 												   // distance squared
normalize(lightDir);
vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation ;
vec3 diffuseLight =( ubo.lightColor.xyz  * max(dot((fragNormalWorld), normalize(lightDir)), 0));

// ambient lighting
vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

// specular lighting
vec3 cameraPosWorld = ubo.invViewMatrix[3].xyz;
vec3 viewDir    =  normalize( cameraPosWorld - fragPosWorld ); 
vec3 halfwayDir = normalize(normalize(lightDir) + viewDir);
float specularStrength = 2.0;
float shininess = 32.0;
float spec =  pow(max(dot(halfwayDir,(fragNormalWorld)), 0.0), shininess);
vec3 specular = specularStrength * spec * ubo.lightColor.xyz ;  

// refletion
vec3 I = normalize(-viewDir);
vec3 reflectDir = normalize( reflect(I ,  fragNormalWorld));
reflectDir.y = -reflectDir.y;
reflectDir.x = -reflectDir.x;
vec3 reflectedColor = texture(skyboxTexture, reflectDir).rgb;

// fresnel
float fresnel = pow(1 - dot(fragNormalWorld, I),5);

color = vec4(fragColor * ( diffuseLight +ambientLight+ specular  ), 1.0);  
}