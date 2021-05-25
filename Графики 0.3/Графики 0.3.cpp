#include <windows.h>
#include <iostream>
#include <GL\glut.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>  
#include <map>
#include <fstream>


using namespace std;

map <string, string> slovar;
vector<string>formulas;
string path = "Formulas1.txt";
int X_COORD = 50;// X - размерность ] должны
int Y_COORD = 50;// Y - размерность ] быть равными
const float ITERATIONS = 0.1;// прорисовка графика (чем меньше тем лучше)

int x_off = X_COORD / 2;// начало
int y_off = Y_COORD / 2;// оси координат

int mainWindow;
string formula;
//исходная функция
#define expr x

void drawgrid(float SERIF_OFFSET, float SERIF_DISTANCE) {
    glBegin(GL_LINES);
    //задаем цвета
    glColor3f(0.0, 0.0, 0.0);

    //рисуем координатные оси
    //горизонталь
    glVertex2f(-900.0, Y_COORD / 2);
    glVertex2f(900, Y_COORD / 2);
    //засечки по горизонтали
    int p = X_COORD / 2;
    for (int i = -900; i < 900; i += SERIF_DISTANCE, p -= SERIF_DISTANCE) {
        glVertex2f(i, Y_COORD / 2);
        glVertex2f(i, Y_COORD / 2 + SERIF_OFFSET);

        glVertex2f(p, Y_COORD / 2);
        glVertex2f(p, Y_COORD / 2 + SERIF_OFFSET);
    }
    //вертикаль
    int t = Y_COORD / 2;
    glVertex2f(X_COORD / 2, -900);
    glVertex2f(X_COORD / 2, 900.0);
    //засечки по вертикали
    for (int i = -900; i < 900; i += SERIF_DISTANCE, t -= SERIF_DISTANCE) {
        glVertex2f(X_COORD / 2, i);
        glVertex2f(Y_COORD / 2 + SERIF_OFFSET, i);

        glVertex2f(X_COORD / 2, t);
        glVertex2f(Y_COORD / 2 + SERIF_OFFSET, t);
    }
    glEnd();
}
bool operation(char c) {
    for (auto it = slovar.begin(); it != slovar.end(); ++it)
        if (it->second[0] == c)
            return true;
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
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
        else if (-op == slovar["sin"][0])value.push_back(sin(unitar));
        else if (-op == slovar["cos"][0])value.push_back(cos(unitar));
        else if (-op == slovar["tg"][0])value.push_back(tan(unitar));
        else if (-op == slovar["abs"][0])value.push_back(abs(unitar));
        else if (-op == slovar["arccos"][0])value.push_back(acos(unitar));
        else if (-op == slovar["arcsin"][0])value.push_back(asin(unitar));
        else if (-op == slovar["ch"][0])value.push_back(cosh(unitar));
        else if (-op == slovar["sh"][0])value.push_back(sinh(unitar));
        else if (-op == slovar["th"][0])value.push_back(tanh(unitar));
        else if (-op == slovar["arctan"][0])value.push_back(atan(unitar));
        else if (-op == slovar["arcsh"][0])value.push_back(asinh(unitar));
        else if (-op == slovar["arcch"][0])value.push_back(acosh(unitar));
        else if (-op == slovar["arcth"][0])value.push_back(atanh(unitar));
        else if (-op == slovar["ln"][0])value.push_back(log(unitar));
        else if (-op == slovar["log10"][0])value.push_back(log10(unitar));
        else if (-op == slovar["log2"][0])value.push_back(log2(unitar));
        else if (-op == slovar["e"][0])value.push_back(exp(unitar));
        else if (-op == slovar["sqrt"][0])value.push_back(sqrt(unitar));


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
    glBegin(GL_LINE_STRIP);
    glPointSize(15);
    glLineWidth(40);
    float y = 0;
    glColor3f(0.8, 0.0, 0.8);
   /* string formula;
    cin >> formula;*/
    float y2= yfunction(-200, formula);
    bool endpaint = false;
    for (float x = -100 * 2; x < 100 * 2; x += ITERATIONS) {
        //перерасчитываем координаты
       // cout << "x = " << x << endl;
        y = yfunction(x, formula);
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

void movecamera(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:
        {
            glTranslatef(0, 1, 0);
            break;
        }
        case GLUT_KEY_DOWN:
        {
            glTranslatef(0, -1, 0);
            break;
        }
        case GLUT_KEY_LEFT:
        {
            glTranslatef(-1, 0, 0);
            break;
        }
        case GLUT_KEY_RIGHT:
        {
            glTranslatef(1, 0, 0);
            break;
        }
        default:
        {

            cout << key << endl;
            break;
        }
    
        /*if (GetKeyState(VK_UP) < 0)glTranslatef(0, 1, 0);
        else if (GetKeyState(VK_DOWN) < 0)glTranslatef(0, -1, 0);
        else if (GetKeyState(VK_LEFT) < 0)glTranslatef(-1, 0, 0);
        else if (GetKeyState(VK_RIGHT) < 0)glTranslatef(1, 0, 0);*/
    }
    glutSetWindow(mainWindow);
    glutPostRedisplay();

}
void scalecamera(unsigned char key , int x, int y)
{
    switch (key)
    {
    case '+':
    {
        glScalef(1.1, 1.1, 0);
        glTranslatef(-2.3, -2.3, 0);
        /*X_COORD +=20;
        Y_COORD +=20;*/
        break;
    }
    case '-':
    {
        glScalef(1/1.1, 1 / 1.1, 0);
        glTranslatef(2.5, 2.5, 0);
        break;
    }
    default:
    {

        cout << key << endl;
        break;
    }
    /*if (GetKeyState(VK_UP) < 0)glTranslatef(0, 1, 0);
    else if (GetKeyState(VK_DOWN) < 0)glTranslatef(0, -1, 0);
    else if (GetKeyState(VK_LEFT) < 0)glTranslatef(-1, 0, 0);
    else if (GetKeyState(VK_RIGHT) < 0)glTranslatef(1, 0, 0);*/
    }
    x_off = X_COORD / 2;
    y_off = Y_COORD / 2;
    glutSetWindow(mainWindow);
    glutPostRedisplay();

}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //cout << "Osnovnie toshki po vashemu grafiku: \n";
    glutSpecialFunc(movecamera);
    glutKeyboardFunc(scalecamera);
    drawgrid(0.3, 1);
    drawfunc();
    //funcinfo(-5, 5);

    glutSwapBuffers();
    glFlush();
}
void StringHandler()
{
    for (map<string, string>::iterator it = slovar.begin(); it != slovar.end(); ++it) {
        size_t pos = 0;
        while ((pos = formula.find(it->first, pos)) != string::npos)
        {
            formula.replace(pos, it->first.size(), it->second);
            pos += it->second.size();
        }
    }
    cout << formula << endl;
}
void ReadFormulasStreams()
{
    ifstream f;
    f.open(path);
    string formulastream;
    while (!f.eof())
    {
        f >> formulastream;
        formulas.push_back(formulastream);
        cout << formulas[0] << endl;
    }
}
void WriteFormulaToFile(string formulastream)
{
    ofstream f;
    f.open(path);
    f << formulastream;
    f.close();
}
void WriteInfoTextOnScreen()
{
    cout << "Список поддерживаемых формул:\n\n Квадратный корень - sqrt()\n Синус - sin()\n Косинус - cos()\n Тангенс - tg()\n Модуль числа - abs()\n Арксинус - arcsin()\n";
    cout << " Арккосинус - arccos()\n Арктангенс - arctan()\n Шинус - sh()\n Кошинус - ch()\n Гиперболический тангенс - th()\n Аркшинус - arcsh()\n Арккошинус - arcch()\n Арк-гиперболический тангенс - arcth()\n";
    cout << " Натуральный логарифм - ln()\n Десятичный логарифм - log10()\n Двоичный логарифм - log2()\n Экспонента в степени (степень в скобках) - e()\n\n\n";
}
int main(int argc, char** argv) {
    ReadFormulasStreams();
    slovar["sin"] = "s";
    slovar["cos"] = "c";
    slovar["tg"] = "t";
    slovar["abs"] = "a";
    slovar["arccos"] = "q";
    slovar["arcsin"] = "w";
    slovar["ch"] = "r";
    slovar["sh"] = "d";
    slovar["th"] = "g";
    slovar["arctan"] = "h";
    slovar["arcsh"] = "j";
    slovar["arcch"] = "k";
    slovar["arcth"] = "l";
    slovar["ln"] = "m";
    slovar["log10"] = "n";
    slovar["log2"] = "b";
    slovar["e"] = "v";
    slovar["sqrt"] = "W";
    setlocale(LC_ALL, "Russian");
    
    cout << "1. История формул" << endl;
    cout << "2. Ввод формулы" << endl;
    cout << "0. Выйти из программы" << endl << "  >>";
    char choice;
    cin >> choice; 
    if (choice == '0') return 0;
    if (choice == '2') 
    cout << "Enter function formula: y=";
    cin >> formula;
    cout << formula;
    WriteFormulaToFile(formula);
    StringHandler();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 100);
    mainWindow = glutCreateWindow("Графики");
    //s
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //пространство координат
    glOrtho(0.0, X_COORD, 0.0, Y_COORD, -1.0, 1.0);

    glutDisplayFunc(display);
    glutMainLoop();
}