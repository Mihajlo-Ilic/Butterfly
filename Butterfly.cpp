#include "Butterfly.hpp"

using namespace bfly;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!GLOBAL STATES AND VARIABLES!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int component_counter=0;

GLuint vertices_id;
GLuint edges_id;
GLuint program_id;

GLuint orto_location;
GLuint model_location;
GLuint type_location;
GLuint color1_location;
GLuint color2_location;
GLuint opacity_location;
GLuint rect_size_location;
GLuint border_width_location;

bool mouse_hold=false;
bool mouse_click=false;

int mouse_state=GLFW_RELEASE;

int key_pressed=-1;

Style panel_style;
Font regular_font;

int mouse_x=0;
int mouse_y=0;

//!!!!!!!!!!!!!!!!!!!!
//!!HELPER FUNCTIONS!!
//!!!!!!!!!!!!!!!!!!!!

void load_projection_matrix(float left,float right,float up,float down){
    float znear=-1.0f;
    float zfar=1.0f;
    float mat[4][4]={
        { 2.0f/(right-left) , 0 , 0 , -(right+left)/(right-left)},
        { 0,  2.0f/(up-down) , 0 , -(up+down)/(up-down)},
        { 0 , 0 , -2.0f/(zfar-znear) , -(zfar+znear)/(zfar-znear)},
        { 0 , 0 , 0 , 1.0f}
    };
    glUniformMatrix4fv(orto_location,1,GL_TRUE,&(mat[0][0]));
}

void load_model_matrix(float x,float y,float width,float heigth){
    float mat[4][4]={
        {width,0,0,x},
        {0,heigth,0,y},
        {0,0,1,0},
        {0,0,0,1}
    };
    glUniformMatrix4fv(model_location,1,GL_TRUE,&(mat[0][0]));
}

bool mouse_in_rect(Rectangle r){
    return ( mouse_x>=r.x && mouse_x<=r.x+r.width && mouse_y>=r.y && mouse_y<=r.y+r.height );
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!IMAGE AND FILE LOADING!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!

GLuint load_bmp(string path){
    ifstream file(path,ios::binary|ios::in);
    GLuint text_id;

    if(file.is_open()){
        char buffer[10];
        unsigned int width;
        unsigned int heigth;
        unsigned int bit_per_pixel;
        unsigned int data_position;

        file.read(buffer,2);
        file.read(buffer,4);
        file.read(buffer,2);
        file.read(buffer,2);
        file.read(buffer,4);

        memcpy(&data_position,buffer,sizeof(unsigned int));

        file.read(buffer,4);
        file.read(buffer,4);
        
        memcpy(&width,buffer,sizeof(unsigned int));

        file.read(buffer,4);

        memcpy(&heigth,buffer,sizeof(unsigned int));

        file.read(buffer,2);
        file.read(buffer,2);

        memcpy(&bit_per_pixel,buffer,sizeof(unsigned int));
        if(bit_per_pixel!=24 && bit_per_pixel!=32){
            cerr<<"Bfly error when loading image data for"<<path<<endl;
            cerr<<"image contains "<<bit_per_pixel<<" bits per pixel"<<endl;
            cerr<<"image is not 24 bits per pixel or 32 bits per pixel"<<endl;
            return 0;
        }
        file.seekg(data_position);
        char pixels[width][heigth*(bit_per_pixel/8)];
       for(int i=0;i<heigth;i++)
       for(int j=0;j<width;j++){
            unsigned int r;
            unsigned int g;
            unsigned int b;
            unsigned int a;

            file.read(buffer,1);
            memcpy(&b,buffer,sizeof(unsigned int));
            file.read(buffer,1);
            memcpy(&g,buffer,sizeof(unsigned int));
            file.read(buffer,1);
            memcpy(&r,buffer,sizeof(unsigned int));

            
            if(bit_per_pixel==32){
                file.read(buffer,1);
                memcpy(&a,buffer,sizeof(unsigned int));
                
                pixels[heigth-(i+1)][4*j]=g;
                pixels[heigth-(i+1)][4*j+1]=b;
                pixels[heigth-(i+1)][4*j+2]=r;
                pixels[heigth-(i+1)][4*j+3]=a;
            }
            else {
                pixels[heigth-(i+1)][3*j]=g;
                pixels[heigth-(i+1)][3*j+1]=b;
                pixels[heigth-(i+1)][3*j+2]=r;
            }
        }

        glGenTextures(1,&text_id);
        glBindTexture(GL_TEXTURE_2D,text_id);

        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    if(bit_per_pixel==24)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,width, heigth, 0,GL_RGB, GL_UNSIGNED_BYTE, &pixels);
                    else 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,width, heigth, 0,GL_RGBA, GL_UNSIGNED_BYTE, &pixels);
        file.close();
        return text_id;
    }

    cerr<<"Bfly couldn't open "<<path<<endl;
    return 0;
}

Font load_font(string path){
    Font fnt;
    ifstream file(path+".fnt");
    string line;
    while(getline(file,line)){
        if(!line.rfind("char id=",0)){
                int x,y,w,h,xo,yo,xa;
                int c;
                sscanf(line.c_str(),"char id=%d      x=%d    y=%d    width=%d    height=%d    xoffset=%d    yoffset=%d    xadvance=%d"
                ,&c,&x,&y,&w,&h,&xo,&yo,&xa);
                fnt.characters[c]={x,y,w,h,xo,yo,xa};
        }
        if(!line.rfind("common lineHeight",0)){
                int lin;
                int base;
                int sw;
                int sh;
                sscanf(line.c_str(),"common lineHeight=%d base=%d scaleW=%d scaleH=%d",&lin,&base,&sw,&sh);
                fnt.line_height=lin;
                fnt.base=base;
                fnt.scale_w=sw;
                fnt.scale_h=sh;
        }
        if(!line.rfind("info face=",0)){
                int size;
                char name[50];
                sscanf(line.c_str(),"info face=\"%s size=%d",name,&size);
                fnt.size=size;
                fnt.name=string(name);
                fnt.name.pop_back();
        }
    }
    fnt.font_atlas=load_bmp(path+".bmp");
    return fnt;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!INPUT HANDLING SECTION!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    key_pressed=codepoint;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action==GLFW_PRESS){
        mouse_hold=true;
    }
    if(action==GLFW_RELEASE && mouse_state==GLFW_PRESS){
        mouse_click=true;
        mouse_hold=false;
    }
    mouse_state=action;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x=(int)xpos;
    mouse_y=(int)ypos;
}


void Butterfly_Input(GLFWwindow *window){
    glfwPollEvents();
    glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window,mouse_button_callback);
    glfwSetCharCallback(window,character_callback);
    glfwSetCursorPosCallback(window,cursor_position_callback);
}

void Butterfly_Refresh(){
    key_pressed=-1;
    mouse_click=false;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!CONSTRUCTORS AND DATA INITIALIZATION!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void Styles_Initialize(){
    regular_font=load_font("./Resources/Butterfly");
    panel_style.font=regular_font;
    panel_style.color1={0.32f,0.34f,0.32f};
    panel_style.color2={0.22f,0.24f,0.22f};
    panel_style.border_width=1;
    panel_style.opacity=1.0f;
    panel_style.text_color={1.0f,1.0f,1.0f};
    panel_style.text_size=25;
    panel_style.border_color={0.9f,0.9f,0.9f};
}

void Butterfly_Initialize(){
    Styles_Initialize();

    GLuint vertex_shader=glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader=glCreateShader(GL_FRAGMENT_SHADER);
    ifstream file("./Resources/v_shader.vert");
    if(file.is_open()){
        stringstream ss;
        ss<<file.rdbuf();
        string s=ss.str();
        const char *text=s.c_str();
        glShaderSource(vertex_shader,1,&text,NULL);
        glCompileShader(vertex_shader);
        int result;
        glGetShaderiv(vertex_shader,GL_COMPILE_STATUS,&result);
        if(!result){
            char info[512];
            glGetShaderInfoLog(vertex_shader,512,NULL,info);
            cerr<<"Bfly Error compiling vertex shader"<<endl;
            cerr<<info<<endl;
        }
        file.close();
    }
    else cerr<<"error opening vertex shader"<<endl;

    file.open("./Resources/f_shader.frag");
    if(file.is_open()){
        stringstream ss;
        ss<<file.rdbuf();
        string s=ss.str();
        const char *text=s.c_str();
        glShaderSource(fragment_shader,1,&text,NULL);
        glCompileShader(fragment_shader);
        int result;
        glGetShaderiv(fragment_shader,GL_COMPILE_STATUS,&result);
        if(!result){
            char info[512];
            glGetShaderInfoLog(fragment_shader,512,NULL,info);
            cerr<<"Bfly Error compiling fragment shader"<<endl;
            cerr<<info<<endl;
        }
    }
    else cerr<<"error opening vertex shader"<<endl;

    program_id=glCreateProgram();
    glAttachShader(program_id,vertex_shader);
    glAttachShader(program_id,fragment_shader);
    glLinkProgram(program_id);

    int result;
    glGetProgramiv(program_id,GL_LINK_STATUS,&result);
    if(!result){
        char info[512];
        glGetProgramInfoLog(program_id,512,NULL,info);
        cerr<<"Bfly Error linking shaders"<<endl;
    }

    orto_location=glGetUniformLocation(program_id,"projection");
    model_location=glGetUniformLocation(program_id,"model");
    type_location=glGetUniformLocation(program_id,"type");
    color1_location=glGetUniformLocation(program_id,"color1");
    color2_location=glGetUniformLocation(program_id,"color2");
    opacity_location=glGetUniformLocation(program_id,"opacity");
    rect_size_location=glGetUniformLocation(program_id,"rect_size");
    border_width_location=glGetUniformLocation(program_id,"border_width");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float array[]={
        0.0 , 0.0 ,
        1.0 , 0.0 ,
        1.0 , 1.0 ,
        
        1.0 , 1.0 ,
        0.0 , 1.0 ,
        0.0 , 0.0
    };

    GLuint vbo;
    glGenVertexArrays(1,&vertices_id);
    glBindVertexArray(vertices_id);
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    
    glBufferData(GL_ARRAY_BUFFER,sizeof(array),array,GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glBindVertexArray(0);
}

GLuint make_text(string text,Style &style,float &text_width,float &text_height){
    float scale=(float)style.text_size/(float)style.font.line_height;
    GLuint vao;
    GLuint vbo;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    vector<float> arr;
    float x_it=0;
    float y_it=0;
    text_width=0;
    text_height=0;
    for(auto it:text){
        Glyph glyph=style.font.characters[it];

        float x0=x_it+glyph.x_off*scale;
        float x1=x0+glyph.width*scale;
        float y0=y_it+(glyph.y_off*scale);
        float y1=y0+glyph.height*scale;
        if(abs(y1)>text_height)
            text_height=abs(y1);
        float ux=(float)glyph.uv_x/(float)style.font.scale_w;
        float ux1=ux+(float)glyph.width/(float)style.font.scale_w;
        float uy=(float)glyph.uv_y/style.font.scale_h;
        float uy1=uy+(float)glyph.height/(float)style.font.scale_h;
        float ar[]={
            x0,y0 , ux,uy,
            x1,y0 , ux1,uy,
            x1,y1 , ux1,uy1,

            x1,y1 , ux1,uy1,
            x0,y1 , ux,uy1,
            x0,y0 , ux,uy
        };
        for(auto f:ar)
            arr.push_back(f);
        x_it+=glyph.x_adv*scale;
    }
    text_width=x_it;
    glBufferData(GL_ARRAY_BUFFER,arr.size()*sizeof(float),&arr[0],GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    return vao;
}

Panel::Panel(){
    rect={50,50,400,400};
    name="tmp_panel_"+to_string(component_counter++);
    tooltip="";
    header="header";
    elements.clear();
    style=panel_style;
    header_color1={0.4f,0.42f,0.41f};
    header_color2={0.34f,0.32f,0.33f};
    text_vao=make_text(header,style,header_width,header_height);
}

Button::Button():Component("tmp_button_"+to_string(component_counter++)){
    rect={75,75,100,35};
    tooltip="";
    text="button";
    style=panel_style;
    vao=make_text(text,style,text_width,text_height);
    image_id=0;
    press_color={style.color1.r+0.1f,style.color1.g+0.5f,style.color1.b+0.1f};
    click_color={style.color1.r+0.5f,style.color1.g+0.1f,style.color1.b+0.1f};
    click=nullptr;
}

ToggleButton::ToggleButton():Component("tmp_toggle_button"+to_string(component_counter++)){

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!DRAWING METHODS!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void draw_rectangle(Rectangle r){
    load_model_matrix((float)r.x,(float)r.y,(float)r.width,(float)r.height);
    glBindVertexArray(vertices_id);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
}

void draw_text(GLuint vao,int x,int y,int size){
    load_model_matrix((float)x,(float)y,1,1);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,size);
    glBindVertexArray(0);
}

void Panel::prepare_style(){
    glUniform3f(color1_location,style.color1.r,style.color1.g,style.color1.b);
    glUniform3f(color2_location,style.color2.r,style.color2.g,style.color2.b);
    glUniform1f(opacity_location,style.opacity);
    glUniform1i(type_location,0);
}

void Panel::prepare_style_text(){
    glUniform3f(color1_location,style.text_color.r,style.text_color.g,style.text_color.b);
    glUniform1f(opacity_location,style.opacity);
    glBindTexture(GL_TEXTURE_2D,style.font.font_atlas);
    glUniform1i(type_location,1);
}

void Panel::prepare_style_header(){
    glUniform3f(color1_location,header_color1.r,header_color1.g,header_color1.b);
    glUniform3f(color2_location,header_color2.r,header_color2.g,header_color2.b);
    glUniform1f(opacity_location,style.opacity);
    glUniform1i(type_location,0);
    
}

void Panel::draw(){
    glUniform1i(border_width_location,style.border_width);
    glUniform2f(rect_size_location,rect.width,rect.height);
    prepare_style();
    draw_rectangle(rect);
    prepare_style_header();
    glUniform2f(rect_size_location,rect.width,style.text_size+2);
    draw_rectangle({rect.x,rect.y,rect.width,style.text_size+2});
    prepare_style_text();
    draw_text(text_vao,rect.x+5,rect.y,header.size()*12);

    for(auto it:elements)
        it->draw();
}

void Button::draw(){
    glUniform1i(border_width_location,style.border_width);
    glUniform2f(rect_size_location,rect.width,rect.height);
    process_events();
    if(state==0){
        glUniform3f(color1_location,style.color1.r,style.color1.g,style.color1.b);
        glUniform3f(color2_location,style.color2.r,style.color2.g,style.color2.b);
        glUniform1i(type_location,0);
    }
    if(state==1){
        glUniform3f(color1_location,style.color1.r,style.color1.g+0.15f,style.color1.b);
        glUniform3f(color2_location,style.color2.r,style.color2.g+0.15f,style.color2.b);
        glUniform1i(type_location,0);
    }
    if(state==2){
        glUniform3f(color1_location,press_color.r,press_color.g,press_color.b);
        glUniform3f(color2_location,press_color.r,press_color.g,press_color.b);
        glUniform1i(type_location,0);
    }
    if(state==3){
        glUniform3f(color1_location,click_color.r,click_color.g,click_color.b);
        glUniform3f(color2_location,click_color.r,click_color.g,click_color.b);
        glUniform1i(type_location,0);
    }
    draw_rectangle(rect);

    glUniform3f(color1_location,style.text_color.r,style.text_color.g,style.text_color.b);
    glUniform1f(opacity_location,style.opacity);
    glBindTexture(GL_TEXTURE_2D,style.font.font_atlas);
    glUniform1i(type_location,1);
    float x_pos=rect.width-text_width;
    if(x_pos>0)
        x_pos/=2.0f;
    else 
        x_pos=0;
    float y_pos=rect.height-text_height;
    if(y_pos>0)
        y_pos/=2.0f;
    else 
        y_pos=0;
    draw_text(vao,rect.x+x_pos,rect.y+y_pos,style.text_size*12);

}

void ToggleButton::draw(){

}

void Window::draw(GLFWwindow *window){
    glUseProgram(program_id);
    int w,h;
    glfwGetFramebufferSize(window,&w,&h);
    load_projection_matrix(0,w,0,h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,w,h);

    glViewport(0,0,w,h);

    for(auto panel:panels)
        panel->draw();

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!EVENT PROCESSING METHODS!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void Button::process_events(){
    if(mouse_in_rect(rect)){
        if(mouse_click){
            state=3;
            if(click!=nullptr)
                click();
        }
        else
            if(mouse_hold)
                state=2;
        else
            state=1;
    }
    else
        state=0;
}

void ToggleButton::process_events(){
    
}

