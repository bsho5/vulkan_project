#version 450                                                                             
                                                                                         
                                                                        
layout ( location = 0 ) out vec4 color; 

layout(push_constant) uniform Push {
  vec2 offset;
  vec3 color;
} push;

                                                                              
void main()                                                                               
{                                                                                         
      color =vec4(push.color, 1.0);

}