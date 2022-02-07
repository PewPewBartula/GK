//Plik: EggLight1.cpp
//Autor: Bartosz Szymczak, 252734
//Laboratorium Grafiki komputerowej i komunikacji człowiek - komputer
//Na podstawie http://www.zsk.ict.pwr.wroc.pl/zsk/dyd/intinz/gk/lab/cw_5_dz/
//Grudzień 2021r.

#define _USE_MATH_DEFINES
#include <windows.h>
#include <iostream>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
using namespace std;

//Definicja oraz inicjalizacja zmiennej przechowującej ilość przedziałów 
//boku kwadratu jednostkowego dziedziny parametrycznej
static int n = 100;

//Definicja zmiennej przechowującej współrzędne (x,y,z)
typedef float point3[3];

//Macierze zmiennych point3
point3** points;
point3** normals;

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

//Promień sfery powierzchni, na której znajduje się obserwator
GLfloat r = 15.0f;

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
    return r * sinf(elevation * M_PI / 180);
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

//Metoda generuje wektory normalne 
void generateNormals() {
    float xu, xv, yu, yv = 0, zu, zv, uf, vf;
    float x, y, z, length;
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            uf = (float)u / ((float)n - 1);
            vf = (float)v / ((float)n - 1);
            //Obliczenie wartości wektorów na podstawie gotowych wzorów
            xu = (-450 * pow(uf, 4) + 900 * pow(uf, 3) - 810 * pow(uf, 2) + (360 * uf) - 45) * cos(M_PI * vf);
            xv = M_PI * (90 * pow(uf, 5) - 225 * pow(uf, 4) + 270 * pow(uf, 3) - 180 * pow(uf, 2) + (45 * uf)) * sin(M_PI * vf);
            yu = 640 * pow(uf, 3) - 960 * pow(uf, 2) + 320 * uf;
            zu = (-450 * pow(uf, 4) + 900 * pow(uf, 3) - 810 * pow(uf, 2) + (360 * uf) - 45) * sin(M_PI * vf);
            zv = -M_PI * (90 * pow(uf, 5) - 225 * pow(uf, 4) + 270 * pow(uf, 3) - 180 * pow(uf, 2) + (45 * uf)) * cos(M_PI * vf);
            x = yu * zv - zu * yv;
            y = zu * xv - xu * zv;
            z = xu * yv - yu * xv;
            //Obliczenie długości wektora
            length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            //Normalizacja wektorów dla pierwszej połowy jajka 
            if (u < n / 2)
            {
                normals[u][v][0] = x / length;
                normals[u][v][1] = y / length;
                normals[u][v][2] = z / length;

            }
            //Normalizacja wektorów dla drugiej połowy jajka 
            else if (u > n / 2)
            {
                normals[u][v][0] = -1 *x / length;
                normals[u][v][1] = -1 * y / length;
                normals[u][v][2] = -1 * z / length;
            }
            else if (u == 0 || u == n)
            {
                normals[u][v][0] = 0;
                normals[u][v][1] = -1;
                normals[u][v][2] = 0;
            }
            else
            {
                normals[u][v][0] = 0;
                normals[u][v][1] = 1;
                normals[u][v][2] = 0;
            }
        }
    }
}

//Funkcja rysująca jajko w podany sposób na podstawie wygenerowanych punktów
void Egg()
{
    //Generacja punktów
    eggPoints();
    //Generacja wektorów znormalizowanych
    generateNormals();
    //Model jajka stworzony z punktów połączonych w trójkąty
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

            glNormal3fv(normals[u][v + 1 - n * vdiff]);
            glVertex3fv(points[u][(v + 1 - n * vdiff)]);

            glNormal3fv(normals[u + 1 - n * udiff][v]);
            glVertex3fv(points[(u + 1 - n * udiff)][v]);

            glNormal3fv(normals[u][v]);
            glVertex3fv(points[u][v]);

            glEnd();

            glBegin(GL_TRIANGLES);

            glNormal3fv(normals[u][v + 1 - n * vdiff]);
            glVertex3fv(points[u][v + 1 - n * vdiff]);

            glNormal3fv(normals[u + 1 - n * udiff][v]);
            glVertex3fv(points[(u + 1 - n * udiff)][v]);

            glNormal3fv(normals[u + 1 - n * udiff][v + 1 - n * vdiff]);
            glVertex3fv(points[(u + 1 - n * udiff)][(v + 1 - n * vdiff)]);

            glEnd();
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
    //Modyfikacja położenia obserwatora
    if (key == 'w')
    {
        elevation++;
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }
    if (key == 's')
    {
        elevation--;
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }
    if (key == 'a')
    {
        azimuth--;
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }
    if (key == 'd')
    {
        azimuth++;
        viewer[0] = getX(r, azimuth, elevation);
        viewer[1] = getY(r, azimuth, elevation);
        viewer[2] = getZ(r, azimuth, elevation);
    }
    // przerysowanie obrazu sceny
    RenderScene();
}

// Funkcja ustalająca stan renderowania
void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Definicja materiału z jakiego zrobione jest jajko
    //Współczynniki dla światła otoczenia
    GLfloat mat_ambient[] = { 1.0,1.0, 1.0, 1 };
    //Współczynniki dla światła rozproszonego
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    //Współczynniki dla świata odbitego
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    //Współczynnik opisujacy połysk
    GLfloat mat_shininess = { 100.0 };
    //Definicja źródła światła
    //Położenie źródła światła
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
    //Współczynniki intensywności światła otoczenia
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    //Współczynniki intensywności światła powodującego odbicie dyfuzyjne
    GLfloat light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    //Współczynniki intensywności światła powodującego odbicie kierunkowe
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    //Definicja stałej zmian oświetlenia w funkcji
    GLfloat att_constant = { 1.0 };
    //Definicja zmiennej liniowej zmian oświetlenia w funkcji 
    GLfloat att_linear = { (GLfloat)0.05 };
    //Definicja zmiennej kwadratowej zmian oświetlenia w funkcji 
    GLfloat att_quadratic = { (GLfloat)0.001 };

    //Ustawienie parametrów materiału
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    //Ustawienie parametrów źródła
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

    //Ustawienie opcji systemu wyświetlania sceny
    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora
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
    cout << "W celu modyfiakcji polozenia obserwatora nalezy uzyc: " << endl;
    cout << "'w' i 's' - ruch wzdluz osi x" << endl;
    cout << "'a' i 'd' - ruch wzdluz osi y" << endl;
}

// Główny punkt wejścia programu. 
void main(void)
{
    info();
    //Wygenerowanie macierzy, przechowującej współrzędne punktów jajka
    points = new point3 * [n];
    normals = new point3 * [n];
    for (int i = 0; i < n; ++i)
    {
        points[i] = new point3[n];
        normals[i] = new point3[n];
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
