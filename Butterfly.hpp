#ifndef BUTTERFLY_HPP
#define BUTTERFLY_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <algorithm>



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

//ORDER
#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1


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
       order=LEFT_TO_RIGHT;
       tooltip="";
       animation_timer=0;
   }

  bool operator==(const Component& c){
     return name==c.get_name();
   }

   bool visible;
   
   void set_position(int _x,int _y)      { rect.x=_x;rect.y=_y;  }
   void set_size(int _width,int _height) { rect.width=_width; rect.height=_height; }
   void set_font(Font fnt)               { style.font=fnt;   }
   void set_color1(float r,float g,float b)    { style.color1={r,g,b}; }
   void set_color2(float r,float g,float b)    { style.color2={r,g,b}; }
   void set_border_color(float r,float g,float b)    { style.border_color={r,g,b}; }
   void set_opacity(float a)             { style.opacity=a; }
   void set_text_size(int size);
   void set_text_color(float r,float g,float b)    { style.text_color={r,g,b}; }
   void set_border_width(int w)          { style.border_width=w; }
   void set_name(string n)               { name=n;  }
   void set_tooltip(string t)            { tooltip=t; }
   void set_order(int i)                 { order=i; }
   void set_text(string t);
   void set_texture_id(GLuint id)        { image_id=id; }

   string get_name() const {return name;}
   string get_text() const {return text;}

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
    void has_render_rect(bool b){background_rectangle=b;}
    void has_icon(bool b){icon=b;}
    void set_text_size(int size);
    void set_text(string t);
  private:
    void rect_uniforms();
    Vector3 press_color;
    Vector3 click_color;
    void (*click)(void);
    bool background_rectangle;
    bool icon;
 };

class ToggleButton:public Component{
    public:
     ToggleButton();
     void set_toggle_event(void (*toggle_event)(void)){
       toggle=toggle_event;
     }
     void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void has_render_rect(bool b){background_rectangle=b;}
    void has_icon(bool b){icon=b;}
    void set_text_size(int size);
    void set_text(string t);

  private:
    void rect_uniforms();
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;

    bool background_rectangle;
    bool icon;
};

class CheckBox : public Component{
  public:
    CheckBox();
    void set_toggle_event(void (*toggle_event)(void)){
       toggle=toggle_event;
     }
    void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void has_icon(bool b){icon=b;}
    void set_text_size(int size);
    void set_text(string t);

  private:
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;

    bool icon;
};

class RadioGroup;

class RadioButton : public Component{
  public:
    RadioButton();
    void set_toggle_event(void (*toggle_event)(void)){
       toggle=toggle_event;
     }
    void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void set_text_size(int size);
    void set_text(string t);
    void set_value(bool b){value=b;}
    RadioGroup* rg=nullptr;

  private:
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;
    bool icon;

};

class ComboBox;

class ComboItem : public Component {
  public:
    ComboItem();

    void draw();
    void process_events();

    void set_text_size(int size);
    void set_text(string t);

    ComboBox *parent=nullptr;

    void add_item(ComboItem *item){
      sub_items.push_back(item);
      item->parent=parent;
      item->set_size(sub_item_width,sub_item_height);
      item->set_text_size(sub_item_height-2);
    }

  private:

    void rect_uniforms();
    Vector3 press_color;
    Vector3 click_color;
    bool opened;

    int sub_item_width;
    int sub_item_height;

    vector<ComboItem *> sub_items; 
};

class ComboBox : public Component{
  public:
    ComboBox();

    void draw();
    void process_events();

    void set_text_size(int size);
    void set_text(string t);

    ComboBox *parent=nullptr;

    void add_item(ComboItem *item){
      items.push_back(item);
      item->parent=this;
      item->set_size(item_width,item_height);
      item->set_text_size(item_height-2);
    }

    void set_default_text(string t);
    void change_selection(ComboItem *it);

    ComboItem *selected;
    bool focus=false;
  private:
    void rect_uniforms();

    Vector3 press_color;
    Vector3 click_color;
    bool opened;

    int item_width;
    int item_height;

    GLuint default_vao;

    vector<ComboItem *> items;
};


//CONTAINERS

class RadioGroup {
  public:

    RadioGroup(){
      multiple=false;
      allow_none=false;
    }

    void add_to_group(RadioButton *rb){
      buttons.push_back(rb);
      if(rb->rg!=nullptr)
        rb->rg->remove(rb);
      rb->rg=this;
    }

    void remove(RadioButton *rb){
        
    }

    bool allow_none=false;
    bool multiple=false;
    int num_selected=0;
    RadioButton *selected;

  private:
    vector<RadioButton *> buttons;
};

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