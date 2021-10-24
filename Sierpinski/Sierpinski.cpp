//Plik: Sierpinski.cpp
//Autor: Bartosz Szymczak, 252734
//Laboratorium Grafiki komputerowej i komunikacji człowiek - komputer
//Na podstawie http://www.zsk.ict.pwr.wroc.pl/zsk/dyd/intinz/gk/lab/cw_2_dz/
//Październik 2021r.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>

//Zmienne globalne odpowiedzialne za poziom rekurencji, stopień perturbacji, 
//zawartość koloru oraz współrzędne
int level;
float pet;
bool col;
typedef float point2[2];

//Metoda generująca losowy kolor
void randomColor()
{
    float f, f1, f2;

    f = (float)rand() / RAND_MAX;
    f1 = (float)rand() / RAND_MAX;
    f2 = (float)rand() / RAND_MAX;

    glColor3f(f, f1, f2);
}

//Funkcja rysująca kwadrat
void drawSquare(float x, float y, float width)
{
    //Perturbacja współrzędnych kwadratu
    float p1;
    //Losowanie perturbacji wspolrzednej z przedzialu [-pet;pet]
    p1 = -pet + (float)rand() / RAND_MAX * 2 * pet;

    //obliczanie narożników kwadratów z uwzględnieniem perturbacji
    point2 topL = { x - width / 2 - p1, y + width / 2 + p1 };
    point2 topR = { x + width / 2 + p1, y + width / 2 + p1 };
    point2 botL = { x - width / 2 - p1, y - width / 2 - p1 };
    point2 botR = { x + width / 2 + p1, y - width / 2 - p1 };

    //Rysowanie kolorowych figur
    if (col == true)
    {
        glBegin(GL_POLYGON);
        //Losowanie nowego koloru
        randomColor();
        glVertex2fv(topL);
        randomColor();
        glVertex2fv(topR);
        randomColor();
        glVertex2fv(botR);
        randomColor();
        glVertex2fv(botL);
        glEnd();
    }
    //Rysowanie jednokolorowych figur
    else
    {
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2fv(topL);
        glVertex2fv(topR);
        glVertex2fv(botR);
        glVertex2fv(botL);
        glEnd();
    }
}
//Funkcja tworzaca dywan Sierpinskiego
void drawCarpet(float x, float y, int width, int level)
{
    //Jeżeli jesteśmy na poziomie wyższym niż 0 należy podzielić kwadrat na kolejne mniejsze części
    if (level > 0) 
    { 
        //Przy rekurencyjnym podziale pomijamy środkowy kwadrat ponieważ jego nie rysujemy
        drawCarpet(x - width / 3, y + width / 3, width / 3, level - 1);
        drawCarpet(x, y + width / 3, width / 3, level - 1);
        drawCarpet(x + width / 3, y + width / 3, width / 3, level - 1);
        drawCarpet(x - width / 3, y, width / 3, level - 1);
        drawCarpet(x + width / 3, y, width / 3, level - 1);
        drawCarpet(x - width / 3, y - width / 3, width / 3, level - 1);
        drawCarpet(x, y - width / 3, width / 3, level - 1);
        drawCarpet(x + width / 3, y - width / 3, width / 3, level - 1);
    }

    //Funkcja rysująca kwadrat
    else drawSquare(x, y, width);
}

// Funkcaja określająca, co ma być rysowane
// (zawsze wywoływana, gdy trzeba przerysować scenę)
void RenderScene(void)
{
    // Czyszczenie okna aktualnym kolorem czyszczącym
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Rysowanie dywanu sierpinskiego
    drawCarpet(0, 0, 81, level);

    // Przekazanie poleceń rysujących do wykonania
    glFlush();
}

// Funkcja ustalająca stan renderowania
void MyInit(void)
{
    // Kolor okna wnętrza okna - ustawiono na szary
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego
void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna

{
    // Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna
    GLfloat AspectRatio;

    //Zabezpieczenie pzred dzieleniem przez 0
    if (vertical == 0) vertical = 1;

    // Ustawienie wielkościokna okna urządzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)
    glViewport(0, 0, horizontal, vertical);

    // Określenie układu współrzędnych obserwatora
    glMatrixMode(GL_PROJECTION);

    // Określenie przestrzeni ograniczającej
    glLoadIdentity();

    // Wyznaczenie współczynnika proporcji okna
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // określenie okna obserwatora.
    // Pozwala to zachować właściwe proporcje rysowanego obiektu
    // Do określenia okna obserwatora służy funkcja glOrtho(...)

    if (horizontal <= vertical) glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

    else glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

    // Określenie układu współrzędnych    
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

//Metoda komunikujaca sie z uzytkownikiem 
void menu()
{
    std::cout << "Podaj poziom dywanu: "<<std::endl;
    std::cin >> level;
    std::cout << "Podaj perturbacje: " << std::endl;
    std::cin >> pet;
    std::cout << "Czy obraz ma byc pokolorowany? ( 0 - brak koloru / 1 - kolor ) " << std::endl;
    std::cin >> col;
}

// Główny punkt wejścia programu. Program działa w trybie konsoli
void main(void)

{
    // Wyswietlenie menu
    menu();

    //inicjalizacja ziarna generatora liczb pseudolosowych
    srand(time(NULL));

    // Ustawienie trybu wyświetlania
    // GLUT_SINGLE - pojedynczy bufor wyświetlania
    // GLUT_RGBA - model kolorów RGB
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    // Utworzenie okna i określenie treści napisu w nagłówku okna
    glutCreateWindow("Dywan Sierpinskiego");

    // Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
    // Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
    // trzeba będzie przerysować okno
    glutDisplayFunc(RenderScene);

    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
    // zmiany rozmiaru okna
    glutReshapeFunc(ChangeSize);

    // Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
    // inicjalizacje konieczneprzed przystąpieniem do renderowania
    MyInit();

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();

}

