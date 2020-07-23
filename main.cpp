#include <iostream>
#include "Butterfly.hpp"

using namespace std;



int main(){
    if(!glfwInit()){
        std::cout<<"nastala je greska u inicijalizaciji!"<<std::endl;
    }
    GLFWwindow *prozor=glfwCreateWindow(1600,800,"window",NULL,NULL);
    glfwMakeContextCurrent(prozor);

    gladLoadGL();
    Butterfly_Initialize();
    bfly::Window *w=new bfly::Window();
    bfly::Panel *p=new bfly::Panel();
    bfly::Panel *p2=new bfly::Panel();
    p2->set_position(600,300);
    w->add_panel(p);
    w->add_panel(p2);
    bfly::Button *b=new bfly::Button();
    bfly::Button *b1=new bfly::Button();
    bfly::Button *b2=new bfly::Button();
    bfly::Button *b3=new bfly::Button();
    bfly::Button *b4=new bfly::Button();
    bfly::Button *b5=new bfly::Button();

    bfly::ToggleButton *tb=new bfly::ToggleButton();
    bfly::CheckBox *cb=new bfly::CheckBox();
    cb->set_position(60,260);

    b1->set_position(60,140);
    b->set_position(60,90);
    b2->set_position(175,90);
    b2->set_size(100,85);
    b3->set_position(290,90);
    b3->set_size(100,85);
    p->add_element(b);
    p->add_element(b1);
    p->add_element(b2);
    p->add_element(b3);
    p->add_element(b4);
    p->add_element(b5);
    p->add_element(tb);
    p->add_element(cb);
    tb->set_position(200,210);

    b->set_texture_id(2);
    b1->set_texture_id(2);
    b2->set_texture_id(2);
    b3->set_texture_id(2);
    b2->has_render_rect(false);
    b3->set_text("");
    b->has_icon(true);
    b1->has_icon(true);
    b1->set_order(RIGHT_TO_LEFT);

    b4->set_position(60,300);
    b4->set_size(200,100);
    b4->set_texture_id(2);
    b4->has_icon(true);
    b4->set_order(RIGHT_TO_LEFT);
    b4->set_text_size(70);
    b5->set_position(60,210);
    while(!glfwWindowShouldClose(prozor)){
        glfwPollEvents();
        Butterfly_Input(prozor);
        glfwSwapBuffers(prozor);
        glClearColor(0.9f,0.9f,0.9f,1);
        glClear(GL_COLOR_BUFFER_BIT);
        w->draw(prozor);

        Butterfly_Refresh();
    }
    return 0;
}