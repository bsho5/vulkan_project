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
layout(location = 3) out vec3 normalOut;
layout(location = 4) out vec4 ioEyeSpacePosition;


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
 

const vec3 DIRECTION_TO_LIGHT =normalize (vec3(200.0, -100.0,200.0));
const float AMBIENT = 0.05;
                                      
struct Wave {
    float amplitude;    // Amplitude (A)
    float wavelength;   // Wavelength (L)
    float speed;        // Speed (S)
    vec3 direction;     // Direction (D)
};

// Function to compute the sum of sine waves and their partial derivatives
void computeWaterHeightAndDerivatives(vec3 position1, float time, Wave waves[32], int numWaves, out float height, out vec3 dHeight) {
    height = 0.0; // vertex movment on the y axis
    float a = 1.19*2 ; // amplitude
    float w = 0.8474*0.15;// frequency
    dHeight = vec3(0.0); // vertex normal based on using partial derivatives
    vec3 dWaveHeight = vec3(1.0); // a vector to compute every wave's partial derivative
    float waveHeight = 0.0; // a vector to compute every wave's movment on the y axis


    for (int i = 0; i < numWaves; ++i) {
        a = a * 0.84; // Brownian Motion amplitude
        w = w * 1.18; // Brownian Motion frequency
     
        float frequency = w; 
        float phaseConstant = waves[i].speed * frequency;      // Phase constant = S * ω
                    
        // Compute the wave phase
        vec3 d = waves[i].direction; // wave direction 
        vec2 xzDirection = vec2(d.x,d.z); // wave direction in x and z planes    
        float xz = ( position1.x *d.x) +(position1.z *d.z); // the dot product of the wave direction in x and z planes and x,z points  
        float k = 2.5; // peaks sharpener controller
        float phase = (xz *w) + (time *phaseConstant);
        float steepness = abs(pow(cos(phase), 1.0)); // Adjust based on your needs (e.g., pow(cos(phase), 2.0) for stronger effect)
        float dSteepness = abs(pow(sin(phase), 1.0)); // steepness derivative
       
        // Compute the wave contribution to the height
        waveHeight = steepness * (2.0*a * pow(((sin( phase)+1)/2),k));
        height += waveHeight ;

        // Compute the partial derivatives

        vec2 dxz = xzDirection * dSteepness * (k*a * frequency*cos(phase)*pow(((sin( phase)+1)/2),k-1)) ;  
       
      

        dWaveHeight =  vec3 (-1.0*dxz.x,-1.0*dxz.y,1.0);
       // vec3 (dxz.x,dxz.y,1.0);
       // vec3 xNormal = vec3(1.0 , 0.0,dxz.x );
       // vec3 zNormal = vec3(0.0 , 1.0,dxz.y );
       //cross((xNormal),(zNormal));
       
       dHeight += dWaveHeight;

 
    
    }
}

// Function to compute the direction of a vertex
vec4 computeHeightAndNormal(vec3 position, float time, Wave waves[32], int numWaves) {
    float height;

    vec3 dHeight;

    computeWaterHeightAndDerivatives(position, time, waves, numWaves, height, dHeight);

    
    // The direction is the normalized gradient vector
    return (vec4(dHeight,height));
}

    const int NUM_WAVES = 32;
    Wave waves[NUM_WAVES];

    void addWaves(){
    waves[0] = Wave(1.5, 6.0, 0.5, vec3(0.3,0.2,0.4));
    waves[1] = Wave(2.0, 4.0, 0.5, vec3(0.5,1.0,1.0));
    waves[2] = Wave(2.5, 7.0, 0.5, vec3(0.6,0.3,0.5));
    waves[3] = Wave(1.0, 6.0, 1.0, vec3(0.7,0.5,0.7));
    waves[4] = Wave(1.6, 4.0, 0.5, vec3(0.4,0.9,0.2));
    waves[5] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.4));
    waves[6] = Wave(0.7, 4.0, 0.1, vec3(0.3,0.6,0.6));
    waves[7] = Wave(1.8, 7.0, 1.0, vec3(0.5,0.5,0.3));
    waves[8] = Wave(1.5, 6.0, 0.5, vec3(0.6,0.2,0.35));
    waves[9] = Wave(2.0, 4.0, 0.5, vec3(0.55,1.0,1.0));
    waves[10] = Wave(2.5, 7.0, 0.5, vec3(0.3,0.3,0.5));
    waves[11] = Wave(1.0, 6.0, 1.0, vec3(0.4,0.5,0.6));
    waves[12] = Wave(1.6, 4.0, 0.5, vec3(0.6,0.9,0.25));
    waves[13] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.45));
    waves[14] = Wave(0.7, 4.0, 0.1, vec3(0.2,0.6,0.3));
    waves[15] = Wave(1.8, 7.0, 1.0, vec3(0.4,0.5,0.5));
    waves[16] = Wave(1.5, 6.0, 0.5, vec3(0.3,0.2,0.4));
    waves[17] = Wave(2.0, 4.0, 0.5, vec3(0.5,1.0,1.0));
    waves[18] = Wave(2.5, 7.0, 0.5, vec3(0.6,0.3,0.5));
    waves[19] = Wave(1.0, 6.0, 1.0, vec3(0.7,0.5,0.7));
    waves[20] = Wave(1.6, 4.0, 0.5, vec3(0.4,0.9,0.2));
    waves[21] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.4));
    waves[22] = Wave(0.7, 4.0, 0.1, vec3(0.3,0.6,0.6));
    waves[23] = Wave(1.8, 7.0, 1.0, vec3(0.5,0.5,0.3));
    waves[24] = Wave(1.5, 6.0, 0.5, vec3(0.6,0.2,0.35));
    waves[25] = Wave(2.0, 4.0, 0.5, vec3(0.55,1.0,1.0));
    waves[26] = Wave(2.5, 7.0, 0.5, vec3(0.3,0.3,0.5));
    waves[27] = Wave(1.0, 6.0, 1.0, vec3(0.4,0.5,0.6));
    waves[28] = Wave(1.6, 4.0, 0.5, vec3(0.6,0.9,0.25));
    waves[29] = Wave(2.3, 5.0, 0.25, vec3(1.0,1.0,0.45));
    waves[30] = Wave(0.7, 4.0, 0.1, vec3(0.2,0.6,0.3));
    waves[31] = Wave(1.8, 7.0, 1.0, vec3(0.4,0.5,0.5));

    }

void main()                                                                                
{        

    addWaves();

    vec4 positionWorld =  push.modelMatrix * vec4(position,1.f);

    vec4 wave = computeHeightAndNormal(positionWorld.xyz,  ubo.t,  waves,  NUM_WAVES) ;
    float height = wave.w;
    vec3 height_normal = (wave.xyz);  
    positionWorld.y = height; 
 
    vec4 finalPosition = ubo.projection * ubo.view * positionWorld ;
    gl_Position =vec4(finalPosition) ;
    


    fragNormalWorld = normalize(mat3(push.normalMatrix) * normalize(height_normal));
    fragPosWorld = positionWorld.xyz;
    normalOut = normalize(mat3(push.normalMatrix) * normalize(vec3(height_normal.x,height_normal.y,8.0))); // different normal for diffuse color cuz it looks better
    fragColor =   
    // 10*vec3(0.55f, 0.79f, 0.97f); // sea color
    10*vec3(0.0004f, 0.016f, 0.047f); // sea color
	ioEyeSpacePosition =  push.modelMatrix *ubo.view * vec4(position, 1.0);
}