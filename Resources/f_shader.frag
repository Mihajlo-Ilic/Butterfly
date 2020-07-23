#version 330

uniform vec3 color1;
uniform vec3 color2;
uniform float opacity;
uniform int type;
uniform sampler2D image;

uniform vec2 rect_size;
uniform int border_width;

in vec2 uv_coord;

void main(){
   if(type==0){
      float ux=uv_coord.x*rect_size.x;
      float uy=uv_coord.y*rect_size.y;
      if(ux<rect_size.x-border_width && ux>border_width && uy<rect_size.y-border_width && uy>border_width){
      float factor=distance(uv_coord,vec2(0.5));
      gl_FragColor=vec4( (1-factor)*color1 + factor*color2 ,opacity);
      }
      else {
         gl_FragColor=vec4(vec3(0.0f),1);
      }
   }
   else
   if(type==1)
   {
      float dist=1.0f-texture(image,uv_coord).a;
      float alfa=1.0f-smoothstep(0.48f,0.48f+0.23f,dist);
      gl_FragColor=vec4(color1,alfa);
   }
   else{
      gl_FragColor=texture(image,uv_coord);
   }
}