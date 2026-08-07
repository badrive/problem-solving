#define _POSIX_C_SOURCE 200809L	/* needed for getline */
#include <stdio.h>
#include <stdlib.h>

/* everything about one map lives in this box */
typedef struct s_map
{
	int		rows;		/* number of lines            */
	int		cols;		/* width of every line        */
	char	empty;		/* the '.' character          */
	char	obst;		/* the 'o' character          */
	char	full;		/* the 'x' character          */
	char	**grid;		/* the map itself             */
}	t_map;

/* ---------- free the grid inside the struct ---------- */
void	free_map(t_map *m)
{
	int	i;

	if (!m->grid)
		return ;
	for (i = 0; i < m->rows; i++)
		free(m->grid[i]);		/* free(NULL) is safe */
	free(m->grid);
	m->grid = NULL;
}

/* ---------- free the dp table ---------- */
void	free_dp(int **dp, int rows)
{
	int	i;

	if (!dp)
		return ;
	for (i = 0; i < rows; i++)
		free(dp[i]);
	free(dp);
}

/* ---------- read + check the map: 1 = ok, 0 = map error ---------- */
int	read_map(FILE *f, t_map *m)
{
	char	*line;
	size_t	cap;
	long	len;
	int		i, j, ok;

	m->grid = NULL;
	if (fscanf(f, "%d %c %c %c\n", &m->rows, &m->empty, &m->obst,
			&m->full) != 4)
		return (0);
	if (m->rows < 1 || m->empty == m->obst || m->empty == m->full
		|| m->obst == m->full)
		return (0);
	ok = 1;
	m->cols = 0;
	m->grid = calloc(m->rows, sizeof(char *));
	for (i = 0; i < m->rows; i++)
	{
		line = NULL;
		cap = 0;
		len = getline(&line, &cap, f);
		if (len <= 0)						/* file ended too early */
		{
			free(line);
			ok = 0;
			break ;
		}
		if (line[len - 1] == '\n')
			line[--len] = '\0';
		m->grid[i] = line;					/* stored -> will be freed */
		if (i == 0)
			m->cols = (int)len;
		if (len == 0 || (int)len != m->cols)	/* same width, not empty */
			ok = 0;
		for (j = 0; j < (int)len; j++)			/* legal characters only */
			if (line[j] != m->empty && line[j] != m->obst)
				ok = 0;
	}
	if (ok)									/* nothing after last row */
	{
		line = NULL;
		cap = 0;
		if (getline(&line, &cap, f) > 0)
			ok = 0;
		free(line);
	}
	if (!ok)
	{
		free_map(m);
		return (0);
	}
	return (1);
}

/* ---------- the algorithm: now takes one argument ---------- */
void	solve(t_map *m)
{
	int	**dp;
	int	i, j, up, left, diag, mn;
	int	best = 0, br = 0, bc = 0;

	dp = malloc(m->rows * sizeof(int *));
	for (i = 0; i < m->rows; i++)
		dp[i] = calloc(m->cols, sizeof(int));
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			if (m->grid[i][j] != m->empty)		/* obstacle */
				dp[i][j] = 0;
			else if (i == 0 || j == 0)			/* first row / col */
				dp[i][j] = 1;
			else
			{
				up = dp[i - 1][j];
				left = dp[i][j - 1];
				diag = dp[i - 1][j - 1];
				mn = up < left ? up : left;
				if (diag < mn)
					mn = diag;
				dp[i][j] = mn + 1;
			}
			if (dp[i][j] > best)				/* '>' keeps top-left */
			{
				best = dp[i][j];
				br = i;
				bc = j;
			}
		}
	}
	for (i = br - best + 1; i <= br; i++)		/* paint */
		for (j = bc - best + 1; j <= bc; j++)
			m->grid[i][j] = m->full;
	free_dp(dp, m->rows);
}

/* ---------- handle one map from an open stream ---------- */
void	do_map(FILE *f)
{
	t_map	m;
	int		i;

	if (!read_map(f, &m))
	{
		fprintf(stderr, "map error\n");
		return ;
	}
	solve(&m);
	for (i = 0; i < m.rows; i++)
		fprintf(stdout, "%s\n", m.grid[i]);
	free_map(&m);
}

int	main(int argc, char **argv)
{
	FILE	*f;
	int		i;

	if (argc < 2)					/* no argument -> read stdin */
	{
		do_map(stdin);
		return (0);
	}
	for (i = 1; i < argc; i++)		/* one or more files */
	{
		f = fopen(argv[i], "r");
		if (!f)
			fprintf(stderr, "map error\n");
		else
		{
			do_map(f);
			fclose(f);
		}
		if (argc > 2)				/* line break after each map */
			fprintf(stdout, "\n");
	}
	return (0);
}
