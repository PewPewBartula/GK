//Plik: Jajko.cpp
//Autor: Bartosz Szymczak, 252734
//Laboratorium Grafiki komputerowej i komunikacji człowiek - komputer
//Na podstawie http://www.zsk.ict.pwr.wroc.pl/zsk/dyd/intinz/gk/lab/cw_3_dz/
//Listopad 2021r.

#define _USE_MATH_DEFINES
#include <windows.h>
#include <iostream>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
using namespace std;

//Definicja zmiennej przechowującej współrzędne (x,y,z)
typedef float point3[3];

//Definicja zmiennej przechowującej ilość przedziałów 
//boku kwadratu jednostkowego dziedziny parametrycznej
int n;
 
//Definicja i inicjalizacja zmiennej odpowiedzialnej za rodzaj wyświetlania obrazu
int model = 1;

//Macierze zmiennych point3
point3** points;
point3** colors;

//Trzy kąty obrotu, theta[0] odpowiada za obrót wokół osi x, theta [1] wokół osi y, a theta [2] wokół osi z
static GLfloat theta[] = { 20.0, 20.0, 20.0 }; 

//Definicja i inicjalizacja zmiennej przechowującej informację o obracaniu obrazu
//początkowo ustawiona na false
bool spin = false;

//Funkcja zmienia aktualny kąt obrócenia jajka
void spinEgg()
{

    theta[0] -= 0.01;
    if (theta[0] > 360.0) theta[0] -= 360.0;

    theta[1] -= 0.01;
    if (theta[1] > 360.0) theta[1] -= 360.0;

    theta[2] -= 0.01;
    if (theta[2] > 360.0) theta[2] -= 360.0;

    //odświeżenie zawartości aktualnego okna aplikacji 
    glutPostRedisplay(); 
}

//Metoda generujaca punkty reprezentujace strukture jajka 
void eggPoints()
{
    float x, y, z, fu, fv;
    for (int u = 0; u < n; ++u)
    {
        for(int v = 0; v < n; ++v)
        {
            //Wyznaczenie wartości "u" i "v"
            //Z definicji "u" i "v" to liczby z zakresu [0;1]
            fu = (float)u / ((float)n - 1);
            fv = (float)v / ((float)n - 1);
            //Obliczenie odpowiednich wartości współrzędnych punktu na podstawie gotowych wzorów
            x = (-90 * pow(fu, 5) + 225 * pow(fu, 4) - 270 * pow(fu, 3) + 180 * pow(fu, 2) - (45 * fu)) * cos(M_PI * fv);
            y = 160 * pow(fu, 4) - 320 * pow(fu, 3) + 160 * pow(fu, 2);
            z = (-90 * pow(fu, 5) + 225 * pow(fu, 4) - 270 * pow(fu, 3) + 180 * pow(fu, 2) - (45 * fu)) * sin(M_PI * fv);
            //Przypisanie obliczonych wartosci do odpowiednich zmiennych
            points[u][v][0] = x;
            points[u][v][1] = y;
            points[u][v][2] = z;
        }
    }
}

//Funkcja rysująca jajko w podany sposób na podstawie wygenerowanych punktów
void Egg()
{
    //Generacja punktów
    eggPoints();
    //Model jajka stworzony z siatki punktów
    if (model == 1)
    {
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (int u = 0; u < n; ++u)
        {
            for (int v = 0; v < n; ++v)
            {
                glVertex3fv(points[u][v]);
            }
        }
        glEnd();
    }
    //Model jajka stworzony z punktów połączonych w linie
    else if (model == 2)
    {
        //Połaczenie punktów poziomo
        glBegin(GL_LINES);
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                glVertex3fv(points[u][v]);
                if (v != n - 1) {
                    glVertex3fv(points[u][v + 1]);
                }
                else {
                    glVertex3fv(points[u][0]);
                }
            }
        }
        glEnd();
        //Połaczenie punktów pionowo
        glBegin(GL_LINES);
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                glVertex3fv(points[u][v]);
                if (u != n - 1) {
                    glVertex3fv(points[u + 1][v]);
                }
                else {
                    glVertex3fv(points[0][v]);
                }               
            }
        }
        glEnd();
    }
    //Model jajka stworzony z punktów połączonych w trójkąty
    else if (model == 3)
    {
        for (int u = 0; u < n; ++u)
        {
            for (int v = 0; v < n; ++v)
            {
                //Zabezpieczenie przed wyjściem poza tablice    
                int vdiff = 0, udiff = 0;
                if (v == n - 1) {
                    vdiff = 1;
                }
                if (u == n - 1) {
                    udiff = 1;
                }

                glBegin(GL_TRIANGLES);
                glColor3fv(colors[u][v + 1 - n * vdiff]);
                glVertex3fv(points[u][(v + 1 - n * vdiff)]);
                glColor3fv(colors[u + 1 - n * udiff][v]);
                glVertex3fv(points[(u + 1 - n * udiff)][v]);
                glColor3fv(colors[u][v]);
                glVertex3fv(points[u][v]);
                glEnd();

                glBegin(GL_TRIANGLES);
                glColor3fv(colors[u][v + 1 - n * vdiff]);
                glVertex3fv(points[u][v + 1 - n * vdiff]);

                glColor3fv(colors[u + 1 - n * udiff][v]);
                glVertex3fv(points[(u + 1 - n * udiff)][v]);

                glColor3fv(colors[u + 1 - n * udiff][v + 1 - n * vdiff]);
                glVertex3fv(points[(u + 1 - n * udiff)][(v + 1 - n * vdiff)]);
                glEnd();
            }
        }
    }
}

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)
void RenderScene(void)
{

    // Czyszczenie okna aktualnym kolorem czyszczącym
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Czyszczenie macierzy bieżącej
    glLoadIdentity();

    //Kolejne 3 funkcje odpowiedzialne za obracanie modelu
    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    //Funkcja tworzy jajko w okreslony sposób
    Egg();

    // Przekazanie poleceń rysujących do wykonania
    glFlush();

    // Aktualizowanie buforów (zamiana buforu z aktualnymi danymi z buforem wyświetlającym obraz)
    glutSwapBuffers();
}

//Funkcja reagująca na naciśnięty klawisz
void keys(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'w') model = 2;
    if (key == 's') model = 3;

    // przerysowanie obrazu sceny
    RenderScene(); 
}


// Funkcja ustalająca stan renderowania
void MyInit(void)
{
    // Kolor czyszcący ustawiono na czarny
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    GLfloat AspectRatio;
    if (vertical == 0) vertical = 1;
    glViewport(0, 0, horizontal, vertical);
    // Przełączenie macierzy bieżącej na macierz projekcji 
    glMatrixMode(GL_PROJECTION);
    // Czyszcznie macierzy bieżącej            
    glLoadIdentity();
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    if (horizontal <= vertical)
        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
    else
        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
    // Przełączenie macierzy bieżącej na macierz widoku modelu                                   
    glMatrixMode(GL_MODELVIEW);
    // Czyszcenie macierzy bieżącej
    glLoadIdentity();
}

//Funkcja wyswietla menu i pobiera dane od użytkownika w oknie konsolowym
void menu()
{
    cout << "Podaj liczbe calkowita N, bedaca iloscia przedzialow \nboku kwadratu jednostkowego dziedziny parametrycznej (najlepiej z zakresu [0;100]) : " << endl;
    cout << "> ";
    cin >> n;

    cout << "Czy obraz ma sie obracac? (0 - NIE, 1 - TAK)" << endl;
    cout << "> ";
    cin >> spin;

    cout << endl << "W celu zmiany trybu wyswietlania obrazu nalezy nacisnac: " << endl;
    cout << "'p' - siatka punktow" << endl;
    cout << "'w' - siatka punktow polaczona liniami" << endl;
    cout << "'s' - siatka punktow polaczona w trojkaty" << endl;
}

// Główny punkt wejścia programu.
void main(void)
{
    //Wyświetlenie menu w oknie konsolowym
    menu();
    //Wygenerowanie macierzy, przechowującej współrzędne punktów jajka
    points = new point3 * [n];
    for (int i = 0; i < n; ++i)
    {
        points[i] = new point3[n];
    }
    //Wygenerowanie macierzy, przechowującej kolory
    colors = new point3 * [n];
    for (int i = 0; i < n; ++i)
    {
        colors[i] = new point3[n];
        for (int j = 0; j < n; ++j)
        {
            colors[i][j][0] = (float)rand() / RAND_MAX;
            colors[i][j][1] = (float)rand() / RAND_MAX;
            colors[i][j][2] = (float)rand() / RAND_MAX;
        }
    }
    //Funkcja ustawia tryby wyświetlania:
    // GLUT_DOUBLE - używanie podwójnego buforu przestrzeni renderowania,
    // GLUT_RGB - używanie kolorów w konwencji RGB
    // GLUT_DEPTH - użycie buforu do stworzenia głębii (istotny dla grafiki 3D)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //Ustawienie początkowej wielkości okna aplikacji
    glutInitWindowSize(300, 300);
    //Tworzenie okna aplikacji o podanym tytule 
    glutCreateWindow("Jajko");
    //Metoda glutDisplayFunc ustawia funkcję, 
    //która będzie wywoływana w pętli w celu renderowania obrazu
    glutDisplayFunc(RenderScene);
    //Metoda glutReshapeFunc ustawia funkjcę, 
    //która będzie wywoływana w sytuacji zmiany rozmiaru wytworzonego okna
    glutReshapeFunc(ChangeSize);
    //Metoda glutIdleFunc ustawia funkjcę,
    //która będzie wywoływana w pętli w celu modyfikacji obrazu
    if (spin) glutIdleFunc(spinEgg);
    //Metoda glutKeyboardFunc ustawia funkcję,
    //która będzie wywoływana w sytuacji wykrycia wciśnięcia klawisza 
    glutKeyboardFunc(keys);
    //Wywołanie funkcji ustalającej stan renderowania 
    MyInit();
    //Funkcja glEnable włącza aktualizowanie buforu głębii 
    glEnable(GL_DEPTH_TEST);
    //Funkcja glutMainLoop zapętla powyższe procesy
    glutMainLoop();
}
