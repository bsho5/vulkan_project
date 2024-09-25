#version 450                                                                             
                                                                                         

layout (location = 0) in vec3 fragUvw;


                                                                        
layout ( location = 0 ) out vec4 color; 

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
layout(set = 0, binding = 1) uniform samplerCube image;
layout(set = 1, binding = 1) uniform sampler2D image2;
layout(push_constant) uniform Push {

  mat4 modelMatrix;
  mat4 normalMatrix;

} push;
                                                                              
void main()                                                                               
{                  
   

 vec3 flippedTexCoord = vec3(-fragUvw.x, -fragUvw.y, fragUvw.z);
  vec3 imageColor = texture(image, flippedTexCoord).rgb;
  imageColor = pow(imageColor, vec3(2.2)); // Convert sRGB to linear space
  color = vec4(imageColor, 1.0f);
}