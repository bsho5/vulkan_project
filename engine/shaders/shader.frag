#version 450                                                                             
                                                                                         
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec3 fragUvw;


                                                                        
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

vec3 rotationMatrixToEulerAngles(mat3 R) {
    float sy = sqrt(R[0][0] * R[0][0] + R[1][0] * R[1][0]);

    bool singular = sy < 1e-6;

    float x, y, z;
    if (!singular) {
        x = atan(R[2][1], R[2][2]);
        y = atan(-R[2][0], sy);
        z = atan(R[1][0], R[0][0]);
    } else {
        x = atan(-R[1][2], R[1][1]);
        y = atan(-R[2][0], sy);
        z = 0.0;
    }

    return vec3(x, y, z); // Returns pitch (x), yaw (y), roll (z)
}
                                                                              
void main()                                                                               
{                  
   
  // mat3 view = mat3(ubo.view);
  // vec3 rotation =  rotationMatrixToEulerAngles(view);
  // vec3 direction = normalize(fragPosWorld -rotation );
 vec3 flippedTexCoord = vec3(-fragUvw.x, -fragUvw.y, fragUvw.z);
  vec3 imageColor = texture(image, flippedTexCoord).rgb;
  imageColor = pow(imageColor, vec3(2.2)); // Convert sRGB to linear space
  color = vec4(imageColor, 1.0f);
}