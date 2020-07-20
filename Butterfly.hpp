#ifndef BUTTERFLY_HPP
#define BUTTERFLY_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <sstream>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstring>


namespace bfly{

//CONSTRAINTS
#define FIXED 0
#define EXPANDING 1
#define MINIMUM 2
#define MAXIMUM 3

//COMPONENT STATES
#define BUTTON_IDLE 0
#define BUTTON_HOVER 1
#define BUTTON_PRESS 2
#define BUTTON_CLICKED 3


using namespace std;

 struct Rectangle{
     int x;
     int y;
     int width;
     int height;
 };

 struct Vector3{
     float r;
     float g;
     float b;
 };

 struct Glyph{
     int uv_x;
     int uv_y;
     int width;
     int height;
     int x_off;
     int y_off;
     int x_adv;
 };

 struct Font{
     GLuint font_atlas;
     int size;
     int base;
     int line_height;
     unordered_map<char,Glyph> characters;
     string name;
     int scale_h;
     int scale_w;
 };

 struct Style{
     Vector3 color1;
     Vector3 color2;
     Font font;
     Vector3 text_color;
     Vector3 border_color;
     int border_width;
     float opacity;
     int text_size;
 };

//INTERFACE FOR COMPONENTS TO IMPLEMENT DRAWING METHODS

 class Drawable{
  public:
   virtual void draw() =0;
   virtual void process_events() =0;
  protected:
   int state;
   int constraint_width;
   int constraint_height;
   int min_width;
   int min_height;
   int max_height;
   int max_width;
 };


 //BASE CLASS FOR MOST GUI ELEMENTS

 class Component : public Drawable {
  public:
   Component(string _name):name(_name){
       visible=true;
       order=0;
       tooltip="";
       animation_timer=0;
   }

   bool visible;
   
   void set_position(int _x,int _y)      { rect.x=_x;rect.y=_y;  }
   void set_size(int _width,int _height) { rect.width=_width; rect.height=_height; }
   void set_font(Font fnt)               { style.font=fnt;   }
   void set_color1(float r,float g,float b)    { style.color1={r,g,b}; }
   void set_color2(float r,float g,float b)    { style.color2={r,g,b}; }
   void set_border_color(float r,float g,float b)    { style.border_color={r,g,b}; }
   void set_opacity(float a)             { style.opacity=a; }
   void set_text_size(int size)          { style.text_size=size; }
   void set_text_color(float r,float g,float b)    { style.text_color={r,g,b}; }
   void set_border_width(int w)          { style.border_width=w; }
   void set_name(string n)               { name=n;  }
   void set_tooltip(string t)            { tooltip=t; }
   void set_order(int i)                 { order=i; }
   void set_text(string t)               { text=t; }
   void set_texture_id(GLuint id)        { image_id=id; }

  protected:

   string name;
   string tooltip;
   int order;
   Style style;
   Rectangle rect;
   GLuint vao;
   GLuint image_id;
   string text;
   float text_width;
   float text_height;
   unsigned animation_timer;
 };

//COMPONENTS

 class Button :public Component{
  public:
    Button();
    void set_click_event(void (*click_event)(void)){
         click=click_event;
     }
    void draw();
    void process_events();
    void set_options(unsigned bitmask) {}

  private:

    Vector3 press_color;
    Vector3 click_color;
    void (*click)(void);
    //2^0 background image or icon
    //2^1 without background rectangle bool
    unsigned options;
 };

class ToggleButton:public Component{
    public:
     ToggleButton();
     void set_toggle_event(void (*toggle_event)(void)){
       toggle=toggle_event;
     }
     void draw();
    void process_events();

  private:

    Vector3 press_color;
    Vector3 click_color;
    void (*toggle)(void);
};


//CONTAINERS

 class Panel{
  public:

   Panel();
   bool visible;

   void set_header(string h) { header=h; }
   void set_name(string n)   { name=n;   }
   void set_position(int x,int y) { rect.x=x; rect.y=y; }
   void set_size(int width,int height) { rect.width=width; rect.height=height; }
   void draw();
   void add_element(Drawable *d) { elements.push_back(d); }

  private:

   void prepare_style();
   void prepare_style_text();
   void prepare_style_header();
   string header;
   Rectangle rect;
   string name;
   string tooltip;
   Style style;
   vector<Drawable *> elements;
   GLuint text_vao;
   Vector3 header_color1;
   Vector3 header_color2;
   float header_height;
   float header_width;
 };

 class Window{
  public:
   Window(){}
   void draw(GLFWwindow *window);
   void add_panel(Panel *p) {panels.push_back(p);}
   private:
   vector<Panel *> panels;
 };
}

void Butterfly_Input(GLFWwindow *window);

void Butterfly_Refresh();

void Butterfly_Initialize();

#endif