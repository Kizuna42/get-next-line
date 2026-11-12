/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kyuki <kyuki@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 01:57:15 by kyuki             #+#    #+#             */
/*   Updated: 2021/04/06 10:06:19 by kyuki            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

int	ft_malloc_p(void **p, size_t len)
{
	*p = malloc(len);
	if (*p == NULL)
		return (0);
	return (1);
}

static int	free_all(char **save, char **line)
{
	free(*save);
	*save = NULL;
	free(*line);
	*line = NULL;
	return (READ_ERROR);
}

static int	read_to_save(int fd, char **save, char *buf)
{
	ssize_t	ret;
	char	*tmp;

	ret = 1;
	while (ret > 0 && !(*save != NULL && ft_strchr(*save, '\n')))
	{
		ret = read(fd, buf, BUFFER_SIZE);
		if (ret <= 0)
			break ;
		buf[ret] = '\0';
		if (*save == NULL)
			tmp = ft_strdup(buf);
		else
			tmp = ft_strjoin(*save, buf);
		free(*save);
		*save = tmp;
		if (*save == NULL)
			return (READ_ERROR);
	}
	return ((int)ret);
}

static int	appendline_freesave(char **save, char **line)
{
	size_t	len;
	int		is_newline;
	char	*tmp;

	len = 0;
	while ((*save)[len] != '\n' && (*save)[len] != '\0')
		len++;
	is_newline = ((*save)[len] == '\n');
	*line = ft_substr(*save, 0, len);
	tmp = NULL;
	if (is_newline)
		tmp = ft_strdup(&((*save)[len + 1]));
	free(*save);
	*save = tmp;
	if (*line == NULL || (is_newline && tmp == NULL))
		return (free_all(save, line));
	if (is_newline)
		return (READ);
	return (READ_EOF);
}

int	get_next_line(const int fd, char **line)
{
	int				ret;
	static char		*save[MAX_FD];
	char			*buf;

	if (fd < 0 || fd >= MAX_FD || line == NULL || BUFFER_SIZE <= 0)
		return (READ_ERROR);
	*line = NULL;
	if (!ft_malloc_p((void **)&buf, (size_t)BUFFER_SIZE + 1))
		return (READ_ERROR);
	ret = read_to_save(fd, &save[fd], buf);
	free(buf);
	if (ret < 0)
		return (free_all(&save[fd], line));
	if (save[fd] == NULL)
	{
		*line = ft_strdup("");
		if (*line == NULL)
			return (READ_ERROR);
		return (READ_EOF);
	}
	return (appendline_freesave(&save[fd], line));
}
