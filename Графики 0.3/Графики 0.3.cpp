#include <windows.h>
#include <iostream>
#include <GL\glut.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>  
#include <map>
#include <fstream>
#include <stdlib.h>  


using namespace std;

map <string, string> slovar;
vector<string>formulas;
string path = "Formulas1.txt";
int X_COORD = 50;// Размерность оси координат по оси X и Y
int Y_COORD = X_COORD;
const float ITERATIONS = 0.1;// Размер шага для вычисления значения для функции. Для построения более точного графика значение понизить

int x_off = X_COORD / 2; //Начало оси координат
int y_off = Y_COORD / 2;

int mainWindow;
string formula;


void drawgrid(float SERIF_OFFSET, float SERIF_DISTANCE) {
    glBegin(GL_LINES);
    //Цвет для отрисовки осей и засечек
    glColor3f(0.0, 0.0, 0.0);

    //Горизонтальная ось
    glVertex2f(-900.0, Y_COORD / 2);
    glVertex2f(900, Y_COORD / 2);
    //Засечки по горизонтали
    int p = X_COORD / 2;
    for (int i = -900; i < 900; i += SERIF_DISTANCE, p -= SERIF_DISTANCE) {
        glVertex2f(i, Y_COORD / 2);
        glVertex2f(i, Y_COORD / 2 + SERIF_OFFSET);

        glVertex2f(p, Y_COORD / 2);
        glVertex2f(p, Y_COORD / 2 + SERIF_OFFSET);
    }
    //Вертикальная ось
    int t = Y_COORD / 2;
    glVertex2f(X_COORD / 2, -900);
    glVertex2f(X_COORD / 2, 900.0);
    //Засечки по вертикали
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


//Функций, возвращающая приоритет поступившей операции
int prioritet(char op) {
    if (op < 0) return 3;
    else {
        if (op == '+' || op == '-')return 1;
        else if (op == '*' || op == '/')return 2;
        else if (op == '^')return 4;
        else return -1;
    }
}

//Функция, которая описывает работу кажного оператора для программы
void action(vector<float>& value, char op) {
    if (op < 0) {                           
        //Для унарных операций
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
    else {                               
        //Для бинарных операций
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

    bool unary = true;        //Создадим булевскую переменную, для распределения операторов на унарные и бинарные
    vector<float>value;       //Создадим массив для целых чисел
    vector<char>op;           //Создадим масссив для операторов
    for (int i = 0; i < formula.size(); i++) {
        if (formula[i] == '(') {    //Если текущий элемент — открывающая скобка, то положим её в стек
            op.push_back('(');
            unary = true;
        }
        else if (formula[i] == ')') {
            while (op.back() != '(') {  //Если закрывающая скобка - выполняем все операции, находящиеся внутри этой скобки
                action(value, op.back());
                op.pop_back();
            }
            op.pop_back();
            unary = false;
        }
        else if (operation(formula[i])) { //Если данный элемент строки является одни из выше перечисленных операндов,то
            char zn = formula[i];
            if (unary == true)zn = -zn; //Придает отрицательное значение, для распознавания функции унарности оператора 
            while (!op.empty() && prioritet(op.back()) >= prioritet(zn)) {
                action(value, op.back());   //Выполняем сами алгебраические вычисления, где все уже операции упорядочены  
                op.pop_back();              //В одном из стеков по строгому убыванию приоритета, если двигаться от вершины
            }
            op.push_back(zn);
            unary = true;
        }
        else {
            string number;      //Заведем строку для найденных числовых операндов
            while (i < formula.size() && (isdigit(formula[i]) || formula[i] == '.')) {
                number += formula[i++];//Распознаем их с помощью библиотечной функции строк
            }
            i--;
            value.push_back(atof(number.c_str()));//Поместим в наш стек с числовыми выражениями
            unary = false;
        }
    }
    while (!op.empty()) {     //Выполним еще не использованные операции в стеке 
        action(value, op.back());
        op.pop_back();
    }
    return value.back(); //Получим на выходе значение выражения
}
float yfunction(float X, string formula)
{
    string x = to_string(X); //ВВедем две строки: сам многочлен/числовое выражение и по необходимости значение переменной 
    if (x[0] == '-') {  //Для нормальной работы унитарного минуса при отрицательном значении переменной, заключим его в скобки
        x.insert(x.begin(), '(');
        x.insert(x.end(), ')');
    }
    //cout << x << endl;
    for (int i = 0; i < formula.size(); i++) {
        if (formula[i] == 'x') {
            formula.erase(i, 1);
            formula.insert(i, x);//Проведем замену
        }
    }
    return calculus(formula);  //Выведем ответ
}
void drawfunc() {
    //Функция отрисовки графика
    glBegin(GL_LINE_STRIP); //Если поставить кол-во итераций как можно меньше, то можно использовать GL_POINTS,
                            //дабы избежать проблем с соединением точек, которые не должны быть соединены.
    glPointSize(15);
    glLineWidth(40);
    float y = 0;
    glColor3f(0.8, 0.0, 0.8);
   
    for (float x = -100 * 2; x < 100 * 2; x += ITERATIONS) {
        y = yfunction(x, formula);
        glPointSize(90);
        glVertex2f(x_off + x, y_off + y);//Относительно центра координат ставим точку по x и y
    }
    glEnd();
}

void movecamera(int key, int x, int y)
{
    //Перемещение камеры, стрелки на клавиатуре
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
    }
    glutSetWindow(mainWindow);
    glutPostRedisplay();

}
void scalecamera(unsigned char key , int x, int y)
{
    //Приближение и отдаление камеры
    //По желанию можно настроить кнопки, дебаг для определения названия клавиши ниже
    switch (key)
    {
    case '+':
    {
        glScalef(1.1, 1.1, 0);
        glTranslatef(-2.3, -2.3, 0);
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
        //Дебаг для определения клавиши
        /*cout << key << endl;*/
        break;
    }
    }
    x_off = X_COORD / 2;
    y_off = Y_COORD / 2;
    glutSetWindow(mainWindow);
    glutPostRedisplay();

}
void display() {
    //Функция вывода всего в окно и управления всем этим
    glClear(GL_COLOR_BUFFER_BIT);

    glutSpecialFunc(movecamera);
    glutKeyboardFunc(scalecamera);
    drawgrid(0.3, 1);
    drawfunc();

    glutSwapBuffers();
    glFlush();
}
void StringHandler()
{
    //Ищем в строке названия функций и заменяем их на символы, которые обозначают эти функции
    for (map<string, string>::iterator it = slovar.begin(); it != slovar.end(); ++it) {
        size_t pos = 0;
        while ((pos = formula.find(it->first, pos)) != string::npos)
        {
            formula.replace(pos, it->first.size(), it->second);
            pos += it->second.size();
        }
    }
    /*cout << formula << endl;*/
    //Для дебага
}
void ReadFormulasStreams()
{
    //Читаем из текстового файла формулы, которые мы запомнили
    ifstream f;
    f.open(path);
    string formulastream;
    while (!f.eof())
    {
        f >> formulastream;
        formulas.push_back(formulastream);
    }
}
void ListOfFormulas()
{
    //Выводим историю формул на экран
    int size = formulas.size();
    for (int i = 0; i < size-1; i++)
    {
        cout << i + 1 << ". " << formulas[i]<<endl;
    }
}
void WriteFormulaToFile(string formulastream)
{
    //Записываем формулу в файл
    ofstream f;
    f.open(path, ofstream::app);
    f << formulastream;
    f << "\n";
    f.close();
}
void WriteInfoTextOnScreen()
{
    cout << "Список поддерживаемых формул:\n\n Квадратный корень - sqrt()\n Синус - sin()\n Косинус - cos()\n Тангенс - tg()\n Модуль числа - abs()\n Арксинус - arcsin()\n";
    cout << " Арккосинус - arccos()\n Арктангенс - arctan()\n Шинус - sh()\n Кошинус - ch()\n Гиперболический тангенс - th()\n Аркшинус - arcsh()\n Арккошинус - arcch()\n Арк-гиперболический тангенс - arcth()\n";
    cout << " Натуральный логарифм - ln()\n Десятичный логарифм - log10()\n Двоичный логарифм - log2()\n Экспонента в степени (степень в скобках) - e()\n\n\n";
}
void Menu()
{
    cout << "1. История формул" << endl;
    cout << "2. Ввод формулы" << endl;
    cout << "0. Выйти из программы" << endl << "  >>";
    char choice;
    scanf_s("%c", &choice);
    switch (choice)
    {
        case '1':
        {
            system("cls");
            ListOfFormulas();
            cout << "Выбор формулы: ";
            int number;
            cin >> number;
            formula = formulas[number - 1];
            break;
        }
        case '2':
        {
            system("cls");
            WriteInfoTextOnScreen();
            cout << "Введите формулу: ";
            cin >> formula;
            WriteFormulaToFile(formula);
            break;
        }
        case '0':
        {
            system("cls");
            exit(3);
            break;
        }
        break;
    }
}
int main(int argc, char** argv) {
    // Для мапы
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
    // Для мапы

    ReadFormulasStreams();
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Menu();
    StringHandler();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 100);
    mainWindow = glutCreateWindow("Графики");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, X_COORD, 0.0, Y_COORD, -1.0, 1.0);
    glutDisplayFunc(display);
    glutMainLoop();
}