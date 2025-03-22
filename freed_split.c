#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void	free_array(char **arr, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}


int	word_count(char *str, char c)
{
	int		i;
	size_t	count;

	count = 0;
	i = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		if (str[i] != c)
		{
			while (str[i] != c)
				i++;
			count++;
		}
		if (!str[i])
			break ;
		i++;
	}
	return (count);
}

char	*word_allocate(char *str, char c)
{
	int			i;
	char		*str_malloc;

	i = 0;
	while (str[i] && str[i] != c)
		i++;
	str_malloc = (char *)malloc(sizeof(char) * (i + 1));
	i = 0;
	while (str[i] && str[i] != c)
	{
		str_malloc[i] = str[i];
		i++;
	}
	str_malloc[i] = '\0';
	return (str_malloc);
}

char	**ft_split(char *str, char c)
{
	int		i;
	char	**arr;

	i = 0;
	arr = (char **)malloc(sizeof(char *) * (word_count(str, c) + 1));
	if (!arr)
		return (NULL);
	while (*str)
	{
		while (*str && *str == c)
			str++;
		if (*str && *str != c)
		{
			arr[i] = word_allocate(str, c);
			if (!arr[i])
			{
				free_array(arr, i);
				return (NULL);
			}
			i++;
		}
		while (*str && *str != c)
			str++;
	}
	arr[i] = NULL;
	return (arr);
}

int	main()
{
    int a;
	char	*str = "lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultricies diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi.";
	printf("%d\n", word_count(str, ' '));
	// printf("%d %d\n", a, word_count(str, ' '));
	int	i = 0;
	char	**arr = ft_split(str, ' ');
	while (i < word_count(str, ' '))
	{
		printf("%s\n", arr[i]);
		i++;
	}
	free_array(arr, word_count(str, ' '));
	return (0);
}