#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <time.h>
#include <string>

#define PI 3.14159

// Definisanje globalne promenljive
GLfloat hunter_x = 0;
// Definisanje tipa tacka
typedef struct Point
{
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
} Point;
// Definisanje linije odbrane
struct defence
{
    GLfloat g, x, y;
    defence() {}
    defence(GLfloat _x, GLfloat _y, GLfloat _g) : x(_x), y(_y), g(_g) {}
};
// Definisanje neprijatelja (Invejdera)
struct enemy
{
    GLfloat x, y;
    int type;
    enemy() {}
    enemy(GLfloat _x, GLfloat _y, int _type) : x(_x), y(_y), type(_type) {}
};
// Definisanje rakete
struct Missle
{
    float x, y;
    bool moving;
};

bool startInitializationFlag = false; // fleg za pocetak
bool gameOver = false; // fleg za kraj igre
bool flash = false; // fleg za fles ekrana
bool forward = true; 
int score = 0; 
int lives = 3;
// Definisanje vektora
std::vector<struct defence> block;      
std::vector<struct enemy> invaders; 
std::vector<struct Missle> Missle;
std::vector<struct enemy> hit;
std::vector<Point> explosion;
// Definisanje pocetnih pozicija odbrane, invejdera i lovca
void startInitialization()
{
    struct defence temp;
    struct Missle temp2;
    srand(time(NULL));
    for (int i = 0, k = -24; i < 24; i++, k += 2)
    {
        temp = defence(k, 2, 1.0f);

        block.push_back(temp);
        temp.y = 2;
        block.push_back(temp);
        temp.x = k + 1;
        block.push_back(temp);
    }
    for (int i = 0; i < 5; i++)
    {
        for (float j = 0, k = -13; j < 10; j++, k += 3)
        {

            invaders.push_back(enemy(k, 25.5 - i * 2.5, i + 1));
        }
    }
    for (int i = 0; i < 2; i++)
    {

        temp2.moving = false;
        temp2.y = -1;
        temp2.x = hunter_x - 1 + 2 * i;
        Missle.push_back(temp2);
    }
}
// Pomeranje invejdera
void moveInvader(bool *forward)
{
    bool overflow = false; //definisanje flega za overflow
    float step = 0.01;

    for (int i = 0; i < invaders.size(); i++)
    {
        if (invaders[i].y < 2)
        {
            //ako su napadaci preblizu, igra se zavrsava
            gameOver = true;
            return;
        }
        if (invaders[i].x + step > 18 && *forward == true)
        {
            overflow = true;
            break;
        }
        if (invaders[i].x - step < -20 && *forward == false)
        {
            overflow = true;
            break;
        }
    }
    if (overflow)
    { 
        *forward = !*forward;
        for (int i = 0; i < invaders.size(); i++)
        {
            invaders[i].y -= 0.5;
        }
    }
    step = (*forward == true) ? step : -step;

    for (int i = 0; i < invaders.size(); i++)
    {
        invaders[i].x += step;
    }
}
// Definisanje ispisivanja teksta na ekranu
void drawText(const char *text, int length, float x, float y, float size, float r, float g, float b)
{
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, 0.0f);
    glScalef(size, size, 1.0f);

    for (int i = 0; i < length; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)text[i]);
    }
    glPopMatrix();
}
// Pomeranje neprijateljskih raketa
void moveHit()
{
    for (int i = 0; i < hit.size(); i++)
    {
        hit[i].y -= 0.1; // brzina raketa invejdera
    }
}
// Pomeranje raketa lovca
void moveMissle()
{
    for (int i = 0; i < Missle.size(); i++)
    {
        if (Missle[i].moving == true)
        {
            Missle[i].y += 0.5; // brzina rakete lovca
        }
    }
}
// Crtanje lovca
void Hunter()
{
    glPushMatrix();
    glTranslatef(hunter_x, 0, 0);
    glColor3f(0.5f, 0.6f, 0.86f);
    glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0f, -1.0f); //krilo
    glVertex2f(1.0f, -1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f); // malo prednje krilo
    glVertex2f(0.5f, -0.1f);
    glVertex2f(-0.5f, -0.1f);
    glVertex2f(0.0f, 1.0f); // vrh lovca
    glVertex2f(0.2f, 0.5f);
    glVertex2f(-0.2f, 0.5f);
    glVertex2f(0.0f, -1.4f); // rep lovca
    glVertex2f(0.2f, -1.0f);
    glVertex2f(-0.2f, -1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.1f, -1.5f);
    glVertex2f(-0.1f, -1.5f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2f(0.0f, -1.9f); // plamen na repu lovca
    glEnd();

    glColor3f(0.5f, 0.6f, 0.86f);
    glBegin(GL_QUADS);
    glVertex2f(0.2f, -0.2f); // sredina
    glVertex2f(0.2f, 0.5f);
    glVertex2f(-0.2f, 0.5f);
    glVertex2f(-0.2f, -0.2f);
    glEnd();
    glPopMatrix();
}
// Crtanje prvog invejdera
void invader_1()
{
    glColor3f(0.6f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();

    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 1.2); 
    glVertex2f(0, 2);
    glVertex2f(1, 2);
    glVertex2f(2, 1.2); 
    glVertex2f(2, 2);
    glVertex2f(1, 2);
    glVertex2f(0, 1);
    glVertex2f(0, 0);
    glVertex2f(0.8, 1);
    glVertex2f(2, 1); 
    glVertex2f(2, 0);
    glVertex2f(1.2, 1);
    glVertex2f(0, 0); 
    glVertex2f(2, 0);
    glVertex2f(1, 0.9);
    glEnd();
    // praznina za oci
    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_QUADS);  
    glVertex2f(0.5, 1.3);        
    glVertex2f(0.5, 1.5);
    glVertex2f(0.7, 1.5);
    glVertex2f(0.7, 1.3);
    glVertex2f(1.3, 1.3); 
    glVertex2f(1.3, 1.5);
    glVertex2f(1.5, 1.5);
    glVertex2f(1.5, 1.3);
    glEnd();
}
// Crtanje drugog invejdera
void invader_2()
{
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();

    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0 - 0.2, 2);
    glVertex2f(2.2, 2);
    glVertex2f(1, 1.4);
    glVertex2f(0, 0.9);
    glVertex2f(0, 1.7);
    glVertex2f(0.6, 1.4);
    glVertex2f(2, 0.9); 
    glVertex2f(2, 1.7);
    glVertex2f(1.4, 1.4);
    glVertex2f(0.2, 0); 
    glVertex2f(0.2, 0.7);
    glVertex2f(0.6, 0);
    glVertex2f(1.8, 0);
    glVertex2f(1.8, 0.7);
    glVertex2f(1.4, 0);
    glVertex2f(0.5, 0.5); 
    glVertex2f(1.4, 0.5);
    glVertex2f(1, 0 - 0.3);
    glEnd();
    // praznina za oci
    glBegin(GL_QUADS);    
    glVertex2f(0.5, 0.7); 
    glVertex2f(0.5, 0.9);
    glVertex2f(0.7, 0.9);
    glVertex2f(0.7, 0.7);
    glVertex2f(1.3, 0.7); 
    glVertex2f(1.3, 0.9);
    glVertex2f(1.5, 0.9);
    glVertex2f(1.5, 0.7);
    glEnd();
}
// Crtanjetreceg invejdera
void invader_3()
{
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();

    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(2, 0);
    glVertex2f(1, 0.9);
    glVertex2f(0, 1.5); 
    glVertex2f(0, 2);
    glVertex2f(0.6, 2);
    glVertex2f(2, 1.5);
    glVertex2f(2, 2);
    glVertex2f(1.4, 2);
    glVertex2f(0, 1);
    glVertex2f(0, 0.2);
    glVertex2f(0.8, 1);
    glVertex2f(2, 1); 
    glVertex2f(2, 0.2);
    glVertex2f(1.2, 1);
    glEnd();
    // praznina za oci
    glBegin(GL_QUADS);    
    glVertex2f(0.5, 1.3); 
    glVertex2f(0.5, 1.5);
    glVertex2f(0.8, 1.5);
    glVertex2f(0.8, 1.3);
    glVertex2f(1.2, 1.3); 
    glVertex2f(1.2, 1.5);
    glVertex2f(1.5, 1.5);
    glVertex2f(1.5, 1.3);
    glEnd();
}
// Pozivanje invejdera koji ce biti iscrtani na ekranu
void Invaders(struct enemy inv)
{
    glPushMatrix();
    glTranslatef(inv.x, inv.y, 0);
    if (inv.type >= 4)
    {
        invader_3();
    }
    else if (inv.type >= 2)
    {
        invader_2();
    }
    else
    {
        invader_1();
    }
    glPopMatrix();
}
// Crtanje linije odbrane
void drawBlock(struct defence block)
{
    glPushMatrix();
    glTranslatef(block.x, block.y, 0);
    glColor3f(0.0f, block.g, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();
    glPopMatrix();
}
// Funkcija za crtanje eksplozije i njen efekat
void drawExplosion(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    int konstanta = 20;
    GLfloat dvaPi = 2.0f * PI;
    glLineWidth(1);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    glColor3f(1.0f, 1.0f, 0.0f);
    for (int i = 0; i <= konstanta; i++)
    {
        glVertex2f((cos(i * dvaPi / konstanta)), sin(i * dvaPi / konstanta));
    }
    glEnd();
    glPopMatrix();
}
void flashScreen()
{
    glClearColor(0.6, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0.5, 1, 1);
}
// Crtanje rakete invejdera
void drawHit(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(1.0f, 0.0f, 0.1f);
    glLineWidth(2);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(0.1f, -0.2f);
    glVertex2f(0.1f, 0.2f);
    glVertex2f(-0.1f, 0.2f);
    glEnd();
    glPopMatrix();
}
// Crtanje rakete lovca
void drawMissle(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2);
    glBegin(GL_POLYGON);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(-0.1f, 0.1f);
    glVertex2f(0.0f, 0.2f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(0.1f, -0.2f);
    glEnd();
    glPopMatrix();
}
// Funkcija za resetovanje raketa kada raketa pogodi cilj
void resetMissle(int m)
{
    Missle[m].y = -1;
    Missle[m].moving = false;
    Missle[m].x = hunter_x - 1 + 2 * m;
}
// Funkcija za pogodak invejdera, kada je pogodjen, invejder nestaje
void hitInvader(int invader)
{
    hit.push_back(invaders[invader]);
}
// Nasumično se bira invejder svake 4 sekunde za ispaljivanje rakete
// poziva brže sa vremenom dok ne dostigne 1 u sekundi
void chooseInvader(int move)
{
    if (invaders.size() < hit.size())
        return;
    int invader = rand() % invaders.size();
    hitInvader(invader);

    glutTimerFunc((4000 - move > 1000 ? 4000 - move : 1000), chooseInvader, move * 1.3);
}
// provera okvira oko invejdera
bool checkBoundingBox(Point p0, Point p1, float size)
{
    return !(p0.x < p1.x || p0.x > (p1.x + size) || p0.y < p1.y || p0.y > (p1.y + size));
}
void Collider(int move)
{
    // provera da li su rakete pogodile liniju odbrane
    for (int i = 0; i < hit.size(); i++)
    {
        if (hit[i].y <= 4)
        { //udaljenost na kojoj se nalaze odbrane
            for (int j = 0; j < block.size(); j++)
            {
                if (checkBoundingBox(Point(hit[i].x, hit[i].y), Point(block[j].x, block[j].y), 1))
                {
                    explosion.push_back(Point(hit[i].x, hit[i].y));
                    hit.erase(hit.begin() + i);
                    block[j].g -= 0.25f;
                    if (block[i].g <= 0)
                    {
                        block.erase(block.begin() + i);
                    }
                    break;
                }
            }
        }
        if (checkBoundingBox(Point(hit[i].x, hit[i].y), Point(hunter_x - 1, -1), 2))
        { // udaljenost lovca
            lives--;
            flash = true;
            explosion.push_back(Point(hit[i].x, hit[i].y));
            gameOver = (lives > 0) ? false : true;
            hit.erase(hit.begin() + i);
        }
        if (hit[i].y <= -2)
        {
            hit.erase(hit.begin() + i);
        }
    }
    for (int j = 0; j < Missle.size(); j++)
    {
        if (Missle[j].y > 30)
        {
            resetMissle(j);
        }
        for (int i = 0; i < block.size(); i++)
        {
            if (checkBoundingBox(Point(Missle[j].x, Missle[j].y), Point(block[i].x, block[i].y), 1))
            {
                block[i].g -= 0.25f;
                explosion.push_back(Point(Missle[j].x, Missle[j].y));
                resetMissle(j);
                //printf("Pogodak!");

                if (block[i].g <= 0)
                {
                    block.erase(block.begin() + i);
                }
            }
        }
        // proverava da li je raketa pogodila invejdera
        for (int i = 0; i < invaders.size(); i++)
        {
            if (checkBoundingBox(Point(Missle[j].x, Missle[j].y), Point(invaders[i].x, invaders[i].y), 2))
            {
                explosion.push_back(Point(Missle[j].x, Missle[j].y));
                resetMissle(j);

                score += invaders[i].type > 3 ? 10 : (invaders[i].type > 1 ? 20 : 40);
                invaders.erase(invaders.begin() + i);
            }
        }
    }
    glutTimerFunc(move, Collider, move);
}
void moveAll(int move)
{
    moveInvader(&forward);
    moveHit();
    moveMissle();
    glutPostRedisplay();
    Collider(0);
    glutTimerFunc(move, moveAll, move);
}
// Funkcija za ispisivanje vrednosti rezultata i broja zivota preostalih igracu
void results(void)
{
    // Prelazi na model koordinatnog sistema
    glMatrixMode(GL_MODELVIEW);
    // Inicijalizuje trenutnu matricu transformacije
    glLoadIdentity();
    // cisti prozor za pregled sa bojom
    glClear(GL_COLOR_BUFFER_BIT);
    // ispisivanje rezultata na ekranu
    std::string pts = ("Rezultat ");
    pts += std::to_string(score);
    drawText(pts.c_str(), pts.length(), -19.5, 29.5, 0.008, 1, 1, 1);
    // ispisivanje broja zivota na ekranu
    pts = ("Zivoti ");
    pts += std::to_string(lives);
    drawText(pts.c_str(), pts.length(), -19.5, 28, 0.008, 0.7, 0.7, 1);

    // brisanje linije odbrane
    for (int i = 0; i < block.size(); i++)
    {
        drawBlock(block[i]);
    }
    for (int i = 0; i < invaders.size(); i++)
    {
        Invaders(invaders[i]);
    }
    for (int i = 0; i < hit.size(); i++)
    {
        drawHit(hit[i].x, hit[i].y);
    }
    for (int i = 0; i < Missle.size(); i++)
    {
        drawMissle(Missle[i].x, Missle[i].y);
    }
    if (flash)
    {
        flashScreen(); // funkcija za fles ekrana pri pogotku lovca
        flash = false;
    }
    for (int i = 0; i < explosion.size(); i++)
    {
        drawExplosion(explosion[i].x, explosion[i].y);
    }
    explosion.clear();
    //ako je "pauzirano" ispisuje poruku za kraj igre
    //ako nema više neprijatelja ispisuje poruku za pobedu
    if (gameOver)
        drawText("Kraj igre!", 10, -6, 15, 0.025, 1, 0, 0);
        
    if (invaders.size() == 0)
    {
        drawText("Pobeda!", 8, -6, 15, 0.025, 1, 1, 1);
    }
    Hunter();
    glutSwapBuffers();
}
// Funkcija za promenu velicine prozora
void windowSizeChange(GLsizei w, GLsizei h)
{
    GLsizei width, height;
    if (h == 0)
        h = 1;
    // Ažurirajte promenljive
    width = w;
    height = h;

    // Određuje dimenzije prikaza
    glViewport(0, 0, width, height);

    // Inicijalizuje koordinatni sistem
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Uspostavlja prozor za izbor (levo, desno, dole,
    // gore) držeći proporciju sa prozorom za pregled
    if (width <= height)
    {
        gluOrtho2D(-20.0f, 20.0f, -3.0f * height / width, 31.0f * height / width);
    }
    else
    {
        gluOrtho2D(-20.0f * width / height, 20.0f * width / height, -3.0f, 31.0f);
    }
}
// Funkcija za definisanje specijalnih karaktera unetih sa tastature
void specialKeys(int key, int x, int y)
{
    // printf("%f\n", hunter_x);
    // Pomeranje lovca
    if (key == GLUT_KEY_LEFT && !gameOver)
    {
        hunter_x -= 0.5f;
        if (hunter_x < -19)
            hunter_x = -19;
    }
    if (key == GLUT_KEY_RIGHT && !gameOver)
    {
        hunter_x += 0.5f;
        if (hunter_x > 19)
            hunter_x = 19;
    }
    if (key == GLUT_KEY_UP && Missle[0].moving != true)
    {
        Missle[0].moving = true;
    }
    if (key == GLUT_KEY_UP && Missle[1].moving != true)
    {
        Missle[1].moving = true;
    }
    //glutTimerFunc(10, moveMissle, 20);
    for (int i = 0; i < Missle.size(); i++)
    {
        if (Missle[i].moving == false)
        {
            Missle[i].x = hunter_x - 1 + 2 * i;
        }
    }
    glutPostRedisplay();
}
// Funkcija za definisanje karaktera sa tastature odgovornih za kljucne dogadjaje
void keyboardKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);

    case 'p': 
        gameOver = !gameOver;
        break;
    }
}
// Funkcija odgovorna za inicijalizaciju parametara i promenljivih
void Initialization(void)
{
    // boja pozadine
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    gluOrtho2D(-20, 20, -3, 31);
}
// Main funkcija
int main(int argc, char *argv[])
{
    glutInit(&argc, argv); // inicijalizacija gluta
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glutInitWindowPosition(512, 192);
    glutInitWindowSize(800, 600); // velicinja pocetnog prozora
    glutCreateWindow("GORF clone"); // ime prozora

    if (!startInitializationFlag)
    {
        startInitialization();
        startInitializationFlag = true;
    }
    glutTimerFunc(0, moveAll, 5);
    // inicijalizacija invejdera, pocinju kretanje
    glutTimerFunc(0, chooseInvader, 100);
    // Poziv funkcije za ispisivanje vrednosti
    glutDisplayFunc(results);
    glMatrixMode(GL_PROJECTION);
    // Poziv funkcije za promenu veličine prikaza
    // Poziv funkcije za rukovanje posebnim ključevima
    glutSpecialFunc(specialKeys);
    // Poziv funkcije za rukovanje ASCII ključevima
    glutKeyboardFunc(keyboardKeys);
    Initialization();
    glutReshapeFunc(windowSizeChange);
    glutMainLoop();

    return 0;
}
