#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#ifdef __linux__
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;

/* Generic bar value */
struct Bar
{
    float x;
    float y;
    float w;
    float h;
    float step;
};

/* Generic ball value */
struct Ball
{
    float r;
    float x;
    float y;
    float velocity;
    float xstep;
    float ystep;
};

/* Default bar */
Bar bar = {0, -11, 3, 1, 0.5};
/* Top bar for bounce and scoring */
Bar top_bar = {0, 11, 16, 1, 0};
/* Main ball */
Ball ball = {0.5, 0, 0, 1.0, 0.10, 0.10};

const float bar_maxr = 13; // right
const float bar_maxl = -13; // left

const float ball_maxu = 9.5; // up
const float ball_maxd = -9.5; // down
const float ball_maxr = 15; // right
const float ball_maxl = -15; // left

bool isReachedXMax = false;
bool isReachedYMax = false;
bool isGameEnd = false;
bool isGameOver = false;

/* Start the scene at 30 unit backward from center */
const int zoom = -30;

/* Levels and points boundary */
const int level_a = 3;
const int level_b = 6;
const int level_c = 9;

/* Player configs */
static int player_score = 0;
static int player_lives = 3;
static int level = 1;

/* Text buffer */
char finaltext[100];

static void printscore()
{
    char text[50];
    /* Printing values with formatted text */
    sprintf(text, "Level %d - Score %d - Lives %d", level, player_score, player_lives);
    /* Yellow color */
    glColor3f(1, 1, 0);
    /* Top left corner */
    glRasterPos3f( -15 , 10.75 , zoom);
    /* Print using glutBitmapCharacter() */
    for(int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

static void finalscreen()
{
    /* If game over, show text in red color */
    if(isGameOver)
    {
        glColor3f(1, 0, 0);
    }
    /* Or show text in normal yellow color */
    else if(player_score == level_c)
    {
        glColor3f(1, 1, 0);
    }
    /* Center position */
    glRasterPos3f(-7, 0, zoom);
    for(int i = 0; finaltext[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, finaltext[i]);
    glutSwapBuffers();
}

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* If game is at end, don't render scene, show final screen */
    if(isGameEnd)
    {
        finalscreen();
        return;
    }

    /* Print score in each step */
    printscore();

    // Ball
    glPushMatrix();
    glTranslatef(ball.x, ball.y, zoom);
    glColor3f(0.8, 0, 0.1);
    glutSolidSphere(ball.r, 20, 20);
    glPopMatrix();

    // Bottom bar
    glPushMatrix();
    glTranslatef(bar.x, bar.y, zoom);
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    glVertex2f(bar.w, bar.h);
    glColor3f(0, 1, 0);
    glVertex2f(bar.w, -bar.h);
    glColor3f(0, 0, 1);
    glVertex2f(-bar.w, -bar.h);
    glColor3f(1, 0, 1);
    glVertex2f(-bar.w, bar.h);
    glEnd();
    glPopMatrix();

    // Top bar
    glPushMatrix();
    glTranslatef(top_bar.x, top_bar.y, zoom);
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    glVertex2f(top_bar.w, top_bar.h);
    glColor3f(0, 1, 0);
    glVertex2f(top_bar.w, -top_bar.h);
    glColor3f(0, 0, 1);
    glVertex2f(-top_bar.w, -top_bar.h);
    glColor3f(1, 0, 1);
    glVertex2f(-top_bar.w, top_bar.h);
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

void init(void)
{
    /* Init bg with grey color */
    glClearColor( 0.9, 0.9, 0.9, 1);
    glClearDepth( 1.0 );
    /* 32bit */
    glEnable(GL_DEPTH_TEST);
    /* Smooth rendering */
    glShadeModel(GL_SMOOTH);
    /* Enabling colored objects */
    glEnable(GL_COLOR_MATERIAL);
}

void reshape(int w, int h)
{
    float aspectRatio;
    h = (h == 0) ? 1 : h;
    w = (w == 0) ? 1 : w;
    /* Setting window dimention as viewport */
    glViewport( 0, 0, w, h );
    aspectRatio = (float)w/(float)h;
    /* Projection mode */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* Set perspective */
    gluPerspective(45, aspectRatio, 1.0, 100.0);
    /* Back to modelview mode */
    glMatrixMode(GL_MODELVIEW);
}

void idle()
{
#ifdef _WIN32
    Sleep(100); //Sleep = usleep/1000
#else
    usleep(10000);
#endif

    /* Don't calculate any score if game is at end */

    if(isGameEnd)
        return;

    /* If ball's Y value is equal to maximum_down value
    then check if it touched the bar */
    if(ball.y <= ball_maxd)
    {
        /* Check ball's co-ord is between bar's width */
        if(ball.x<=bar.x-bar.w or ball.x>=bar.x+bar.w) // Bar missing
        {
            /* Reduce life */
            player_lives--;
            /* Start the ball from bar, for user's ease */
            bar.x = ball.x;
            /* Reset bar */
            bar.y = -11;

            /* If player has no life left
            end the game and show black banner */
            if(player_lives==0)
            {
                glClearColor(0, 0, 0, 1);
                isGameEnd = true;
                isGameOver = true;
                strcpy(finaltext, "Game Over! You lost all three balls");
            }
        }
        else // Score
        {
            /* Count score */
            player_score++;

            /* Increase ball's speed by 0.1 */
            ball.velocity += 0.1;

            /* Check score level */
            if(player_score==level_a)
                level++;
            else if(player_score==level_b)
                level++;
            /* If final level reached, end the game with green banner */
            else if(player_score==level_c)
            {
                glClearColor(0, 0.8, 0, 1);
                strcpy(finaltext, "Congratulation! Top score recorded");
                isGameEnd = true;
            }
        }
    }

    /* Ball bounce */

    /* If X value of the ball has touched right area */
    if(ball.x>ball_maxr)
        isReachedXMax=true;
    /* If X value of the ball has touched left area */
    else if(ball.x<ball_maxl)
        isReachedXMax=false;

    /* If Y value of the ball has touched top area */
    if(ball.y>ball_maxu)
        isReachedYMax=true;
    /* If Y value of the ball has touched down area */
    else if(ball.y<ball_maxd)
        isReachedYMax=false;

    /* Move the ball by step and velocity */
    if(isReachedXMax)
        ball.x -= ball.xstep * ball.velocity;
    else
        ball.x += ball.xstep * ball.velocity;
    if(isReachedYMax)
        ball.y -= ball.ystep * ball.velocity;
    else
        ball.y += ball.ystep * ball.velocity;

    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    /* If user press ESC key, quit */
    switch(key)
    {
    case 27:
        exit(1);
    }
}

void specialkey(int key, int x, int y)
{
    /* Move the bar with left right key */
    if(key==GLUT_KEY_LEFT and bar.x > bar_maxl)
    {
        bar.x -= bar.step;
    }
    else if(key==GLUT_KEY_RIGHT and bar.x < bar_maxr)
    {
        bar.x += bar.step;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(640, 480);
    glutCreateWindow("Ping Pong Game");
    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkey);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
