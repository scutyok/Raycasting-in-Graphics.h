#include "graphics.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <bitset>

using namespace std;

#define M_PI 3.14159265359
#define mW 25
#define mH 25
#define sW 1280
#define sH 720

//maps can be any shape now
int map[mW][mH];

//pozitiile de start ale playerului
double posX, posY;

int mapData(char n[])
{
	char s[100] = {'m', 'a', 'p',n[0],n[1],n[2],n[3], '.', 'i', 'n'};
    ifstream fin(s);
    int i=1, j=1, jnk;
    char ch,oldch=1;
    fin >> posX >> posY;
    fin >> jnk;
    while (fin >> noskipws >> ch)
    {
        if (oldch == '\n' && ch == '\n')
        {
            break;
        }
        if (ch >= 48 && ch <=57)
        {
            map[i][j] = ch - 48;
            j++;
        }
        if (ch == '\n')
        {
            j = 1;
            i++;
        }
        oldch = ch;
    }
    return 1;
}

void rectangleRotate(int cx, int cy, int w, int h, int angle = 0)
{
	double theta = (double)(angle % 180) * M_PI / 180; //conversia unghiului in radiani
    int dx = w / 2;
    int dy = h / 2;

    int point[8] = {
        (int)(-dx * cos(theta) - dy * sin(theta) + cx), //latura 1
        (int)(-dx * sin(theta) + dy * cos(theta) + cy),
		(int)(dx * cos(theta) - dy * sin(theta) + cx), //latura 2
        (int)(dx * sin(theta) + dy * cos(theta) + cy),
		(int)(dx * cos(theta) + dy * sin(theta) + cx), //latura 3
        (int)(dx * sin(theta) - dy * cos(theta) + cy),
		(int)(-dx * cos(theta) + dy * sin(theta) + cx), //latura 4
        (int)(-dx * sin(theta) - dy * cos(theta) + cy)
    };

    for (int i = 0; i < 8; i += 2) 
    {
        line(point[i], point[i + 1], point[(i + 2) % 8], point[(i + 3) % 8]);
    }
}

int main()
{
    double dirX = -1, dirY = 0; //directia in care se uita
    double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

    double time = 0; //timingul la frameul curent
    double oldTime = 0; //timingul la frameul anterior

    initwindow(sW, sH, "Raycaster");

    char aux, key, ch[10000], numb[10000];

    int ok = 0;
    while (ok == 0)
    {
        settextstyle(3, 0, 5);
        char numbtxt[100] = { 'n', 'u', 'm', 'b', 'e', 'r', ':', ' '};
        outtextxy(0, 0, numbtxt);
        do
        {
            char aux;
            aux = getch();
            if (aux >= 48 && aux <= 57)
            {
                numb[0] = (int)aux;
				ch[0] = (int)aux;
				ch[1] = '\0';
                outtextxy(180, 0, ch);
            }
            aux = getch();
            if (aux >= 48 && aux <= 57)
            {
                numb[1] = (int)aux;
                ch[0] = (int)aux;
                ch[1] = '\0';
                outtextxy(210, 0, ch);
            }
            aux = getch();
            if (aux >= 48 && aux <= 57)
            {
                numb[2] = (int)aux;
                ch[0] = (int)aux;
                ch[1] = '\0';
                outtextxy(240, 0, ch);
            }
            aux = getch();
            if (aux >= 48 && aux <= 57)
            {
                numb[3] = (int)aux;
                ch[0] = (int)aux;
                ch[1] = '\0';
                outtextxy(270, 0, ch);
            }
			char key = getch();
            if (key == 32)
            {
                break;
            }
        }while (1);
		numb[4] = '\0';
        char t[100] = { 'm', 'a', 'p', ':', ' ', numb[0],numb[1] ,numb[2] ,numb[3]};
        outtextxy(0, 50, t);
        key = getch();
		if (key == 32)
		{
            ok = mapData(numb);
		}
    }

    if (key == 32)
    {
		setbkcolor(COLOR(10,10, 10));
        while (1)
        {
            cleardevice();

            for (int x = 0; x <= sW; x++)
            {
                //algoritmul DDA
                double cameraX = 2 * x / double(sW) - 1;
                double rayDirX = dirX + planeX * cameraX;
                double rayDirY = dirY + planeY * cameraX;

                //PATRATUL IN CARE NE AFLAM
                int mapX = (int)posX;
                int mapY = (int)posY;

                //lungimea de la player pana la urmatoarea latura a patratelor prin care trece traiectoria razei
                double sideDistX;
                double sideDistY;

                //calculul lungimii
                double deltaDistX = (rayDirX == 0) ? 1e30 : abs(1 / rayDirX); //distanta poate fi simplificata de la radical la absolut cu ajutorul derivarii
                double deltaDistY = (rayDirY == 0) ? 1e30 : abs(1 / rayDirY); //putem folosii 1 pentru |rayDir| fiindca am nevoie doar de deltaDistX impartit la deltaDistY
                double perpWallDist; //calculam lungimea razei

                //1e30 = infinit -> nu mai da erroare la impartirea cu 0

                //DDA va sarii intotdeauna cate un patrat, deci daca raza merge in directia X, atunci distanta pana la urmatorul patrat pe X va fi 1 si vice versa in cazul lui Y
                int stepX; //(1,-1)
                int stepY; //(1,-1)

                int hit = 0; //indicele daca a fost lovit un perete
                int side; //a fost atins peretele de pe y sau de pe x?

                //calculam patratul si distanta laterala
                if (rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (posX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - posX) * deltaDistX;
                }

                if (rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (posY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - posY) * deltaDistY;
                }
                //DDA
                while (hit == 0)
                {
                    //pasul la urmatorul patrat pe X sau Y
                    if (sideDistX < sideDistY)
                    {
                        sideDistX += deltaDistX;
                        mapX += stepX;
                        side = 0;
                    }
                    else
                    {
                        sideDistY += deltaDistY;
                        mapY += stepY;
                        side = 1;
                    }
                    //verifica patratu si daca am nimerit un perete
                    if (map[mapX][mapY] > 0)
                    {
                        hit = 1;
                    }
                }

                //calculam distanta (NU CALCULA CU EUCLID ==> fisheye effect)
                if (side == 0)
                {
                    perpWallDist = (sideDistX - deltaDistX);
                }
                else
                {
                    perpWallDist = (sideDistY - deltaDistY);
                }

                //inaltimea peretelui
                int lineHeight = (int)(sH / perpWallDist);

                //cel mai inalt si cel mai scund pixel (e pentru fill)
                int drawStart = -lineHeight / 2 + sH / 2; //incepe de sus
                if (drawStart < 0)
                {
                    drawStart = 0;
                }
                int drawEnd = lineHeight / 2 + sH / 2; //incepe de jos
                if (drawEnd >= sH)
                {
                    drawEnd = sH - 1;
                }
                //bazat pe numarul din matrice, alegem culoarea
                int r = 0, g = 0, b = 0;
                float epsilon = sH / 255;
                int drawLum = (drawEnd - drawStart) / epsilon;
                int startLum = 50;
                switch (map[mapX][mapY])
                {
                case 1:
                    if (startLum + drawLum < 255)
                    {
                        r = startLum + drawLum; g = 0; b = 0;
                    }
                    else
                    {
                        r = 255; g = 0; b = 0;
                    }
                    break; //red
                case 2:
                    if (startLum + drawLum < 255)
                    {
                        r = 0; g = startLum + drawLum; b = 0;
                    }
                    else
                    {
                        r = 0; g = 255; b = 0;
                    }
                    break; //green
                case 3:
                    if (startLum + drawLum < 255)
                    {
                        r = 0; g = 0; b = startLum + drawLum;
                    }
                    else
                    {
                        r = 0; g = 0; b = 255;
                    }
                    break; //blue
                case 4:
                    if (startLum + drawLum < 255)
                    {
                        r = startLum + drawLum; g = startLum + drawLum; b = startLum + drawLum;
                    }
                    else
                    {
                        r = 255; g = 255; b = 255;
                    }
                    break;   //white
                default:
                    if (startLum + drawLum < 255)
                    {
                        r = startLum + drawLum; g = startLum + drawLum; b = 0;
                    }
                    else
                    {
                        r = 255; g = 255; b = 0;
                    }
                    break; //yellow
                }
                // luminozitatea peretelui
                if (side == 1)
                {
                    r = r / 2;
                    g = g / 2;
                    b = b / 2;
                }

                //linia verticala
                setfillstyle(SOLID_FILL, COLOR(r,g,b));
                bar(x, drawStart, x + 1, drawEnd);
            }

            swapbuffers();
            oldTime = time;
            time++;

            double frameTime = (time - oldTime) / 1000.0; //frameTime = timpul in secunde pentru frameul curent

            double moveSpeed = frameTime * 100.0;
            double rotSpeed = frameTime * 50.0;

            key = getch();

            if (key == 119) // in fata
            {
                if (map[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
                if (map[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
            }

            if (key == 115) // in spate
            {
                if (map[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
                if (map[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
            }

            //rotam directia camerei
            if (key == 100) // dreapta
            {
                double oldDirX = dirX;
                dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
            }

            if (key == 97) // stanga
            {
                double oldDirX = dirX;
                dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
            }

            if (time > 1000)
            {
                time = 0;
            }

            if (key == 27) // 27 = ESC (ASCII)
            {
                return 0;
            }
        }
    }   

    while (!kbhit())
    {
        delay(200);
    }

    return 0;
}
