/* Test driver: prints "<ret>|<line>" for each get_next_line call.
 * Usage: test_gnl <file>            ... read the file until EOF
 *        test_gnl --errors          ... invalid argument checks
 *        test_gnl --multi f1 f2     ... (bonus) interleave two fds */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	get_next_line(const int fd, char **line);

static int	dump(int fd)
{
	char	*line;
	int		ret;

	while (1)
	{
		line = NULL;
		ret = get_next_line(fd, &line);
		printf("%d|%s\n", ret, line ? line : "(null)");
		free(line);
		if (ret <= 0)
			return (ret);
	}
}

static int	errors(void)
{
	char	*line;
	int		fds[2];

	line = NULL;
	printf("neg_fd=%d\n", get_next_line(-1, &line));
	printf("bad_fd=%d\n", get_next_line(4242, &line));
	printf("null_line=%d\n", get_next_line(0, NULL));
	if (pipe(fds) != 0)
		return (1);
	close(fds[1]);
	printf("closed_pipe=%d\n", get_next_line(fds[0], &line));
	free(line);
	close(fds[0]);
	return (0);
}

static int	multi(const char *f1, const char *f2)
{
	int		fd[2];
	int		ret[2];
	char	*line;
	int		i;

	fd[0] = open(f1, O_RDONLY);
	fd[1] = open(f2, O_RDONLY);
	ret[0] = 1;
	ret[1] = 1;
	while (ret[0] > 0 || ret[1] > 0)
	{
		for (i = 0; i < 2; i++)
		{
			if (ret[i] <= 0)
				continue ;
			line = NULL;
			ret[i] = get_next_line(fd[i], &line);
			printf("fd%d:%d|%s\n", i, ret[i], line ? line : "(null)");
			free(line);
		}
	}
	close(fd[0]);
	close(fd[1]);
	return (0);
}

int	main(int ac, char **av)
{
	int	fd;

	if (ac == 2 && strcmp(av[1], "--errors") == 0)
		return (errors());
	if (ac == 4 && strcmp(av[1], "--multi") == 0)
		return (multi(av[2], av[3]));
	if (ac != 2)
		return (2);
	fd = open(av[1], O_RDONLY);
	if (fd < 0)
		return (2);
	dump(fd);
	close(fd);
	return (0);
}
