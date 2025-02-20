#version 450

// in                                                                                        
// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color; 
// layout(location = 2) in vec3 normal;
// layout(location = 3) in vec2 uv;
// layout(location = 4) in float t;

// out 
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

layout(std140, binding = 2) buffer OutputBuffer {
    vec4 positionWorld[1572864];
    vec3 normalWorld[1572864];
} OBuffer;

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
 

// const vec3 DIRECTION_TO_LIGHT =normalize (vec3(200.0, -100.0,200.0));
// const float AMBIENT = 0.05;
                                      
// struct Wave {
//     float amplitude;    // Amplitude (A)
//     float wavelength;   // Wavelength (L)
//     float speed;        // Speed (S)
//     vec3 direction;     // Direction (D)
// };

//  const int NUM_WAVES = 17;

// // Function to compute the sum of sine waves and their partial derivatives
// void computeWaterHeightAndDerivatives(vec3 position1, float time, Wave waves[NUM_WAVES], int numWaves, out float height, out vec3 dHeight) {
//     height = 0.0;                                               // vertex movment on the y axis
//     float a = 1.0 * 0.75  ;                                     // amplitude
//     float w = 1.0 * 0.25 ;                                      // frequency
//     dHeight = vec3(0.0);                                        // vertex normal based on using partial derivatives
//     vec3 dWaveHeight = vec3(1.0);                               // a vector to compute every wave's partial derivative
//     float waveHeight = 0.0;                                     // a vector to compute every wave's movment on the y axis
//     vec2 dxz = vec2(0.0);


//     for (int i = 0; i < numWaves; ++i) {
          
//         float phaseConstant =  waves[i].speed * w;              // Phase constant = S * ω
                    
//         // Compute the wave phase
//         vec3 d = waves[i].direction;                            // wave direction 
//         vec2 xzDirection = d.xz;                                // wave direction in x and z planes    
//         float xz = (d.x * position1.x) + (d.z * position1.z);
//         float phase =  ((xz * w) + (time + phaseConstant)) ;
     
//         // Compute the wave contribution to the height
//         waveHeight = a * exp(sin(phase)-1);
//         height += waveHeight ;

//         // Compute the partial derivatives
//         dxz =  xzDirection * w * a * exp(sin(phase)-1) * cos(phase);
//         dWaveHeight = vec3 (-1.0 * dxz.x ,-1.0 , -1.0 * dxz.y);
//         dHeight += dWaveHeight;

//         a = a * 0.82;                                          // Brownian Motion amplitude
//         w = w * 1.18;                                          // Brownian Motion frequency
//     }
// }

// // Function to compute the direction of a vertex
// vec4 computeHeightAndNormal(vec3 position, float time, Wave waves[NUM_WAVES], int numWaves) {
//     float height = 0.0;
//     vec3 dHeight = vec3(0.0);
//     computeWaterHeightAndDerivatives(position, time, waves, numWaves, height, dHeight);
//     return (vec4(dHeight,height));
// }

   
    // Wave waves[NUM_WAVES];

    // void addWaves(){
    // waves[0] = Wave(1.5, 6.0, 0.5, vec3(1.0,0.2,1.0));
    // waves[1] = Wave(2.0, 4.0, 0.5, vec3(0.5,1.0,1.0));
    // waves[2] = Wave(2.5, 7.0, 0.5, vec3(0.6,0.3,0.5));
    // waves[3] = Wave(1.0, 6.0, 1.0, vec3(0.7,0.5,0.7));
    // waves[4] = Wave(1.6, 4.0, 0.5, vec3(0.4,0.9,0.2));
    // waves[5] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.4));
    // waves[6] = Wave(0.7, 4.0, 0.1, vec3(0.3,0.6,0.6));
    // waves[7] = Wave(1.8, 7.0, 1.0, vec3(0.5,0.5,0.3));
    // waves[8] = Wave(1.5, 6.0, 0.5, vec3(0.6,0.2,0.35));
    // waves[9] = Wave(2.0, 4.0, 0.5, vec3(0.55,1.0,1.0));
    // waves[10] = Wave(2.5, 7.0, 0.5, vec3(0.3,0.3,0.5));
    // waves[11] = Wave(1.0, 6.0, 1.0, vec3(0.4,0.5,0.6));
    // waves[12] = Wave(1.6, 4.0, 0.5, vec3(0.6,0.9,0.25));
    // waves[13] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.45));
    // waves[14] = Wave(0.7, 4.0, 0.1, vec3(0.2,0.6,0.3));
    // waves[15] = Wave(1.8, 7.0, 1.0, vec3(0.4,0.5,0.5));
    // waves[16] = Wave(1.5, 6.0, 0.5, vec3(0.3,0.2,0.4));
    // //waves[17] = Wave(2.0, 4.0, 0.5, vec3(0.5,1.0,1.0));
    // // waves[18] = Wave(2.5, 7.0, 0.5, vec3(0.6,0.3,0.5));
    // // waves[19] = Wave(1.0, 6.0, 1.0, vec3(0.7,0.5,0.7));
    // //waves[20] = Wave(1.6, 4.0, 0.5, vec3(0.4,0.9,0.2));
    // // waves[21] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.4));
    // // waves[22] = Wave(0.7, 4.0, 0.1, vec3(0.3,0.6,0.6));
    // // waves[23] = Wave(1.8, 7.0, 1.0, vec3(0.5,0.5,0.3));
    // // waves[24] = Wave(1.5, 6.0, 0.5, vec3(0.6,0.2,0.35));
    // // waves[25] = Wave(2.0, 4.0, 0.5, vec3(0.55,1.0,1.0));
    // // waves[26] = Wave(2.5, 7.0, 0.5, vec3(0.3,0.3,0.5));
    // // waves[27] = Wave(1.0, 6.0, 1.0, vec3(0.4,0.5,0.6));
    // // waves[28] = Wave(1.6, 4.0, 0.5, vec3(0.6,0.9,0.25));
    // // waves[29] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.45));
    // // waves[30] = Wave(0.7, 4.0, 0.1, vec3(0.2,0.6,0.3));
    // // waves[31] = Wave(1.8, 7.0, 1.0, vec3(0.4,0.5,0.5));
    // }

void main()                                                                                
{        

    // addWaves();

    // // calculating the new x and z positions of the vertices  
    // vec3 base_position = vec3(0.0,0.0,0.0);
    // uint vi =  gl_VertexIndex;
    // uint vertexNumber = 6291456;
    // uint quadWidth = uint(sqrt(vertexNumber/6));
    // uint quadVertices = 6;
    // uint axisVertices = vertexNumber/quadWidth;
    // uint x = vi % (axisVertices); 
    // vec3 increament = vec3(floor(float(x)/(quadVertices*1.0)),0.0,floor(float(vi)/(axisVertices)));
    // vi = vi % quadVertices ;
    // if(vi>2) vi = vi-2; 
    // vec3 p = base_position + increament + (vec3(floor(float(vi)/2.0),0.0, vi % 2 ) );
    // vec4 positionWorld = vec4(p,1.f);

    // // calculating the new y position of the vertices  
    // vec4 wave = vec4(0.0);
    // wave = computeHeightAndNormal(positionWorld.xyz,  ubo.t,  waves,  NUM_WAVES) ;
    // float height = wave.w;
    // vec3 height_normal = (wave.xyz);  
    // positionWorld.y = -(height) ; 

    vec4 position = OBuffer.positionWorld[gl_VertexIndex];
    vec3 normal = OBuffer.normalWorld[gl_VertexIndex];
    // final position
    vec4 finalPosition = ubo.projection * ubo.view * position ;
    gl_Position =vec4(finalPosition) ;
    // fragment shader variables
    fragNormalWorld = normal;
    fragPosWorld = ( position).xyz;
    fragColor = vec3(0.0004f, 0.016f, 0.047f);
}