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
    b1->set_position(60,140);
    b->set_position(60,90);
    p->add_element(b);
    p->add_element(b1);
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