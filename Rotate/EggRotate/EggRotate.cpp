//Plik: EggRotate.cpp
//Autor: Bartosz Szymczak, 252734
//Laboratorium Grafiki komputerowej i komunikacji człowiek - komputer
//Na podstawie http://www.zsk.ict.pwr.wroc.pl/zsk/dyd/intinz/gk/lab/cw_4_dz/
//Listopad 2021r.

#define _USE_MATH_DEFINES
#include <windows.h>
#include <iostream>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
using namespace std;

//Definicja i inicjalizacja zmiennej odpowiedzialnej za rodzaj wyświetlania obrazu
static int model = 1;

//Definicja oraz inicjalizacja zmiennej przechowującej ilość przedziałów 
//boku kwadratu jednostkowego dziedziny parametrycznej
static int n = 20;

//Definicja zmiennej przechowującej współrzędne (x,y,z)
typedef float point3[3];

//Macierze zmiennych point3
point3** points;
point3** colors;

//Kąt obrotu obiektu
static GLfloat theta[3] = { 0.0f, 0.0f, 0.0f };  

//Przelicznik pikseli na stopnie
static GLfloat pix2anglex,pix2angley;     

//Stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty został lewy klawisz myszy
// 2 - naciśnięty został prawy klawisz myszy
static GLint status = 0;       

//Poprzednia pozycja kursora myszy
static int x_pos_old = 0;       

//Różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_x = 0;        

// poprzednia pozycja kursora myszy
static int y_pos_old = 0;       

//Różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_y = 0;

//Promień sfery powierzchni, na której znajduje się obserwator
GLfloat r = 10.0f;

//Azymut, na której znajduje się obserwator
GLfloat azimuth = 0.0f;

//Elewacja, na której znajduje się obserwator
GLfloat elevation = 0.0f;

//Inicjalizacja współrzędncyh położenia obserwatora
static GLfloat viewer[] = { 0.0f, 0.0f, r };

//Funkcja zwraca współrzędną x, wiążącą położenie obserwatora z 
//azymutem, kątem elewacji i promieniem sfery powierzchni, na której znajduje się obserwator
GLfloat getX(GLfloat r, GLfloat azimuth, GLfloat elevation)
{
    return r * cosf(azimuth * M_PI / 180) * cosf(elevation * M_PI / 180);
}

//Funkcja zwraca współrzędną y, wiążącą położenie obserwatora z 
//azymutem, kątem elewacji i promieniem sfery powierzchni, na której znajduje się obserwator
GLfloat getY(GLfloat r, GLfloat azimuth, GLfloat elevation)
{
    return r*sinf(elevation * M_PI / 180);
}

//Funkcja zwraca współrzędną z, wiążącą położenie obserwatora z 
//azymutem, kątem elewacji i promieniem sfery powierzchni, na której znajduje się obserwator
GLfloat getZ(GLfloat r, GLfloat azimuth, GLfloat elevation)
{
    return r * sinf(azimuth * M_PI / 180) * cosf(elevation * M_PI / 180);
}

//Metoda generujaca punkty reprezentujace strukture jajka 
void eggPoints()
{
    float x, y, z, fu, fv;
    for (int u = 0; u < n; ++u)
    {
        for (int v = 0; v < n; ++v)
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

//Funkcja sprawdza stan myszy i ustawia wartości odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //Przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
        x_pos_old = x;         
                             
        //Wcięnięty został lewy klawisz myszy
        status = 1;          
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //Przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
        y_pos_old = y;
        //Wcięnięty został prawy klawisz myszy
        status = 2;
    }
    //Nie został wcięnięty żaden klawisz
    else status = 0;          
}

//Funkcja sprawdza położenie kursora myszy i ustawia wartości odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
    //Obliczenie różnicy położenia kursora myszy
    delta_x = x - x_pos_old;     

    delta_y = y - y_pos_old;

    //Podstawienie bieżącego położenia jako poprzednie
    x_pos_old = x;            

    y_pos_old = y;

    //Przerysowanie obrazu sceny
    glutPostRedisplay();     
}

// Funkcja określająca co ma być rysowane (zawsze wywoływana, gdy trzeba
// przerysować scenę)
void RenderScene(void)
{

    // Czyszczenie buforów koloru i głębokości
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Czyszczenie macierzy bieżącej
    glLoadIdentity();

    // Jeśli lewy klawisz myszy wciśnięty
    if (status == 1)                     
    {
        //Modyfikacja azymutu i elewacji o kąt proporcjonalny 
        //do różnicy położeń kursora myszy
        azimuth += delta_x * pix2anglex * 0.5f;
        elevation += delta_y * pix2angley * 0.5f;

        //Obliczenie współrzędnych obserwatora
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }                                  

    // Jeśli prawy klawisz myszy wciśnięty
    if (status == 2)
    {
        //Modyfikacja promienia o kąt proporcjonalny 
        //do różnicy położeń kursora myszy
        r += delta_y * pix2angley;
        //Ograniczenie przybliżenia i oddalenia
        if (r < 3.0f) r = 3.0f;
        if (r > 20.0f) r = 19.99f;

        //Obliczenie współrzędnych obserwatora
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }

    //Ustalenie nowej pozycji obserwatora
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    //Funkcja tworzy jajko w okreslony sposób
    Egg();

    // Przekazanie poleceń rysujących do wykonania
    glutSwapBuffers();
}

//Funkcja reagująca na naciśnięty klawisz
void keys(unsigned char key, int x, int y)
{
    //Wybor rodzaju rysowania jajka
    if (key == 'p') model = 1;
    if (key == 'w') model = 2;
    if (key == 's') model = 3;

    // przerysowanie obrazu sceny
    RenderScene();
}

// Funkcja ustalająca stan renderowania
void MyInit(void)
{
    // Kolor czyszczący ustawiono na czarny
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych w przypadku zmiany rozmiarów okna.
//Parametry vertical i horizontal (wysokość i szerokość okna) są przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // przeliczenie pikseli na stopnie
    pix2anglex = 360.0 / (float)horizontal;  
    pix2angley = 360.0 / (float)vertical;

    // Przełączenie macierzy bieżącej na macierz projekcji
    glMatrixMode(GL_PROJECTION);

    // Czyszcznie macierzy bieżącej
    glLoadIdentity();

    // Ustawienie parametrów dla rzutu perspektywicznego
    gluPerspective(70, 1.0f, 1.0f, 30.0f);

    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

    else
    // Ustawienie wielkości okna okna widoku (viewport) w zależności
    // relacji pomiędzy wysokością i szerokością okna
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

    // Przełączenie macierzy bieżącej na macierz widoku modelu  
    glMatrixMode(GL_MODELVIEW);

    // Czyszczenie macierzy bieżącej
    glLoadIdentity();
}

//Funkcja wyswietla informacje o działaniu programu w oknie konsolowym
void info()
{  
    cout  << "W celu zmiany trybu wyswietlania obrazu jajka nalezy nacisnac: " << endl;
    cout << "'p' - siatka punktow" << endl;
    cout << "'w' - siatka punktow polaczona liniami" << endl;
    cout << "'s' - siatka punktow polaczona w trojkaty" << endl;
    cout << endl << "W celu modyfiakcji polozenia obserwatora nalezy uzyc: " << endl;
    cout << "LPM + ruch myszy - Obracanie obiektu" << endl;
    cout << "PPM + ruch myszy - Przyblizanie obiektu" << endl;
}

// Główny punkt wejścia programu. 
void main(void)
{
    info();
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
    //Naprawienie "szwu" kolorowego jajka
    for (int i = 0; i < n; ++i)
    {
        colors[i][0][0] = colors[n - i - 1][n - 1][0];
        colors[i][0][1] = colors[n - i - 1][n - 1][1];
        colors[i][0][2] = colors[n - i - 1][n - 1][2];
    }

    //Funkcja ustawia tryby wyświetlania:
    // GLUT_DOUBLE - używanie podwójnego buforu przestrzeni renderowania,
    // GLUT_RGB - używanie kolorów w konwencji RGB
    // GLUT_DEPTH - użycie buforu do stworzenia kontroli głębii
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    //Ustawienie początkowej wielkości okna aplikacji
    glutInitWindowSize(300, 300);

    //Tworzenie okna aplikacji o podanym tytule 
    glutCreateWindow("Rzutowanie perspektywiczne");

    // Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy
    glutMouseFunc(Mouse);

    // Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
    glutMotionFunc(Motion);

    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Będzie ona wywoływana za każdym razem
    // gdy zajdzie potrzeba przerysowania okna
    glutDisplayFunc(RenderScene);


    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // za zmiany rozmiaru okna                       
    glutReshapeFunc(ChangeSize);

    //Metoda glutKeyboardFunc ustawia funkcję,
    //która będzie wywoływana w sytuacji wykrycia wciśnięcia klawisza 
    glutKeyboardFunc(keys);


    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania
    MyInit();

    // Włączenie mechanizmu usuwania niewidocznych elementów sceny
    glEnable(GL_DEPTH_TEST);

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();
}
