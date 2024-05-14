#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

pthread_mutex_t op = PTHREAD_MUTEX_INITIALIZER, in = PTHREAD_MUTEX_INITIALIZER, mutex1 = PTHREAD_MUTEX_INITIALIZER, mutex2 = PTHREAD_MUTEX_INITIALIZER, mutex3 = PTHREAD_MUTEX_INITIALIZER, mutex4 = PTHREAD_MUTEX_INITIALIZER, mutex5 = PTHREAD_MUTEX_INITIALIZER;

#define GOST_P 8
#define TIMER 500
#define PAC_DELAY 8000

typedef struct
{
    float x;
    float y;
    float px;
    float py;
    char mov;
    int mp;

} Pacman;
typedef struct
{
    float x;
    float y;
    float px;
    float py;
    char mov;
    int ck;
    int count;
    int mov1;
    int isblue;
} Ghost;

int score = 0;
Pacman pac;
Ghost g1, g2, g3, g4;
Ghost a1, a2, a3, a4;

int lives = 3;
char map[29][31] = {"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",
                    "hbbbbbbbbbbbbbbhbbbbbbbbbbbbbbh",
                    "hgqpppppppppppbhbpppppppppppqbh",
                    "hbphhhhbphhhbpbhbpbhhhbphhhgpbh",
                    "hbphhhhbphhhbpbhbpbhhhbphhhbpbh",
                    "hbpbbbbbpbbbbpbbbpbbbbbpbbbbpbh",
                    "hbpppppppppppppppppppppppppppbh",
                    "hbphhhhbphhhbpbhbpbhhhbphhhbpbh",
                    "hbpbbbbbpbbbbpbbbpbbbbbpbbbbpbh",
                    "hbpppppppppppppppppppppppppppbh",
                    "hhhhhhhbpbphhhwwwhhhbpbpbhhhhhh",
                    "bbbbbbhbpbphbbbbbbbhbpbpbhbbbbb",
                    "hhhhhhhbpbphbbbbbbbhbpbpbhhhhhh",
                    "tbbbbbbbpbphbbbbbbbhbpbpbbbbbbt",
                    "tbbbbbbbpbphbbbbbbbhbpbpbbbbbbt",
                    "hhhhhhhbpbphhhhhhhhhbpbpbhhhhhh",
                    "bbbbbbhbpbpbbbbbbbbbbpbpbhbbbbb",
                    "hhhhhhhbpbpppppqppppppbpbhhhhhh",
                    "hbbbbbbbpbpbbbbbbbbbbpbpbbbbbbh",
                    "hbppppppppphhhhhhhhhbpppppppgbh",
                    "hbpbpbpbpbphhhhhhhhhbpbpbpbpbbh",
                    "hbpbpbpbpbphhhhhhhhhbpbpbpbpbbh",
                    "hbpbpbpbpbpbbbbbbbbbbpbpbpbpbbh",
                    "hbpppppppppppppppppppppppppppbh",
                    "hbpbphhhhhhbpbpbpbphhhhhhbpbpbh",
                    "hbpbphhhhhhbpbpbpbphhhhhhbpbpbh",
                    "hbpbpbbbbbbbpbpbpbpbbbbbbgpbpbh",
                    "hmpppppppppppppppppppppppppppbh",
                    "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"};

sfRenderWindow *window;
int pck;
sfCircleShape *pacman[GOST_P];
sfRectangleShape *ghost1[GOST_P];
sfRectangleShape *ghost2[GOST_P];
sfRectangleShape *ghost3[GOST_P];
sfRectangleShape *ghost4[GOST_P];
sfRectangleShape *ghost_flash;
sfRectangleShape *pacman_dying[GOST_P];

sfRectangleShape *black;
sfRectangleShape *black1;
sfTexture *texture;
sfRectangleShape *block;
sfCircleShape *circle;
sfCircleShape *circlePower;
sfRectangleShape *wblock;
int open = 1;
int input = 0;
int gmov = 0;
sfEvent event;
int wallck = 0;
int timerBlue = TIMER;
sfText *text;

// write a function to read input from use
void *Input()
{
    while (open == 0)
    {

        if (input == 1)
        {
            if (event.type == sfEvtClosed)
            {
                pthread_mutex_unlock(&op);
                open = 1;
                break;
            }
            else if (event.type == sfEvtKeyPressed)
            {
                if (event.key.code == sfKeyEscape)
                {
                    pthread_mutex_unlock(&op);
                    open = 1;
                    break;
                }
                else if (event.key.code == sfKeyUp && pac.mp == 0)
                {
                    pac.mov = 'u';
                }
                else if (event.key.code == sfKeyDown && pac.mp == 0)
                {
                    pac.mov = 'd';
                }
                else if (event.key.code == sfKeyLeft && pac.mp == 0)
                {
                    pac.mov = 'l';
                }
                else if (event.key.code == sfKeyRight && pac.mp == 0)
                {
                    pac.mov = 'r';
                }
                pthread_mutex_unlock(&mutex1);
                pac.mp = 1;
            }
            pthread_mutex_lock(&in);
            input = 0;
        }
    }
}

void *PacmanMovement()
{
    sfEvent event; // Declare the sfEvent struct

    int i, j;
    {
        pac.px = pac.x;
        pac.py = pac.y;
        i = (int)(pac.y / 20) - 2;
        j = (int)(pac.x / 20) - 1;
        // printf("%d  %d\n", i, j);

        if (pac.mov == 'u')
        {
            if (map[i - 1][j] != 'h' && map[i - 1][j + 1] != 'h')
            {
                pac.y -= 20;
            }
        }
        else if (pac.mov == 'd')
        {
            if (map[i + 1 + 1][j] != 'h' && map[i + 1 + 1][j + 1] != 'h' && map[i + 1 + 1][j] != 'w')
            {
                pac.y += 20;
            }
        }
        else if (pac.mov == 'l')
        {
            if (map[i][j - 1] != 'h' && map[i + 1][j - 1] != 'h')
            {
                pac.x -= 20;
            }
        }
        else if (pac.mov == 'r')
        {
            if (map[i][j + 1 + 1] != 'h' && map[i + 1][j + 1 + 1] != 'h')
            {
                pac.x += 20;
            }
        }
        sleep(0.5);
    }
}

// provide function for collision with ghost in which lives get reduced
void *Collision()
{
    int i, j;
    i = (int)(pac.y / 20) - 2;
    j = (int)(pac.x / 20) - 1;
    if ((pac.x == g1.x && pac.y == g1.y) || (pac.x == g2.x && pac.y == g2.y) || (pac.x == g3.x && pac.y == g3.y) || (pac.x == g4.x && pac.y == g4.y) || (pac.x == g1.x && pac.y == g1.y + 5) || (pac.x == g2.x && pac.y == g2.y + 5) || (pac.x == g3.x && pac.y == g3.y + 5) || (pac.x == g4.x && pac.y == g4.y + 5))
    {
        lives -= 1;
        printf("Lives: %d\n", lives);
        // replace ghost with black1 rectangle
        sfRectangleShape_setPosition(black1, (sfVector2f){g1.px, g1.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g2.px, g2.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g3.px, g3.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g4.px, g4.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        // draw window
        sfRenderWindow_display(window);
        g1.x = a1.x;
        g2.x = a2.x;
        g3.x = a3.x;
        g4.x = a4.x;

        g1.y = a1.y;
        g2.y = a2.y;
        g3.y = a3.y;
        g4.y = a4.y;
    }
}

void *ReverseCollision()
{
    int i, j;
    i = (int)(pac.y / 20) - 2;
    j = (int)(pac.x / 20) - 1;
    if ((pac.x == g1.x && pac.y == g1.y) || (pac.x == g2.x && pac.y == g2.y) || (pac.x == g3.x && pac.y == g3.y) || (pac.x == g4.x && pac.y == g4.y) || (pac.x == g1.x && pac.y == g1.y + 5) || (pac.x == g2.x && pac.y == g2.y + 5) || (pac.x == g3.x && pac.y == g3.y + 5) || (pac.x == g4.x && pac.y == g4.y + 5))
    {
        score += 10;
        printf("Score: %d\n", score);
        // replace ghost with black1 rectangle
        sfRectangleShape_setPosition(black1, (sfVector2f){g1.px, g1.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g2.px, g2.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g3.px, g3.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        sfRectangleShape_setPosition(black1, (sfVector2f){g4.px, g4.py});
        sfRenderWindow_drawRectangleShape(window, black1, NULL);
        // draw window
        sfRenderWindow_display(window);
        g1.x = a1.x;
        g2.x = a2.x;
        g3.x = a3.x;
        g4.x = a4.x;

        g1.y = a1.y;
        g2.y = a2.y;
        g3.y = a3.y;
        g4.y = a4.y;
    }
}

void DisplayText()
{
    // Create strings for displaying lives and score
    char livesStr[20];
    char scoreStr[20];
    sprintf(livesStr, "Lives: %d", lives);
    sprintf(scoreStr, "Score: %d", score);

    // Set text for lives
    sfText_setString(text, livesStr);
    sfText_setCharacterSize(text, 24);
    sfText_setFillColor(text, sfWhite);
    sfText_setPosition(text, (sfVector2f){1, 1});
    sfRenderWindow_drawText(window, text, NULL);

    // Set text for score
    sfText_setString(text, scoreStr);
    sfText_setFillColor(text, sfWhite);
    sfText_setPosition(text, (sfVector2f){100, 1});
    sfRenderWindow_drawText(window, text, NULL);
}
void *Teleport()
{
    if (pac.x == 2 * 20 && pac.y == 15 * 20)
    {
        pac.x = 30 * 20;
        pac.y = 15 * 20;
    }
    else if (pac.x == 30 * 20 && pac.y == 15 * 20)
    {
        pac.x = 2 * 20;
        pac.y = 15 * 20;
    }
}

// provide function for score
void *Score()
{
    int i, j;
    i = (int)(pac.y / 20) - 2;
    j = (int)(pac.x / 20) - 1;
    if (map[i][j] == 'p')
    {
        score += 1;
        map[i][j] = 'b';
    }
    if (map[i + 1][j] == 'p')
    {
        score += 1;
        map[i + 1][j] = 'b';
    }
    if (map[i][j + 1] == 'p')
    {
        score += 1;
        map[i][j + 1] = 'b';
    }
    if (map[i + 1][j + 1] == 'p')
    {
        score += 1;
        map[i + 1][j + 1] = 'b';
    }
    // printf("Score: %d\n", score);
}

void *ScorePower()
{
    int i, j;
    i = (int)(pac.y / 20) - 2;
    j = (int)(pac.x / 20) - 1;

    if (map[i][j] == 'q')
    {
        score += 10;
        map[i][j] = 'b';
        g1.isblue = 1;
        g2.isblue = 1;
        g3.isblue = 1;
        g4.isblue = 1;
        timerBlue = TIMER;
    }
    if (map[i + 1][j] == 'q')
    {
        score += 10;
        map[i + 1][j] = 'b';
        g1.isblue = 1;
        g2.isblue = 1;
        g3.isblue = 1;
        g4.isblue = 1;
        timerBlue = TIMER;
    }
    if (map[i][j + 1] == 'q')
    {
        score += 10;
        map[i][j + 1] = 'b';
        g1.isblue = 1;
        g2.isblue = 1;
        g3.isblue = 1;
        g4.isblue = 1;
        timerBlue = TIMER;
    }
    if (map[i + 1][j + 1] == 'q')
    {
        score += 10;
        map[i + 1][j + 1] = 'b';
        g1.isblue = 1;
        g2.isblue = 1;
        g3.isblue = 1;
        g4.isblue = 1;
        timerBlue = TIMER;
    }

    // printf("Score: %d\n", score);
}

void *DisplayPacman()
{
    // set the position of black rectangle
    sfRectangleShape_setPosition(black, (sfVector2f){pac.px, pac.py});
    sfRenderWindow_drawRectangleShape(window, black, NULL);
    if (pac.mov == 'u')
    {
        if (pck == 0)
        {
            sfCircleShape_setPosition(pacman[0], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[0], NULL);
            pck = 1;
        }
        else
        {
            sfCircleShape_setPosition(pacman[1], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[1], NULL);
            pck = 0;
        }
    }
    else if (pac.mov == 'd')
    {
        if (pck == 0)
        {
            sfCircleShape_setPosition(pacman[2], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[2], NULL);
            pck = 1;
        }
        else
        {
            sfCircleShape_setPosition(pacman[3], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[3], NULL);
            pck = 0;
        }
    }
    else if (pac.mov == 'l')
    {
        if (pck == 0)
        {
            sfCircleShape_setPosition(pacman[4], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[4], NULL);
            pck = 1;
        }
        else
        {
            sfCircleShape_setPosition(pacman[5], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[5], NULL);
            pck = 0;
        }
    }
    else if (pac.mov == 'r')
    {
        if (pck == 0)
        {
            sfCircleShape_setPosition(pacman[6], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[6], NULL);
            pck = 1;
        }
        else
        {
            sfCircleShape_setPosition(pacman[7], (sfVector2f){pac.x, pac.y});
            sfRenderWindow_drawCircleShape(window, pacman[7], NULL);
            pck = 0;
        }
    }
    usleep(PAC_DELAY);
}

void *GhostMovement1()
{
    while (open == 0)
    {
        if (g1.mov1 == 1)
        {
            // printf("\nGhost 1 is moving %d\n", g1.count);

            int i, j;
            g1.px = g1.x;
            g1.py = g1.y;
            i = (int)(g1.y / 20) - 2;
            j = (int)(g1.x / 20) - 1;
            if (g1.mov == 'r')
            {
                if (map[i][j + 1 + 1] != 'h' && map[i + 1][j + 1 + 1] != 'h')
                {
                    g1.x += 10;
                }
                else
                    g1.count = 0;
            }
            else if (g1.mov == 'l')
            {
                if (map[i][j - 1] != 'h' && map[i + 1][j - 1] != 'h')
                {
                    g1.x -= 10;
                }
                else
                    g1.count = 0;
            }
            if (g1.mov == 'u')
            {
                if (map[i - 1][j] != 'h' && map[i - 1][j + 1] != 'h')
                {
                    g1.y -= 10;
                }
                else
                    g1.count = 0;
            }
            else if (g1.mov == 'd')
            {
                if (map[i + 1 + 1][j] != 'h' && map[i + 1 + 1][j + 1] != 'h')
                {
                    g1.y += 10;
                }
                else
                    g1.count = 0;
            }

            if (g1.count == 0)
            {

                int r = rand() % 4;
                if (r == 0)
                    g1.mov = 'u';
                else if (r == 1)
                    g1.mov = 'd';
                else if (r == 2)
                    g1.mov = 'l';
                else if (r == 3)
                    g1.mov = 'r';

                g1.count = 10;
            }

            g1.count--;

            pthread_mutex_lock(&mutex2);
            g1.mov1 = 0;
            usleep(PAC_DELAY + 1000);
        }
    }
}

void *GhostMovement2()
{

    while (open == 0)
    {

        if (g2.mov1 == 1)
        {
            // printf("\nGhost 2 is moving %d\n", g2.count);

            int i, j;
            g2.px = g2.x;
            g2.py = g2.y;
            i = (int)(g2.y / 20) - 2;
            j = (int)(g2.x / 20) - 1;
            if (g2.mov == 'r')
            {
                if (map[i][j + 1 + 1] != 'h' && map[i + 1][j + 1 + 1] != 'h')
                {
                    g2.x += 10;
                }
                else
                    g2.count = 0;
            }
            else if (g2.mov == 'l')
            {
                if (map[i][j - 1] != 'h' && map[i + 1][j - 1] != 'h')
                {
                    g2.x -= 10;
                }
                else
                    g2.count = 0;
            }
            if (g2.mov == 'u')
            {
                if (map[i - 1][j] != 'h' && map[i - 1][j + 1] != 'h')
                {
                    g2.y -= 10;
                }
                else
                    g2.count = 0;
            }
            else if (g2.mov == 'd')
            {
                if (map[i + 1 + 1][j] != 'h' && map[i + 1 + 1][j + 1] != 'h')
                {
                    g2.y += 10;
                }
                else
                    g2.count = 0;
            }

            if (g2.count == 0)
            {

                int r = rand() % 4;
                if (r == 0)
                    g2.mov = 'u';
                else if (r == 1)
                    g2.mov = 'd';
                else if (r == 2)
                    g2.mov = 'l';
                else if (r == 3)
                    g2.mov = 'r';

                g2.count = 10;
            }

            g2.count--;

            pthread_mutex_lock(&mutex3);
            g2.mov1 = 0;
            usleep(40000);
        }
    }
}

void *GhostMovement3()
{

    while (open == 0)
    {

        if (g3.mov1 == 1)
        {
            // printf("\nGhost 3 is moving %d\n", g3.count);

            int i, j;
            g3.px = g3.x;
            g3.py = g3.y;
            i = (int)(g3.y / 20) - 2;
            j = (int)(g3.x / 20) - 1;
            if (g3.mov == 'r')
            {
                if (map[i][j + 1 + 1] != 'h' && map[i + 1][j + 1 + 1] != 'h')
                {
                    g3.x += 10;
                }
                else
                    g3.count = 0;
            }
            else if (g3.mov == 'l')
            {
                if (map[i][j - 1] != 'h' && map[i + 1][j - 1] != 'h')
                {
                    g3.x -= 10;
                }
                else
                    g3.count = 0;
            }
            if (g3.mov == 'u')
            {
                if (map[i - 1][j] != 'h' && map[i - 1][j + 1] != 'h')
                {
                    g3.y -= 10;
                }
                else
                    g3.count = 0;
            }
            else if (g3.mov == 'd')
            {
                if (map[i + 1 + 1][j] != 'h' && map[i + 1 + 1][j + 1] != 'h')
                {
                    g3.y += 10;
                }
                else
                    g3.count = 0;
            }

            if (g3.count == 0)
            {

                int r = rand() % 4;
                if (r == 0)
                    g3.mov = 'u';
                else if (r == 1)
                    g3.mov = 'd';
                else if (r == 2)
                    g3.mov = 'l';
                else if (r == 3)
                    g3.mov = 'r';

                g3.count = 10;
            }

            g3.count--;

            pthread_mutex_lock(&mutex4);
            g3.mov1 = 0;
            usleep(50000);
        }
    }
}

void *GhostMovement4()
{

    while (open == 0)
    {

        if (g4.mov1 == 1)
        {
            // printf("\nGhost 4 is moving %d\n", g4.count);

            int i, j;
            g4.px = g4.x;
            g4.py = g4.y;
            i = (int)(g4.y / 20) - 2;
            j = (int)(g4.x / 20) - 1;
            if (g4.mov == 'r')
            {
                if (map[i][j + 1 + 1] != 'h' && map[i + 1][j + 1 + 1] != 'h')
                {
                    g4.x += 10;
                }
                else
                    g4.count = 0;
            }
            else if (g4.mov == 'l')
            {
                if (map[i][j - 1] != 'h' && map[i + 1][j - 1] != 'h')
                {
                    g4.x -= 10;
                }
                else
                    g4.count = 0;
            }
            if (g4.mov == 'u')
            {
                if (map[i - 1][j] != 'h' && map[i - 1][j + 1] != 'h')
                {
                    g4.y -= 10;
                }
                else
                    g4.count = 0;
            }
            else if (g4.mov == 'd')
            {
                if (map[i + 1 + 1][j] != 'h' && map[i + 1 + 1][j + 1] != 'h')
                {
                    g4.y += 10;
                }
                else
                    g4.count = 0;
            }

            if (g4.count == 0)
            {

                int r = rand() % 4;
                if (r == 0)
                    g4.mov = 'u';
                else if (r == 1)
                    g4.mov = 'd';
                else if (r == 2)
                    g4.mov = 'l';
                else if (r == 3)
                    g4.mov = 'r';

                g4.count = 10;
            }

            g4.count--;

            pthread_mutex_lock(&mutex5);
            g4.mov1 = 0;
            usleep(90000);
        }
    }
}

void *DisplayGhost1()
{
    if (!window)
    {
        printf("Error: Window is not initialized\n");
        return NULL;
    }

    if (!black1 || !ghost1[0] || !ghost1[1] || !ghost1[2] || !ghost1[3] || !ghost1[4] || !ghost1[5] || !ghost1[6] || !ghost1[7])
    {
        printf("Error: Objects are not initialized\n");
        return NULL;
    }

    sfRectangleShape_setPosition(black1, (sfVector2f){g1.px, g1.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);

    if (g1.isblue == 1)
    {
        sfRectangleShape_setPosition(ghost_flash, (sfVector2f){g1.x, g1.y});
        sfRenderWindow_drawRectangleShape(window, ghost_flash, NULL);
    }
    else
    {

        if (g1.mov == 'u')
        {
            if (g1.ck == 0)
            {
                sfRectangleShape_setPosition(ghost1[0], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[0], NULL);
                g1.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost1[1], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[1], NULL);
                g1.ck = 0;
            }
        }
        else if (g1.mov == 'd')
        {
            if (g1.ck == 0)
            {
                sfRectangleShape_setPosition(ghost1[2], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[2], NULL);
                g1.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost1[3], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[3], NULL);
                g1.ck = 0;
            }
        }
        else if (g1.mov == 'l')
        {
            if (g1.ck == 0)
            {
                sfRectangleShape_setPosition(ghost1[4], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[4], NULL);
                g1.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost1[5], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[5], NULL);
                g1.ck = 0;
            }
        }
        else if (g1.mov == 'r')
        {
            if (g1.ck == 0)
            {
                sfRectangleShape_setPosition(ghost1[6], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[6], NULL);
                g1.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost1[7], (sfVector2f){g1.x, g1.y});
                sfRenderWindow_drawRectangleShape(window, ghost1[7], NULL);
                g1.ck = 0;
            }
        }
    }
}

void *DisplayGhost2()
{
    if (!window)
    {
        printf("Error: Window is not initialized\n");
        return NULL;
    }

    if (!black1 || !ghost2[0] || !ghost2[1] || !ghost2[2] || !ghost2[3] || !ghost2[4] || !ghost2[5] || !ghost2[6] || !ghost2[7])
    {
        printf("Error: Objects are not initialized\n");
        return NULL;
    }

    sfRectangleShape_setPosition(black1, (sfVector2f){g2.px, g2.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);

    if (g2.isblue == 1)
    {
        sfRectangleShape_setPosition(ghost_flash, (sfVector2f){g2.x, g2.y});
        sfRenderWindow_drawRectangleShape(window, ghost_flash, NULL);
    }
    else
    {

        if (g2.mov == 'u')
        {
            if (g2.ck == 0)
            {
                sfRectangleShape_setPosition(ghost2[0], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[0], NULL);
                g2.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost2[1], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[1], NULL);
                g2.ck = 0;
            }
        }
        else if (g2.mov == 'd')
        {
            if (g2.ck == 0)
            {
                sfRectangleShape_setPosition(ghost2[2], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[2], NULL);
                g2.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost2[3], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[3], NULL);
                g2.ck = 0;
            }
        }
        else if (g2.mov == 'l')
        {
            if (g2.ck == 0)
            {
                sfRectangleShape_setPosition(ghost2[4], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[4], NULL);
                g2.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost2[5], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[5], NULL);
                g2.ck = 0;
            }
        }
        else if (g2.mov == 'r')
        {
            if (g2.ck == 0)
            {
                sfRectangleShape_setPosition(ghost2[6], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[6], NULL);
                g2.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost2[7], (sfVector2f){g2.x, g2.y});
                sfRenderWindow_drawRectangleShape(window, ghost2[7], NULL);
                g2.ck = 0;
            }
        }
    }
}
void *DisplayGhost3()
{
    if (!window)
    {
        printf("Error: Window is not initialized\n");
        return NULL;
    }

    if (!black1 || !ghost3[0] || !ghost3[1] || !ghost3[2] || !ghost3[3] || !ghost3[4] || !ghost3[5] || !ghost3[6] || !ghost3[7])
    {
        printf("Error: Objects are not initialized\n");
        return NULL;
    }

    sfRectangleShape_setPosition(black1, (sfVector2f){g3.px, g3.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);
    if (g3.isblue == 1)
    {
        sfRectangleShape_setPosition(ghost_flash, (sfVector2f){g3.x, g3.y});
        sfRenderWindow_drawRectangleShape(window, ghost_flash, NULL);
    }
    else
    {
        if (g3.mov == 'u')
        {
            if (g3.ck == 0)
            {
                sfRectangleShape_setPosition(ghost3[0], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[0], NULL);
                g3.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost3[1], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[1], NULL);
                g3.ck = 0;
            }
        }
        else if (g3.mov == 'd')
        {
            if (g3.ck == 0)
            {
                sfRectangleShape_setPosition(ghost3[2], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[2], NULL);
                g3.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost3[3], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[3], NULL);
                g3.ck = 0;
            }
        }
        else if (g3.mov == 'l')
        {
            if (g3.ck == 0)
            {
                sfRectangleShape_setPosition(ghost3[4], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[4], NULL);
                g3.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost3[5], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[5], NULL);
                g3.ck = 0;
            }
        }
        else if (g3.mov == 'r')
        {
            if (g3.ck == 0)
            {
                sfRectangleShape_setPosition(ghost3[6], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[6], NULL);
                g3.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost3[7], (sfVector2f){g3.x, g3.y});
                sfRenderWindow_drawRectangleShape(window, ghost3[7], NULL);
                g3.ck = 0;
            }
        }
    }
}

void *DisplayGhost4()
{
    if (!window)
    {
        printf("Error: Window is not initialized\n");
        return NULL;
    }

    if (!black1 || !ghost4[0] || !ghost4[1] || !ghost4[2] || !ghost4[3] || !ghost4[4] || !ghost4[5] || !ghost4[6] || !ghost4[7])
    {
        printf("Error: Objects are not initialized\n");
        return NULL;
    }

    sfRectangleShape_setPosition(black1, (sfVector2f){g4.px, g4.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);

    if (g4.isblue == 1)
    {
        sfRectangleShape_setPosition(ghost_flash, (sfVector2f){g4.x, g4.y});
        sfRenderWindow_drawRectangleShape(window, ghost_flash, NULL);
    }
    else
    {

        if (g4.mov == 'u')
        {
            if (g4.ck == 0)
            {
                sfRectangleShape_setPosition(ghost4[0], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[0], NULL);
                g4.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost4[1], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[1], NULL);
                g4.ck = 0;
            }
        }
        else if (g4.mov == 'd')
        {
            if (g4.ck == 0)
            {
                sfRectangleShape_setPosition(ghost4[2], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[2], NULL);
                g4.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost4[3], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[3], NULL);
                g4.ck = 0;
            }
        }
        else if (g4.mov == 'l')
        {
            if (g4.ck == 0)
            {
                sfRectangleShape_setPosition(ghost4[4], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[4], NULL);
                g4.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost4[5], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[5], NULL);
                g4.ck = 0;
            }
        }
        else if (g4.mov == 'r')
        {
            if (g4.ck == 0)
            {
                sfRectangleShape_setPosition(ghost4[6], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[6], NULL);
                g4.ck = 1;
            }
            else
            {
                sfRectangleShape_setPosition(ghost4[7], (sfVector2f){g4.x, g4.y});
                sfRenderWindow_drawRectangleShape(window, ghost4[7], NULL);
                g4.ck = 0;
            }
        }
    }
}

void *DisplayMap()
{
    sfEvent event;

    // Draw map elements
    for (int i = 0; i < 31; i++)
    {
        for (int j = 0; j < 29; j++)
        {

            if (wallck == 0 && map[j][i] == 'h')
            {
                sfRectangleShape_setPosition(block, (sfVector2f){((i + 1) * 20.0), ((j + 2) * 20.0)});
                sfRenderWindow_drawRectangleShape(window, block, NULL);
            }
            else if (map[j][i] == 'p')
            {
                sfCircleShape_setPosition(circle, (sfVector2f){((i + 1) * 20.0), ((j + 2) * 20.0)});
                sfRenderWindow_drawCircleShape(window, circle, NULL);
            }
            else if (map[j][i] == 'q')
            {
                sfCircleShape_setPosition(circlePower, (sfVector2f){((i + 1) * 20.0), ((j + 2) * 20.0)});
                sfRenderWindow_drawCircleShape(window, circlePower, NULL);
            }
            else if (map[j][i] == 'w')
            {
                sfRectangleShape_setPosition(wblock, (sfVector2f){((i + 1) * 20.0), ((j + 2) * 20.0)});
                sfRenderWindow_drawRectangleShape(window, wblock, NULL);
            }
        }
    }

    wallck = 1;
    // Display the window
    sfRenderWindow_display(window);
}

void *Initialize()
{


    sfVideoMode mode = {650, 650, 32};
    window = sfRenderWindow_create(mode, "SFML Window", sfResize | sfClose, NULL);
    // Display Splash screen for 3 seconds
    texture = sfTexture_createFromFile("img/splash.jpg", NULL);
    //create object and display it
    sfRectangleShape *intro = sfRectangleShape_create();
    sfRectangleShape_setSize(intro, (sfVector2f){650.f, 650.f});
    sfRectangleShape_setTexture(intro, texture, sfTrue);
    sfRenderWindow_drawRectangleShape(window, intro, NULL);
    sfRenderWindow_display(window);
    sleep(3);
    // clear window
    sfRenderWindow_clear(window, sfBlack);

    // Create rectangles
    block = sfRectangleShape_create();
    sfRectangleShape_setSize(block, (sfVector2f){20.f, 20.f});
    sfRectangleShape_setFillColor(block, sfBlue);
    // Create rectangles
    wblock = sfRectangleShape_create();
    sfRectangleShape_setSize(wblock, (sfVector2f){20.f, 10.f});
    sfRectangleShape_setFillColor(wblock, sfWhite);
    // create black rectangle
    black = sfRectangleShape_create();
    sfRectangleShape_setSize(black, (sfVector2f){40.f, 40.f});
    sfRectangleShape_setFillColor(black, sfBlack);
    // create black rectangle
    black1 = sfRectangleShape_create();
    sfRectangleShape_setSize(black1, (sfVector2f){30.f, 30.f});
    sfRectangleShape_setFillColor(black1, sfBlack);
    // Create Circles
    circle = sfCircleShape_create();
    sfCircleShape_setRadius(circle, 3.0f);
    sfCircleShape_setFillColor(circle, sfGreen);
    // Create Power up Circles
    circlePower = sfCircleShape_create();
    sfCircleShape_setRadius(circlePower, 6.0f);
    sfCircleShape_setFillColor(circlePower, sfWhite);

    // Create circle for pacman

    for (int i = 0; i < GOST_P; i++)
    {
        pacman[i] = sfCircleShape_create();
        sfCircleShape_setRadius(pacman[i], 20.0f);
    }

    // Set image for pacman
    texture = sfTexture_createFromFile("img/pacman-halfopen-u2.png", NULL);
    sfCircleShape_setTexture(pacman[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-open-u2.png", NULL);
    sfCircleShape_setTexture(pacman[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-halfopen-d2.png", NULL);
    sfCircleShape_setTexture(pacman[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-open-d2.png", NULL);
    sfCircleShape_setTexture(pacman[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-halfopen-l2.png", NULL);
    sfCircleShape_setTexture(pacman[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-open-l2.png", NULL);
    sfCircleShape_setTexture(pacman[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-halfopen-r2.png", NULL);
    sfCircleShape_setTexture(pacman[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-open-r2.png", NULL);
    sfCircleShape_setTexture(pacman[7], texture, sfTrue);

    // create circle for ghost like above

    // Create rectangles for ghost1

    for (int i = 0; i < GOST_P; i++)
    {
        ghost1[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ghost1[i], (sfVector2f){30.f, 30.f});
    }
    // Set image texture for ghost1 (blinky)
    texture = sfTexture_createFromFile("img/blinky-up1.png", NULL);
    sfRectangleShape_setTexture(ghost1[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-up2.png", NULL);
    sfRectangleShape_setTexture(ghost1[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-down1.png", NULL);
    sfRectangleShape_setTexture(ghost1[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-down2.png", NULL);
    sfRectangleShape_setTexture(ghost1[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-left1.png", NULL);
    sfRectangleShape_setTexture(ghost1[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-left2.png", NULL);
    sfRectangleShape_setTexture(ghost1[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-right1.png", NULL);
    sfRectangleShape_setTexture(ghost1[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/blinky-right2.png", NULL);
    sfRectangleShape_setTexture(ghost1[7], texture, sfTrue);

    // Create rectangles for ghost2 (clyde)
    for (int i = 0; i < GOST_P; i++)
    {
        ghost2[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ghost2[i], (sfVector2f){30.f, 30.f});
    }
    // Set image texture for ghost2 (clyde)
    texture = sfTexture_createFromFile("img/clyde-up1.png", NULL);
    sfRectangleShape_setTexture(ghost2[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-up2.png", NULL);
    sfRectangleShape_setTexture(ghost2[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-down1.png", NULL);
    sfRectangleShape_setTexture(ghost2[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-down2.png", NULL);
    sfRectangleShape_setTexture(ghost2[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-left1.png", NULL);
    sfRectangleShape_setTexture(ghost2[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-left2.png", NULL);
    sfRectangleShape_setTexture(ghost2[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-right1.png", NULL);
    sfRectangleShape_setTexture(ghost2[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/clyde-right2.png", NULL);
    sfRectangleShape_setTexture(ghost2[7], texture, sfTrue);

    // Create rectangles for ghost3 (inky)

    for (int i = 0; i < GOST_P; i++)
    {
        ghost3[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ghost3[i], (sfVector2f){30.f, 30.f});
    }
    // Set image texture for ghost3 (inky)
    texture = sfTexture_createFromFile("img/inky-up1.png", NULL);
    sfRectangleShape_setTexture(ghost3[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-up2.png", NULL);
    sfRectangleShape_setTexture(ghost3[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-down1.png", NULL);
    sfRectangleShape_setTexture(ghost3[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-down2.png", NULL);
    sfRectangleShape_setTexture(ghost3[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-left1.png", NULL);
    sfRectangleShape_setTexture(ghost3[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-left2.png", NULL);
    sfRectangleShape_setTexture(ghost3[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-right1.png", NULL);
    sfRectangleShape_setTexture(ghost3[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/inky-right2.png", NULL);
    sfRectangleShape_setTexture(ghost3[7], texture, sfTrue);

    // Create rectangles for ghost4 (pinky)

    for (int i = 0; i < GOST_P; i++)
    {
        ghost4[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ghost4[i], (sfVector2f){30.f, 30.f});
    }
    // Set image texture for ghost4 (pinky)
    texture = sfTexture_createFromFile("img/pinky-up1.png", NULL);
    sfRectangleShape_setTexture(ghost4[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-up2.png", NULL);
    sfRectangleShape_setTexture(ghost4[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-down1.png", NULL);
    sfRectangleShape_setTexture(ghost4[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-down2.png", NULL);
    sfRectangleShape_setTexture(ghost4[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-left1.png", NULL);
    sfRectangleShape_setTexture(ghost4[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-left2.png", NULL);
    sfRectangleShape_setTexture(ghost4[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-right1.png", NULL);
    sfRectangleShape_setTexture(ghost4[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pinky-right2.png", NULL);
    sfRectangleShape_setTexture(ghost4[7], texture, sfTrue);

    // Create rectangles for pacman dying
    for (int i = 0; i < GOST_P; i++)
    {
        pacman_dying[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(pacman_dying[i], (sfVector2f){40.0f, 40.0f});
    }
    // Set image texture for pacman dying
    // Set image texture for pacman dying animation frames
    sfTexture *texture;
    texture = sfTexture_createFromFile("img/pacman-dying1.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[0], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying2.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[1], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying3.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[2], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying4.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[3], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying5.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[4], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying6.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[5], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying7.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[6], texture, sfTrue);
    texture = sfTexture_createFromFile("img/pacman-dying8.png", NULL);
    sfRectangleShape_setTexture(pacman_dying[7], texture, sfTrue);

    // Create rectangle for ghost flash
    ghost_flash = sfRectangleShape_create();
    sfRectangleShape_setSize(ghost_flash, (sfVector2f){30.f, 30.f});
    texture = sfTexture_createFromFile("img/flash1.gif", NULL);
    sfRectangleShape_setTexture(ghost_flash, texture, sfTrue);
}
void *Backend()
{
    Initialize();
    while (open == 0 && lives > 0)
    {
        // Process events
        printf("Window is open\n");
        while (sfRenderWindow_pollEvent(window, &event) && input == 0)
        {
            pthread_mutex_unlock(&in);
            input = 1;
        }
        if (input == 1)
            sleep(0.3);
        // Display map

        DisplayPacman();

        DisplayMap();

        // Draw pacman

        // call function for ghost movement and displaying ghost

        if (g1.mov1 == 0)
        {
            DisplayGhost1();
            Collision();
            pthread_mutex_unlock(&mutex2);
            g1.mov1 = 1;
        }

        if (g2.mov1 == 0)
        {
            DisplayGhost2();
            Collision();
            pthread_mutex_unlock(&mutex3);
            g2.mov1 = 1;
        }

        if (g3.mov1 == 0)
        {
            DisplayGhost3();
            Collision();
            pthread_mutex_unlock(&mutex4);
            g3.mov1 = 1;
        }

        if (g4.mov1 == 0)
        {
            DisplayGhost4();
            Collision();
            pthread_mutex_unlock(&mutex5);
            g4.mov1 = 1;
        }

        sfRenderWindow_display(window);
        if (pac.mp == 1)
        {
            PacmanMovement();
            Teleport();
            pthread_mutex_lock(&mutex1);
            pac.mp = 0;
        }

        if (g1.isblue == 0 || g2.isblue == 0 || g3.isblue == 0 || g4.isblue == 0)
        {
            Collision();
        }
        else
        {
            ReverseCollision();
        }
        if (timerBlue == 0)
        {
            g1.isblue = 0;
            g2.isblue = 0;
            g3.isblue = 0;
            g4.isblue = 0;
        }

        if (timerBlue > 0)
            timerBlue--;

        // printf("Reset Blue%d",timerBlue);
        Score();
        ScorePower();
        DisplayText();
    }
    // Cleanup
    // replace all ghost with black1 rectangle
    sfRectangleShape_setPosition(black1, (sfVector2f){g1.px, g1.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);
    sfRectangleShape_setPosition(black1, (sfVector2f){g2.px, g2.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);
    sfRectangleShape_setPosition(black1, (sfVector2f){g3.px, g3.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);
    sfRectangleShape_setPosition(black1, (sfVector2f){g4.px, g4.py});
    sfRenderWindow_drawRectangleShape(window, black1, NULL);

    // replace pacman with black rectangle
    sfRectangleShape_setPosition(black, (sfVector2f){pac.px, pac.py});
    sfRenderWindow_drawRectangleShape(window, black, NULL);
    sfRectangleShape_setPosition(black, (sfVector2f){pac.x, pac.y});
    sfRenderWindow_drawRectangleShape(window, black, NULL);
    sfRenderWindow_display(window);
    for (int i = 0; i < GOST_P; i++)
    {
        // draw dying pacman
        sfRectangleShape_setPosition(pacman_dying[i], (sfVector2f){pac.x, pac.y});
        sfRenderWindow_drawRectangleShape(window, pacman_dying[i], NULL);
        sfRenderWindow_display(window);
        sleep(1);
    }
    // create game over screen
    texture = sfTexture_createFromFile("img/gameover2.png", NULL);
    sfRectangleShape *gameover = sfRectangleShape_create();
    sfRectangleShape_setSize(gameover, (sfVector2f){160.f, 80.f});
    // set postion midle
    sfRectangleShape_setPosition(gameover, (sfVector2f){240.f, 240.f});
    sfRectangleShape_setTexture(gameover, texture, sfTrue);
    sfRenderWindow_drawRectangleShape(window, gameover, NULL);
    sfRenderWindow_display(window);
    sleep(10);
    sfTexture_destroy(texture);

    for (int i = 0; i < GOST_P; i++)
    {
        sfCircleShape_destroy(pacman[i]);
        sfRectangleShape_destroy(pacman_dying[i]);
        sfRectangleShape_destroy(ghost1[i]);
        sfRectangleShape_destroy(ghost2[i]);
        sfRectangleShape_destroy(ghost3[i]);
        sfRectangleShape_destroy(ghost4[i]);
    }
    sfRectangleShape_destroy(ghost_flash);
    sfRectangleShape_destroy(wblock);
    sfRectangleShape_destroy(block);
    sfCircleShape_destroy(circle);
    sfRectangleShape_destroy(block);
    sfRenderWindow_destroy(window);
}

int main()
{
    pac.x = 40.0;
    pac.y = 27 * 20.0;

    pac.px = pac.x;
    pac.py = pac.y;

    g1.y = (2.0 + 1) * 20;
    g1.x = (1.0 + 2) * 20;
    g2.y = (3.0 + 1) * 20;
    g2.x = (27.0 + 2) * 20;
    g3.y = (19.0 + 1) * 20;
    g3.x = (26.0 + 2) * 20;
    g4.y = (26.0 + 2) * 20;
    g4.x = (25.0 + 1) * 20;

    g1.px = g1.x;
    g1.py = g1.y;
    g2.px = g2.x;
    g2.py = g2.y;
    g3.px = g3.x;
    g3.py = g3.y;
    g4.px = g4.x;
    g4.py = g4.y;

    g1.mov = 'r';
    g2.mov = 'l';
    g3.mov = 'r';
    g4.mov = 'l';

    g1.isblue = 0;
    g2.isblue = 0;
    g3.isblue = 0;
    g4.isblue = 0;

    g1.ck = 0;
    g2.ck = 0;
    g3.ck = 0;
    g4.ck = 0;

    g1.count = 10;
    g2.count = 10;
    g3.count = 10;
    g4.count = 10;

    pac.mov = 'r';
    pac.mp = 0;

    g1.mov1 = 0;
    g2.mov1 = 0;
    g3.mov1 = 0;
    g4.mov1 = 0;

    a1 = g1;
    a2 = g2;
    a3 = g3;
    a4 = g4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                 Init Threads                                                       //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pthread_mutex_lock(&op);
    open = 0;
    pthread_mutex_lock(&in);
    input = 0;
    pthread_mutex_lock(&mutex2);
    g1.mov1 = 0;
    pthread_mutex_lock(&mutex3);
    g2.mov1 = 0;
    pthread_mutex_lock(&mutex4);
    g3.mov1 = 0;
    pthread_mutex_lock(&mutex5);
    g4.mov1 = 0;
    pthread_mutex_lock(&mutex1);
    pac.mp = 0;

    pthread_t pid1, pid2, pid3, pid4, pid5, pid6;

    pthread_create(&pid1, NULL, Input, NULL);

    pthread_create(&pid2, NULL, GhostMovement1, NULL);

    pthread_create(&pid3, NULL, GhostMovement2, NULL);

    pthread_create(&pid4, NULL, GhostMovement3, NULL);

    pthread_create(&pid5, NULL, GhostMovement4, NULL);

    pthread_create(&pid6, NULL, Backend, NULL);

    pthread_exit(NULL);
}