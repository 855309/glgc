#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

#include <imgui.h>
#include <imgui-SFML.h>

#include "expr.hpp"
#include "file.hpp"

#include <SFML/Graphics.hpp>

#define WIDTH 1280
#define HEIGHT 720

#define GRID_SPACE 10

#define PIX_SIZE 2

#define NLINE 40

// sf::ContextSettings csettings;
sf::RenderWindow mwindow(sf::VideoMode(WIDTH, HEIGHT), "glgc");

class function
{
    public:
        std::string equation;
        bool visible;

        sf::Color color;

        std::vector<sf::Vector2f> points;

        function(std::string eq, bool v, sf::Color col){
            this->equation = eq;
            this->visible = v;
            this->color = col;
        }
};

struct langdef{
    int ln;
    std::string lang;
    std::string text;
};

std::vector<function> functions;

std::vector<sf::Color> cols = {
    sf::Color(75, 182, 105),
    sf::Color(194, 106, 200),
    sf::Color(223, 203, 63),
    sf::Color(90, 87, 215),
    sf::Color(215, 87, 140)
};

int rand_int(int a, int b){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(a, b);

    return dist(mt);
}

void add_func(std::string func){
    functions.push_back(function(func, true, cols[rand_int(0, cols.size() - 1)]));
}

void draw_point_at(float x, float y, sf::Color color = cols[0]){
    float xdw = (WIDTH / 2) + NLINE * x;
    float ydw = (HEIGHT / 2) - NLINE * y;

    sf::CircleShape rpix;

    rpix.setFillColor(color);
    rpix.setOutlineThickness(0);

    rpix.setPosition(xdw - PIX_SIZE, ydw - PIX_SIZE);
    rpix.setRadius(PIX_SIZE);

    mwindow.draw(rpix);
}

void draw_line_at(sf::Vector2f lf1, sf::Vector2f lf2, sf::Color color = cols[0]){
    float xdw = (WIDTH / 2) + NLINE * lf1.x;
    float ydw = (HEIGHT / 2) - NLINE * lf1.y;

    float x2dw = (WIDTH / 2) + NLINE * lf2.x;
    float y2dw = (HEIGHT / 2) - NLINE * lf2.y;

    sf::Vertex arr[] = {
        sf::Vertex(sf::Vector2(xdw, ydw)),
        sf::Vertex(sf::Vector2(x2dw, y2dw))
    };

    arr->color = color;

    sf::VertexBuffer buf;
    buf.update(arr);
    buf.setPrimitiveType(sf::PrimitiveType::Lines);

    // std::cout << xdw << "-" << ydw << ", " << x2dw << "-" << y2dw << std::endl;

    mwindow.draw(buf);
}

void gen_func(function *func){
    double delta = 0.01;

    double sp = 0 - ((WIDTH / 2) / NLINE);
    double ep = 0 + ((WIDTH / 2) / NLINE);

    double ylimp = 0 + ((HEIGHT / 2) / NLINE);
    double ylimn = 0 - ((HEIGHT / 2) / NLINE);

    double current_x = sp;

    exprtk::symbol_table<double> table;
    table.add_variable("x", current_x);

    exprtk::expression<double> expression;
    expression.register_symbol_table(table);

    exprtk::parser<double> parser;
    parser.compile(func->equation, expression);

    for(double rg = sp; rg <= ep; rg += delta){
        current_x = rg;

        double vl = expression.value();

        if(vl <= ylimp && vl >= ylimn){
            func->points.push_back(sf::Vector2f(rg, vl));
        }
    }
}

sf::Color imvec_to_sfcolor(ImVec4 imcol){
    return sf::Color(imcol.x, imcol.y, imcol.z, 255 - imcol.w);
}

double limitgap = 0.1;
void draw_func(function *func){
    if(func->points.size() == 0){
        gen_func(func);
    }

    sf::Vector2f lpos;
    for(sf::Vector2f pos : func->points){
        draw_point_at(pos.x, pos.y, func->color);

        /*if(abs((lpos.y - pos.y)) > limitgap){
            draw_line_at(lpos, pos, sf::Color::Black);
        }*/

        lpos = pos;
    }
}

std::string lang = "English";
std::vector<langdef> lnp;
std::vector<std::string> langs;
void cachelang(){
    std::vector<std::string> linelist = file_readlines("lang.ini");
    
    int kl = 0;
    std::string langf = "undefined";
    for(std::string line : linelist){
        if(line[0] == '[' && line.back() == ']'){
            kl = 0;
            langf = line.substr(1, line.size() - 2);
            langs.push_back(langf);
        }
        else{
            langdef def = {
                .ln = kl,
                .lang = langf,
                .text = line
            };

            lnp.push_back(def);
            kl++;
        }
    }
}

std::string get_langtext(int f){
    for(langdef d : lnp){
        if(d.ln == f && d.lang == lang){
            return d.text;
        }
    }

    return "undefined";
}

bool init = false;
void langinit(){
    if(!init){
        ImVec2 size(160, 70);

        ImGui::SetNextWindowPos(ImVec2(WIDTH / 2 - size.x / 2, HEIGHT / 2 - size.y / 2));
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Language");

        for(std::string l : langs){
            ImGui::SameLine();
            if(ImGui::Button(l.c_str())){
                lang = l;
                init = !init;
            }
        }

        ImGui::End();
    }
}

bool sett_init = false;
bool win1_init = false;
bool win2_init = false;
bool win3_init = false;
bool win4_init = false;

void sm_bl(bool &state, std::function<void()> scr){
    if(!state){
        // ((void(*)())scr)(); // call the function that pointed by void pointer
        scr();
        state = !state;
    }
}

int main(int argc, char **argv){
    mwindow.setVerticalSyncEnabled(true);
    // csettings.antialiasingLevel = 8;

    ImGui::SFML::Init(mwindow);

    cachelang();

    sf::Clock deltaClock;

    sf::Color gridcolor(200, 200, 200, 100);

    bool picker = false;
    static float pickercol[3] = {0, 0, 0};
    function *pickerf;

    while (mwindow.isOpen()){
        sf::Event event;
        while (mwindow.pollEvent(event)){
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed){
                mwindow.close();
            }
        }

        mwindow.clear(sf::Color::White);

        /* Drawing the grid */
        float th = 1.5f;

        for(int x = 1; x <= WIDTH / GRID_SPACE; x++){
            sf::RectangleShape grid_line;

            int c = x * GRID_SPACE;

            grid_line.setFillColor(gridcolor);
            grid_line.setOutlineThickness(0);

            grid_line.setPosition(c, 0);
            grid_line.setSize(sf::Vector2f(th, HEIGHT));

            mwindow.draw(grid_line);
        }

        for(int y = 1; y <= HEIGHT / GRID_SPACE; y++){
            sf::RectangleShape grid_line;

            int c = y * GRID_SPACE;

            grid_line.setFillColor(gridcolor);
            grid_line.setOutlineThickness(0);

            grid_line.setPosition(0, c);
            grid_line.setSize(sf::Vector2f(WIDTH, th));

            mwindow.draw(grid_line);
        }

        for(int i = 0; i < functions.size(); i++){
            if(functions[i].visible){
                draw_func(&functions[i]);
            }
        }

        /* Draw the x, y lines */
        float xyw = 1.5f;

        sf::RectangleShape xline;
        xline.setFillColor(sf::Color::Black);
        xline.setOutlineThickness(0);

        xline.setPosition(WIDTH / 2, 0);
        xline.setSize(sf::Vector2f(xyw, HEIGHT));

        mwindow.draw(xline);

        sf::RectangleShape yline;
        yline.setFillColor(sf::Color::Black);
        yline.setOutlineThickness(0);

        yline.setPosition(0, HEIGHT / 2);
        yline.setSize(sf::Vector2f(WIDTH, xyw));

        mwindow.draw(yline);

        /* Initialize the imgui window */
        ImGui::SFML::Update(mwindow, deltaClock.restart());
        langinit();
        if(!init){
            ImGui::SFML::Render(mwindow);

            mwindow.display();
            continue;
        }

        /* Input function window */
        sm_bl(win1_init, []() {
            ImGui::SetNextWindowPos(ImVec2(910, 600));
        });
        ImGui::SetNextWindowSize(ImVec2(340, 100));
        ImGui::Begin(get_langtext(0).c_str());

        ImGui::Text(get_langtext(1).c_str());

        ImGui::SameLine();

        static char fnc_text[100] = "";
        ImGui::InputTextWithHint("", get_langtext(2).c_str(), fnc_text, 100);

        if(ImGui::Button(get_langtext(3).c_str())){
            add_func(std::string(fnc_text));
        }

        ImGui::End();

        /* Function list window */
        sm_bl(win2_init, []() {
            ImGui::SetNextWindowPos(ImVec2(980, 20));
        });
        ImGui::SetNextWindowSize(ImVec2(270, 270));
        ImGui::Begin(get_langtext(4).c_str());

        std::string prefix = "f(x) = ";

        int g = functions.size();
        for(int m = 0; m < g; m++){
            function *func = &functions[m];

            ImGui::Checkbox(("##" + std::to_string(m)).c_str(), &func->visible);

            ImGui::SameLine();
            ImGui::Text((prefix + func->equation).c_str());

            ImGui::SameLine();
            if(ImGui::Button(std::string(std::string(get_langtext(5) + "##") + std::to_string(m).c_str()).c_str())){
                functions.erase(functions.begin() + m, functions.begin() + m+1);
                
                g--;
                m--;
            }

            ImGui::SameLine();
            if(ImGui::ColorButton(std::to_string(m).c_str(), ImVec4((float)((float)func->color.r / 255), (float)((float)func->color.g / 255), (float)((float)func->color.b / 255), (float)((float)func->color.a / 255)))){
                picker = true;
                pickerf = func;
            }
        }

        ImGui::End();

        if(picker){
            sm_bl(win3_init, []() {
                ImGui::SetNextWindowPos(ImVec2(20, 20));
            });
            ImGui::SetNextWindowSize(ImVec2(280, 290));
            ImGui::Begin(get_langtext(6).c_str());
            
            ImGui::ColorPicker4("", pickercol, ImGuiColorEditFlags_RGB);
    
            if(ImGui::Button(get_langtext(7).c_str())){
                pickerf->color = sf::Color(pickercol[0]*255, pickercol[1]*255, pickercol[2]*255);
                // std::cout << (int)pickerf->color.r << ", " << (int)pickerf->color.g << ", " << (int)pickerf->color.b << std::endl;
                picker = false;
            }

            ImGui::SameLine();
            if(ImGui::Button(get_langtext(8).c_str())){
                picker = false;
            }

            ImGui::End();
        }

        /* Settings window */
        sm_bl(sett_init, []() {
            ImGui::SetNextWindowPos(ImVec2(1121, 536));
            ImGui::SetNextWindowCollapsed(true);
        });
        ImGui::Begin(get_langtext(9).c_str());
        
        if(ImGui::Button(get_langtext(10).c_str())){
            init = !init;
        }

        ImGui::End();

        ImGui::SFML::Render(mwindow);

        mwindow.display();
    }
}
