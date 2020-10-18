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

    bfly::ComboItem *ci=new bfly::ComboItem();
    bfly::ComboItem *ci1=new bfly::ComboItem();
    bfly::ComboItem *ci2=new bfly::ComboItem();

    bfly::ToggleButton *tb=new bfly::ToggleButton();
    bfly::CheckBox *cb=new bfly::CheckBox();
    bfly::RadioButton *rb=new bfly::RadioButton();
    bfly::RadioButton *rb1=new bfly::RadioButton();
    rb->set_position(60,420);
    rb1->set_position(60,480);
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
    p->add_element(rb);
    p->add_element(rb1);
    tb->set_position(150,210);

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
    b4->set_size(300,100);
    b4->set_texture_id(2);
    b4->has_icon(true);
    b4->set_order(RIGHT_TO_LEFT);
    b4->set_text_size(70);
    b5->set_position(60,210);
    p->set_size(500,700);



    bfly::RadioGroup *rg=new bfly::RadioGroup();
    rg->add_to_group(rb);
    rg->add_to_group(rb1);
    ci->set_texture_id(2);

    bfly::ComboBox *cbox=new bfly::ComboBox();

    cbox->set_position(60,560);
    p->add_element(cbox);
    cbox->add_item(ci);
    cbox->add_item(ci1);
    cbox->add_item(ci2);
    ci->set_text("item1");
    ci1->set_text("item2");
    ci2->set_text("item3");


    bfly::ComboItem *ci21=new bfly::ComboItem();
    bfly::ComboItem *ci22=new bfly::ComboItem();

    ci1->add_item(ci21);
    ci1->add_item(ci22);
    ci21->set_text("item21");
    ci22->set_text("item22");

    bfly::ComboItem *ci31=new bfly::ComboItem();
    bfly::ComboItem *ci32=new bfly::ComboItem();
    bfly::ComboItem *ci33=new bfly::ComboItem();

    ci2->add_item(ci31);
    ci2->add_item(ci32);
    ci2->add_item(ci33);
    ci31->set_text("item31");
    ci32->set_text("item32");
    ci33->set_text("item33");

    bfly::ComboItem *ci331=new bfly::ComboItem();
    bfly::ComboItem *ci332=new bfly::ComboItem();
    bfly::ComboItem *ci3331=new bfly::ComboItem();
    ci33->add_item(ci331);
    ci331->set_text("item331");

    ci33->add_item(ci332);
    ci332->set_text("item332");

    ci331->add_item(ci3331);
    ci3331->set_text("item3331");


    bfly::Label *label=new bfly::Label();
    p->add_element(label);
    label->set_position(60,620);
    bfly::Slider *slider=new bfly::Slider();
    slider->set_position(500,660);
    p->add_element(slider);
    

    int t=650;
    while(!glfwWindowShouldClose(prozor)){
        glfwPollEvents();
        glfwSwapBuffers(prozor);
        glClearColor(0.9f,0.9f,0.9f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Butterfly_Input(prozor);
        w->draw(prozor);
        if(glfwGetKey(prozor,GLFW_KEY_0)){
            bfly::Slider *slider1=new bfly::Slider();
            slider1->set_position(t,660);
            p->add_element(slider1);
            t+=190;
        }
        Butterfly_Refresh();
    }
    return 0;
}
