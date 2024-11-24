#include "graphics.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <bitset>

using namespace std;

ifstream fin("map.in");

#define M_PI 3.14159265359
#define mW 25
#define mH 25
#define sW 1280
#define sH 720

//maps are only square for now

int map[mW][mH];

void mapData(int n)
{
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            fin >> map[i][j];
        }
    }
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
    mapData(24);

    double posX = 22, posY = 12;  //pozitiile de start ale playerului
    double dirX = -1, dirY = 0; //directia in care se uita
    double planeX= 0, planeY = 0.66; //the 2d raycaster version of camera plane

    double time = 0; //timingul la frameul curent
    double oldTime = 0; //timingul la frameul anterior

    initwindow(sW, sH, "Raycaster");

    char ch;
    while(1)
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

            //calculum distantei (NU CALCULA CU EUCLID ==> fisheye effect)
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
            int r=0,g=0,b=0;
            float epsilon = sH / 255;
            int drawLum = (drawEnd - drawStart)/epsilon;
            switch (map[mapX][mapY])
            {
                case 1:  
                    if (100 + drawLum < 255)
                    {
                        r = 100 + drawLum; g = 0; b = 0;
                    }
                    else
                    {
                        r = 255; g = 0; b = 0;
                    }
                    break; //red
                case 2:  
                    if (100 + drawLum < 255)
                    {
                        r = 0; g = 100 + drawLum; b = 0;
                    }
                    else
                    {
                        r = 0; g = 255; b = 0;
                    }
                    break; //green
                case 3:  
                    if (100 + drawLum < 255)
                    {
                        r = 0; g = 0; b = 100 + drawLum;
                    }
                    else
                    {
                        r = 0; g = 0; b = 255;
                    }
                    break; //blue
				case 4:  
                    if (100 + drawLum < 255)
                    {
                        r = 100 + drawLum; g = 100 + drawLum; b = 100 + drawLum;
                    }
                    else
                    {
                        r = 255; g = 255; b = 255;
                    }
                    break;   //white
				default: 
                    if (100 + drawLum < 255)
                    {
                        r = 100 + drawLum; g = 100 + drawLum; b = 0;
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
			setcolor(COLOR(r, g, b));
            line(x, drawStart, x, drawEnd);
        }

		swapbuffers();
        oldTime = time;
        time++;

		double frameTime = (time - oldTime) / 1000.0; //frameTime = timpul in secunde pentru frameul curent
        
        double moveSpeed = frameTime * 100.0;
        double rotSpeed = frameTime * 30.0;

        ch = getch();

        if(ch == 119) // in fata
        {
            if (map[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if (map[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }

        if(ch == 115) // in spate
        {
            if (map[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if (map[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }

		//rotam directia camerei
		if (ch == 100) // dreapta
        {
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }

		if (ch == 97) // stanga
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

        if (ch == 27) // 27 = ESC (ASCII)
        {
            return 0;
        }
    }

    while (!kbhit())
    {
        delay(200);
    }

    return 0;
}
