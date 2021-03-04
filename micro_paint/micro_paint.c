#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//size of the board
int X;
int Y;
//the drawing
char **drawing;

typedef struct srect
{
	//point coordinates
	float px, py;
	//size of rectangle
	float xsize, ysize;
	//type of rectangle and the char to draw it
	char type, fill;
} rect;

//print a string
void print(char *s)
{
	int i = 0;

	while (s[i])
		i++;
	write(1, s, i);
}

void paintpixel(int x, int y, rect r)
{
	//check if point xy is in the rectangle
	if (x >= r.px && x <= r.px + r.xsize && y >= r.py && y <= r.py + r.ysize)
	{
		if (r.type == 'R')
			drawing[y][x] = r.fill;
		else
		{
			//check if point is on the border
			if (!(x - 1 >= r.px && x + 1 <= r.px + r.xsize && y - 1 >= r.py && y + 1 <= r.py + r.ysize))
				drawing[y][x] = r.fill;
		}
	}
}

int checkrect(FILE *file)
{

	rect r;
	int res;
	int x, y = 0;

	res = fscanf(file, "\n%c %f %f %f %f %c", &r.type, &r.px, &r.py, &r.xsize, &r.ysize, &r.fill);

	//res = -1 means file is over
	if (res == -1)
		return (0);

	//rectangle is wrong
	if (res < 6 || r.xsize <= 0 || r.ysize <= 0 || (r.type != 'r' && r.type != 'R'))
	{
		return (1);
	}

	//put the rectangle inside drawing
	while (y < Y)
	{
		x = 0;
		while (x < X)
		{
			paintpixel(x, y, r);
			x++;
		}
		y++;
	}
	//all went well, read the next line
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

	//start reading rectangles, stop if error is found
	while ((ret = checkrect(file)))
		if (ret == 1)
		{
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

		//check returns 0 if all went well
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