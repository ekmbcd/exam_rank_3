#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//size of the board
int X;
int Y;
//the drawing
char **drawing;

typedef struct scircle
{
	//point coordinates
	float px, py;
	//radius of the circle
	float radius;
	//type of rectangle and the char to draw it
	char type, fill;
} circle;

//print a string
void print(char *s)
{
	int i = 0;

	while (s[i])
		i++;
	write(1, s, i);
}

void paintpixel(int x, int y, circle c)
{
	//check if point xy is in the circle
	float dist;

	//Pythagoras theorem to find distance between point and circle's center
	dist = sqrtf((x - c.px) * (x - c.px) + (y - c.py) * (y - c.py));

	if (dist <= c.radius)
	{
		if (c.type == 'C')
			drawing[y][x] = c.fill;
		else
		{
			//check if point is on the circumference
			//(control if there is a further away point that belongs to the circle)
			if (dist + 1 > c.radius)
				drawing[y][x] = c.fill;
		}
	}
}

int checkcircle(FILE *file)
{

	circle c;
	int res;
	int x, y = 0;

	res = fscanf(file, "\n%c %f %f %f %c", &c.type, &c.px, &c.py, &c.radius, &c.fill);

	//res = -1 means file is over
	if (res == -1)
		return (0);

	//circle is wrong
	if (res < 5 || c.radius <= 0 || (c.type != 'c' && c.type != 'C'))
	{
		return (1);
	}

	//draw the circle inside drawing
	while (y < Y)
	{
		x = 0;
		while (x < X)
		{
			paintpixel(x, y, c);
			x++;
		}
		y++;
	}
	//all went well, read the next line (return something other than 1)
	return (42);
}

void createbg(char bg)
{
	int x, y = 0;

	//malloc a matrix of chars and fill it with bg
	drawing = malloc(sizeof(char *) * Y);
	while (y < Y)
	{
		x = 0;
		//remember to malloc each line
		drawing[y] = malloc(X);
		while (x < X)
		{
			drawing[y][x] = bg;
			x++;
		}
		y++;
	}
}

void drawboard()
{
	int x, y = 0;

	//prints the drawing
	while (y < Y)
	{
		x = 0;
		while (x < X)
		{
			write(1, &drawing[y][x], 1);
			x++;
		}
		write(1, "\n", 1);
		y++;
	}
}

void freedrawing()
{
	int y = 0;

	//remember to free every string
	while (y < Y)
	{
		free(drawing[y]);
		y++;
	}

	free(drawing);
}

int checkfile(FILE *file)
{
	int ret;
	//background char
	char bg;

	//read first line (background)
	if (fscanf(file, "%d %d %c", &X, &Y, &bg) != 3)
		return (1);

	//check if background is correct
	if (X < 1 || Y < 1 || X > 300 || Y > 300)
		return (1);

	//setup the background
	createbg(bg);

	//start reading circles, stop if error is found
	while ((ret = checkcircle(file)))
		if (ret == 1)
		{
			//ret = 1 means an error is found
			freedrawing();
			return (1);
		}

	//if we get here all went well
	drawboard();
	freedrawing();

	return (0);
}

int main(int ac, char **av)
{
	FILE *file;

	//requires a filename
	if (ac == 2)
	{
		file = fopen(av[1], "r");

		//checkfile returns 0 if all went well
		if (file && !checkfile(file))
		{
			//remember to fclose
			fclose(file);
			return 0;
		}

		//also fclose in case of errors
		if (file)
			fclose(file);

		//something wrong with file
		print("Error: Operation file corrupted\n");
	}
	else
	{
		//wrong number of arguments
		print("Error: argument\n");
	}
	return (1);
}