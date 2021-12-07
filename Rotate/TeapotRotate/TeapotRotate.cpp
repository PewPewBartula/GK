//Plik: TeapotRotate.cpp
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

//Definicja zmiennej przechowującej współrzędne (x,y,z)
typedef float point3[3];

//Inicjalizacja współrzędnych położenia obserwatora
static GLfloat viewer[] = { 0.0f, 0.0f, 10.0f };

//Kąt obrotu obiektu
static GLfloat theta[3] = { 0.0f, 0.0f, 0.0f };

//Przelicznik pikseli na stopnie
static GLfloat pix2anglex, pix2angley;

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

//Funkcja sprawdza stan myszy i ustawia wartości zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //Przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
        x_pos_old = x;

        //Wciśnięty został lewy klawisz myszy
        status = 1;
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //Przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
        y_pos_old = y;

        //Wciśnięty został prawy klawisz myszy
        status = 2;
    }
    //Nie został wciśnięty żaden klawisz
    else status = 0;
}

//Funkcja sprawdza położenie kursora myszy i ustawia wartości zmiennych globalnych
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

    // Zdefiniowanie położenia obserwatora
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Jeśli lewy klawisz myszy wciśęnięty
    if (status == 1)
    {
        // Modyfikacja kąta obrotu o kat proporcjonalny
	// Do różnicy położeń kursora myszy
        theta[0] += delta_x * pix2anglex;
        theta[1] += delta_y * pix2angley;
    }

    // Jeśli prawy klawisz myszy wciśęnięty
    if (status == 2)
    {
        //Modyfikacja współrzędnej obserwatora 
        viewer[2] += delta_y * pix2angley;
        // Granica przybliżenia i oddalenia
        if (viewer[2] < 1.0f) viewer[2] = 1.01f;
        if (viewer[2] > 30.0f) viewer[2] = 29.99f;
    }

    //Obrót obiektu o nowy kąt
    glRotatef(theta[0], 0.0f, 1.0f, 0.0f);

    glRotatef(theta[1], 1.0f, 0.0f, 0.0f);

    // Ustawienie koloru rysowania na biały
    glColor3f(1.0f, 1.0f, 1.0f);

    // Narysowanie czajnika
    glutWireTeapot(3.0f);

    // Przekazanie poleceń rysujących do wykonania
    glutSwapBuffers();
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
    cout << "W celu modyfiakcji polozenia czajnika nalezy uzyc: " << endl;
    cout << "LPM + ruch myszy - Obracanie obiektu" << endl;
    cout << "PPM + ruch myszy - Przyblizanie obiektu" << endl;
}

// Główny punkt wejścia programu. 
void main(void)
{
    //Wyświetlenie informacji o programie w oknie konsolowym
    info();
    //Funkcja ustawia tryby wyświetlania:
    // GLUT_DOUBLE - używanie podwójnego buforu przestrzeni renderowania,
    // GLUT_RGB - używanie kolorów w konwencji RGB
    // GLUT_DEPTH - użycie buforu do stworzenia kontroli głębi
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

    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania
    MyInit();

    // Włączenie mechanizmu usuwania niewidocznych elementów sceny
    glEnable(GL_DEPTH_TEST);

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();
}
