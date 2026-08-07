#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct life {
    char *cells;
    int w;
    int h;
    int px;
    int py;
    int iters;
    int pen;
}               t_life;


int     init(t_life  *l, char **av)
{
    l->cells = NULL;
    l->px = 0;
    l->py = 0;
    l->pen = 0;
    l->w = atoi(av[1]);
    l->h = atoi(av[2]);
    l->iters = atoi(av[3]);
    if (l->w <= 0 || l->h <= 0 || l->iters < 0) return 0;
    l->cells = calloc(l->w,l->h);
    if (!l->cells)
        return 0;
    return 1;
}

int draw(t_life *l) {
    char buff[4000];
    int i;
    int r = 0;
    while ((r = read(0, buff, 4000)) > 0)
    {
        i = 0;
        while (i < r)
        {
            char c = buff[i++];
            if(c == 'x')
                l->pen = !l->pen;
            else if (c == 'a')
                l->px--;
            else if (c == 'w')
                l->py--;
            else if (c == 's')
                l->py++;
            else if (c == 'd')
                l->px++;
            else continue;
            if (l->pen && l->px >= 0 && l->px < l->w && l->py >= 0 && l->py < l->h)
                l->cells[l->py * l->w + l->px] = 1;
        }
    }
    if (r < 0)
        return 0;
    return 1;
}

int jiran (t_life *l, int x, int y)
{
    int newx = 0;
    int newy = 0;
    int count = 0;
    for (int movey = -1; movey <= 1 ; movey++)
    {
        for (int movex = -1; movex <= 1 ; movex++)
        {
            newx = x + movex;
            newy = y + movey;
            if ((movex || movey) && newx >= 0 && newx < l->w && newy >= 0 && newy < l->h)
              count += l->cells[newy * l->w + newx];  
        }
    }
    return count;
}

int step (t_life *l)
{
    int n = 0;
    char *next= NULL;
    next = calloc(l->w,l->h);
    if (!next)
        return 0;
    for(int i = 0; i < l->h; i++)
    {
        for(int j = 0; j < l->w; j++)
        {
            n = jiran(l,j,i);
            if ( n == 3 || (n == 2 && l->cells[i * l->w + j]))
                next[i * l->w + j] = 1;
        }
    }
    free(l->cells);
    l->cells = next;
    return 1;
}

void print(t_life *l)
{
    for(int i = 0; i < l->h; i++)
    {
        for(int j = 0; j < l->w; j++)
        {
            // printf("%i",l->cells[i * l->w + j]);
            if (l->cells[i * l->w + j] == 1 )
                printf("O");
            else {

                printf(" ");
            }
        }
        printf("\n");
    }
}
int main(int ac, char **av)
{
    if (ac != 4) return 1;
    t_life l;

    if (!init(&l,av))
        return(free(l.cells), 1);
    if (!draw(&l))
        return(free(l.cells), 1);
    while(l.iters-- > 0)
    {
        if (!step(&l))
            return(free(l.cells), 1);
    }
    print(&l);
    free(l.cells);
    return 0;
}
