#include <windows.h>
#include <iostream>
#include <GL\glut.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>  

using namespace std;

const int X_COORD = 50;// X - размерность ] должны
const int Y_COORD = 50;// Y - размерность ] быть равными
const float ITERATIONS = 0.06;// прорисовка графика (чем меньше тем лучше)

int x_off = X_COORD / 2;// начало
int y_off = Y_COORD / 2;// оси координат

//исходная функция
#define expr x

void drawgrid(float SERIF_OFFSET, float SERIF_DISTANCE) {
    glBegin(GL_LINES);
    //задаем цвета
    glColor3f(0.0, 0.0, 0.0);

    //рисуем координатные оси
    //горизонталь
    glVertex2f(0.0, Y_COORD / 2);
    glVertex2f(X_COORD, Y_COORD / 2);
    //засечки по горизонтали
    int p = X_COORD / 2;
    for (int i = X_COORD / 2; i < X_COORD; i += SERIF_DISTANCE, p -= SERIF_DISTANCE) {
        glVertex2f(i, Y_COORD / 2);
        glVertex2f(i, Y_COORD / 2 + SERIF_OFFSET);

        glVertex2f(p, Y_COORD / 2);
        glVertex2f(p, Y_COORD / 2 + SERIF_OFFSET);
    }
    //вертикаль
    int t = Y_COORD / 2;
    glVertex2f(X_COORD / 2, Y_COORD);
    glVertex2f(X_COORD / 2, 0.0);
    //засечки по вертикали
    for (int i = Y_COORD / 2; i < Y_COORD; i += SERIF_DISTANCE, t -= SERIF_DISTANCE) {
        glVertex2f(X_COORD / 2, i);
        glVertex2f(Y_COORD / 2 + SERIF_OFFSET, i);

        glVertex2f(X_COORD / 2, t);
        glVertex2f(Y_COORD / 2 + SERIF_OFFSET, t);
    }
    glEnd();
}
bool operation(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's' || c == 'c' || c == 't' || c == 'a';
}

//данная функция будет возвращать приоритет поступившей операции
int prioritet(char op) {
    if (op < 0) return 3;
    else {
        if (op == '+' || op == '-')return 1;
        else if (op == '*' || op == '/')return 2;
        else if (op == '^')return 4;
        else return -1;
    }
}

//следующая функция описывает принцип работы каждого оператора
void action(vector<float>& value, char op) {
    if (op < 0) {                            //для унарных операций
        float unitar = value.back();
        value.pop_back();
        if (-op == '-')value.push_back(-unitar);
        else if (-op == 's')value.push_back(sin(unitar));
        else if (-op == 'c')value.push_back(cos(unitar));
        else if (-op == 't')value.push_back(tan(unitar));
        else if (-op == 'a')value.push_back(abs(unitar));
    }
    else {                               //для бинарных операций
        float right = value.back();
        value.pop_back();
        float left = value.back();
        value.pop_back();
        if (op == '+')value.push_back(left + right);
        else if (op == '-')value.push_back(left - right);
        else if (op == '*')value.push_back(left * right);
        else if (op == '/')value.push_back(left / right);
        else if (op == '^')value.push_back(pow(left, right));
    }
}
float calculus(string& formula) {

    bool unary = true;        //создадим булевскую переменную, для распределения операторов на унарные и бинарные
    vector<float>value;        //заведем массив для целых чисел
    vector<char>op;           //и соответственно для самых операторов
    for (int i = 0; i < formula.size(); i++) {
        if (formula[i] == '(') {    //если текущий элемент — открывающая скобка, то положим её в стек
            op.push_back('(');
            unary = true;
        }
        else if (formula[i] == ')') {
            while (op.back() != '(') {  //если закрывающая скобка - выполняем все операции, находящиеся внутри этой скобки
                action(value, op.back());
                op.pop_back();
            }
            op.pop_back();
            unary = false;
        }
        else if (operation(formula[i])) { //если данный элемент строки является одни из выше перечисленных операндов,то
            char zn = formula[i];
            if (unary == true)zn = -zn; //придает отрицательное значение, для распознавания функции унарности оператора 
            while (!op.empty() && prioritet(op.back()) >= prioritet(zn)) {
                action(value, op.back());   //выполняем сами алгебраические вычисления, где все уже операции упорядочены  
                op.pop_back();              //в одном из стеков по строгому убыванию приоритета, если двигаться от вершины
            }
            op.push_back(zn);
            unary = true;
        }
        else {
            string number;      //заведем строку для найденных числовых операндов
            while (i < formula.size() && (isdigit(formula[i]) || formula[i] == '.')) {
                number += formula[i++];//распознаем их с помощью библиотечной функции строк
            }
            i--;
            value.push_back(atof(number.c_str()));//поместим в наш стек с числовыми выражениями
            unary = false;
        }
    }
    while (!op.empty()) {     //выполним еще не использованные операции в стеке 
        action(value, op.back());
        op.pop_back();
    }
    return value.back(); //получим на выходе значение выражения
}
float yfunction(float X, string formula)
{
    string x = to_string(X); //введем две строки: сам многочлен/числовое выражение и по необходимости значение переменной 
    if (x[0] == '-') {  //для слаженной работы унитарного минуса при отрицательном значении переменной, заключим его в скобки
        x.insert(x.begin(), '(');
        x.insert(x.end(), ')');
    }
    //cout << x << endl;
    for (int i = 0; i < formula.size(); i++) {
        if (formula[i] == 'x') {
            formula.erase(i, 1);
            formula.insert(i, x);//проведем замену
        }
    }
   // cout << " " <<  calculus(formula) << " ";
    return calculus(formula);  //выведем ответ
}
void drawfunc() {
    //рисуем график
    glBegin(GL_POINTS);
    glPointSize(15);
    glLineWidth(40);
    float y = 0;
    glColor3f(0.8, 0.0, 0.8);
    /*string formula;
    cin >> formula;*/
    for (float x = -X_COORD * 2; x < X_COORD * 2; x += ITERATIONS) {
        //перерасчитываем координаты
       // cout << "x = " << x << endl;
        y = yfunction(x, "s(x)");
        glPointSize(90);
        glVertex2f(x_off + x, y_off + y);//не убирать x и y!! это оффсет по осям!
    }
    glEnd();
}

void funcinfo(int val1, int val2) {
    //информация о графике
    for (float x = val1; x <= val2; x++) {
        float j = expr;
        cout << x << " : " << j << endl;
    }
}
void movecamera()
{
     if (GetKeyState(VK_UP) <0)glTranslatef(0, 1, 0);
     else if (GetKeyState(VK_DOWN) < 0)glTranslatef(0, -1, 0);

}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //cout << "Osnovnie toshki po vashemu grafiku: \n";
        movecamera();
        drawgrid(0.3, 1);
        drawfunc();
    //funcinfo(-5, 5);

    glutSwapBuffers();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 100);
    glutCreateWindow("Графики");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //пространство координат
    glOrtho(0.0, X_COORD, 0.0, Y_COORD, -1.0, 1.0);

    glutDisplayFunc(display);
    glutMainLoop();
}