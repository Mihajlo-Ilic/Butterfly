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
#include <sstream>
#include <iomanip>

namespace bfly
{

//CONSTRAINTS
#define FIXED 0
#define EXPANDING 1
#define MINIMUM 2
#define MAXIMUM 3

//COMPONENT STATES
#define IDLE 0
#define HOVER 1
#define PRESS 2
#define CLICKED 3

//LABEL TEXT ALLIGNMENT
#define LEFT 0
#define CENTER 1
#define RIGHT 2

#define TOP 0
#define BOTTOM 2

//ORDER
#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1

//SLIDER ORIENTATIONS
#define HORIZONTAL_SLIDER 0
#define VERTICAL_SLIDER 1

  using namespace std;

  struct Rectangle
  {
    int x;
    int y;
    int width;
    int height;
  };

  struct Vector3
  {
    float r;
    float g;
    float b;
  };

  struct Glyph
  {
    int uv_x;
    int uv_y;
    int width;
    int height;
    int x_off;
    int y_off;
    int x_adv;
  };

  struct Font
  {
    GLuint font_atlas;
    int size;
    int base;
    int line_height;
    unordered_map<char, Glyph> characters;
    string name;
    int scale_h;
    int scale_w;
  };

  struct Style
  {
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

  class Drawable
  {
  public:
    virtual void draw() = 0;
    virtual void process_events() = 0;
    virtual void on_focus() = 0;
    virtual void on_focus_lost() = 0;

    void set_position(int x, int y)
    {
      rect.x = x;
      rect.y = y;
    }

    void set_size(int width, int height)
    {
      rect.width = width;
      rect.height = height;
    }

    int get_width(){
      return rect.width;
    }

    int get_x(){
      return rect.x;
    }

    int get_y(){
      return rect.y;
    }

    void set_x(int x) { rect.x = x; }
    void set_y(int y) { rect.y = y; }
    void set_width(int width) { rect.width = width; }
    void set_height(int height) { rect.height = height; }
    void set_z(float _z) {z=_z;}

  protected:
    float z;
    Rectangle rect;
    int state;
    int constraint_width;
    int constraint_height;
    int min_width;
    int min_height;
    int max_height;
    int max_width;
  };

  //BASE CLASS FOR MOST GUI ELEMENTS

  class Component : public Drawable
  {
  public:
    Component(string _name) : name(_name)
    {
      visible = true;
      order = LEFT_TO_RIGHT;
      tooltip = "";
      animation_timer = 0;
      z=0;
    }

    bool operator==(const Component &c)
    {
      return name == c.get_name();
    }

    bool visible;

    void set_font(Font fnt) { style.font = fnt; }
    void set_color1(float r, float g, float b) { style.color1 = {r, g, b}; }
    void set_color2(float r, float g, float b) { style.color2 = {r, g, b}; }
    void set_border_color(float r, float g, float b) { style.border_color = {r, g, b}; }
    void set_opacity(float a) { style.opacity = a; }
    void set_text_size(int size);
    void set_text_color(float r, float g, float b) { style.text_color = {r, g, b}; }
    void set_border_width(int w) { style.border_width = w; }
    void set_name(string n) { name = n; }
    void set_tooltip(string t) { tooltip = t; }
    void set_order(int i) { order = i; }
    void set_text(string t);
    void set_texture_id(GLuint id) { image_id = id; }

    string get_name() const { return name; }
    string get_text() const { return text; }
    GLuint get_image() const { return image_id; }

    float get_text_width(){return text_width;}

  protected:
    string name;
    string tooltip;
    int order;
    Style style;
    GLuint vao;
    GLuint image_id;
    string text;
    float text_width;
    float text_height;
    unsigned animation_timer;
  };

  //COMPONENTS

  class Button : public Component
  {
  public:
    Button();
    void set_click_event(void (*click_event)(void))
    {
      click = click_event;
    }
    void draw();
    void process_events();
    void set_options(unsigned bitmask) {}
    void has_render_rect(bool b) { background_rectangle = b; }
    void has_icon(bool b) { icon = b; }
    void on_focus();
    void on_focus_lost();

  private:
    void rect_uniforms();
    Vector3 press_color;
    Vector3 click_color;
    void (*click)(void);
    bool background_rectangle;
    bool icon;
  };

  class ToggleButton : public Component
  {
  public:
    ToggleButton();
    void set_toggle_event(void (*toggle_event)(void))
    {
      toggle = toggle_event;
    }
    void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void has_render_rect(bool b) { background_rectangle = b; }
    void has_icon(bool b) { icon = b; }
    void on_focus();
    void on_focus_lost();

  private:
    void rect_uniforms();
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;

    bool background_rectangle;
    bool icon;
  };

  class CheckBox : public Component
  {
  public:
    CheckBox();
    void set_toggle_event(void (*toggle_event)(void))
    {
      toggle = toggle_event;
    }
    void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void has_icon(bool b) { icon = b; }
    void on_focus();
    void on_focus_lost();

  private:
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;

    bool icon;
  };

  class RadioGroup;

  class RadioButton : public Component
  {
  public:
    RadioButton();
    void set_toggle_event(void (*toggle_event)(void))
    {
      toggle = toggle_event;
    }
    void draw();
    void process_events();

    void set_options(unsigned bitmask) {}
    void set_value(bool b) { value = b; }
    RadioGroup *rg;
    void on_focus();
    void on_focus_lost();

  private:
    Vector3 press_color;
    Vector3 click_color;

    void (*toggle)(void);
    bool value;
    bool icon;
  };

  class ComboBox;
  class TreeView;

  class ComboItem : public Component
  {
  public:
    ComboItem();

    void draw();
    void process_events();

    ComboBox *parent_box = nullptr;
    ComboItem *parent_item=nullptr;

    void add_item(ComboItem *item)
    {
      sub_items.push_back(item);
      item->parent_box = parent_box;
      item->set_size(sub_item_width, sub_item_height);
      item->set_text_size(sub_item_height - 2);
      item->parent_item=this;
    }
    void on_focus();
    void on_focus_lost();

    void close(){
      opened=false;
      for (auto it:sub_items)
        it->close();
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

  class ComboBox : public Component
  {
  public:
    ComboBox();

    void draw();
    void process_events();

    void add_item(ComboItem *item)
    {
      items.push_back(item);
      item->parent_box = this;
      item->set_size(item_width, item_height);
      item->set_text_size(item_height - 2);
    }

    void set_default_text(string t);
    void change_selection(ComboItem *it);

    void on_focus();
    void on_focus_lost();


    void set_open(bool b){
      opened=b;
    }

    void change_selected(ComboItem *item);
    void set_change_event(void (*change_event)(void))
    {
      change = change_event;
    }

  private:
    ComboItem*  selected;
    void rect_uniforms();

    Vector3 press_color;
    Vector3 click_color;
    bool opened;

    int item_width;
    int item_height;

    GLuint default_vao;
    void (*change)(void);

    vector<ComboItem *> items;
  };

  class Label : public Component {
  public:
    Label();

    void draw();
    void process_events();

    void on_focus();
    void on_focus_lost();

    void use_image_as_icon(bool b){
      icon=b;
    }

    void set_allignment_horizontal(int a){
      allign_h=a;
    }

    void set_allignment_vertical(int a){
      allign_v=a;
    }

    private:

    bool icon;
    int allign_h;
    int allign_v;
  };

  class Slider : public Component{
    public:

      Slider();

      void draw();
      void process_events();

      void on_focus();
      void on_focus_lost();
      
      void set_orientation(int a){
        orientation=a;
      }

      void set_value(float v){
        if(v>=minimum && v<=maximum)
          value=v;
      }
      
    private:

    void update_value(int x);
    float minimum;
    float maximum;
    float value;
    float step;

    int orientation;

    int slider_size;
    
    Vector3 press_color;
    Vector3 click_color;
    void rect_uniforms();
  };


  class ScrollBar : public Component{
    public:
      ScrollBar();

      void draw();
      void process_events();

      void on_focus();
      void on_focus_lost();

      void set_width(int width){
        rect.width=width;
        if(orientation==HORIZONTAL_SLIDER){
          set_maximum(width > maximum ? width : maximum);
        }
      }

      void set_height(int height){
        rect.height=height;
        if(orientation==VERTICAL_SLIDER){
          set_maximum(height > maximum ? height : maximum);
        }
      }

      void set_size(int width, int height){
      rect.width = width;
      rect.height = height;
      if(orientation==HORIZONTAL_SLIDER){
          set_maximum(width > maximum ? width : maximum);
        }
      if(orientation==VERTICAL_SLIDER){
          set_maximum(height > maximum ? height : maximum);
        }
      }

      void set_maximum(float m){
        maximum=m;
        if(orientation==HORIZONTAL_SLIDER){
          float scale=(float)rect.width/maximum;
          slider_size=scale*rect.width;
        }
        if(orientation==VERTICAL_SLIDER){
          float scale=(float)rect.height/maximum;
          slider_size=scale*rect.height;
        }
        cout<<slider_size<<endl;
      }

      void set_orientation(int o){
        orientation=o;
      }

      float get_value(){
        int real_width=rect.width-slider_size;
        int real_height=rect.height-slider_size;
        if(orientation==HORIZONTAL_SLIDER){
          cout<<slider_position<<endl;
          return slider_position;
        }
        else {
          if (real_height!=0)
          return ((float)slider_position/(float)real_height)*maximum;
          else return 0;
        }
      }

      float get_maximum(){
        return maximum;
      }

      private:
        int slider_position;
        bool hidden;
        float maximum;
        int orientation;
        int slider_size;
        void rect_uniforms();
        Vector3 press_color;
        Vector3 click_color;
        int relative_pos=0;
  };

  //CONTAINERS

  class RadioGroup
  {
  public:
    RadioGroup()
    {
      multiple = false;
      allow_none = false;
      selected=nullptr;
    }

    void add_to_group(RadioButton *rb)
    {
      buttons.push_back(rb);
      if (rb->rg != nullptr)
        rb->rg->remove(rb);
      rb->rg = this;
    }

    void remove(RadioButton *rb)
    {
    }

    bool allow_none = false;
    bool multiple = false;
    int num_selected = 0;
    RadioButton *selected;

  private:
    vector<RadioButton *> buttons;
  };


  class Panel : public Component
  {
  public:
    Panel();
    void draw();
    void on_focus() {}
    void on_focus_lost() {}
    void process_events() {}
    void add_element(Component *d)
    {
      elements.push_back(d);
      if(d->get_x()+d->get_width()>rect.x+rect.width){
        cout<<"ee"<<endl;
        horizontal->set_maximum( (d->get_x()+d->get_width()-rect.x));
        cout<<horizontal->get_maximum()<<endl;
      }
    }

    void set_position(int x, int y)
    {
      rect.x = x;
      rect.y = y;
      horizontal->set_position(rect.x,rect.height+rect.y-20);
      horizontal->set_width(rect.width);

      vertical->set_position(rect.x+rect.width-20,rect.y+26);
      vertical->set_height(rect.height-20-26);

    }

    void set_size(int width, int height)
    {
      rect.width = width;
      rect.height = height;
      horizontal->set_position(rect.x,rect.height+rect.y-20);
      horizontal->set_width(rect.width);

      vertical->set_position(rect.x+rect.width-20,rect.y+26);
      vertical->set_height(rect.height-20-26);

      if(horizontal->get_maximum()<rect.width)
        horizontal->set_maximum(rect.width);


      if(vertical->get_maximum()<rect.height-20-26)
        vertical->set_maximum(rect.height-20-26);
    }

  private:
    void prepare_style();
    void prepare_style_text();
    void prepare_style_header();
    vector<Drawable *> elements;
    ScrollBar *horizontal;
    ScrollBar *vertical;
  };

  class Window
  {
  public:
    Window() {}
    void draw(GLFWwindow *window);
    void add_panel(Panel *p) { panels.push_back(p); }

  private:
    vector<Panel *> panels;
  };
} // namespace bfly

void Butterfly_Input(GLFWwindow *window);

void Butterfly_Refresh();

void Butterfly_Initialize();

#endif
