#include "philo.h"

long	ft_long_atoi(const char *string)
{
	int		i;
	int		negative;
	long	result;

	i = 0;
	negative = 1;
	result = 0;
	while (string[i] == ' ' || (string[i] >= 9 && string[i] <= 13))
		i++;
	if (string[i] == '+' || string[i] == '-')
	{
		if (string[i] == '-')
			negative *= -1;
		i++;
	}
	while (string[i] >= '0' && string[i] <= '9')
	{
		result = (result * 10) + (string[i] - 48);
		i++;
	}
	return (result * negative);
}

int	ft_num_check(char *s)
{
	if (s[0] == '+' && s[1] == '0' && s[2] == '0')
		return (0);
	if (s[0] == '0' && (s[1] >= '0' && s[1] <= '9'))
		return (0);
	if (s[0] >= '0' && s[0] <= '9')
		return (1);
	else if ((s[0] == '+') && (s[1] >= '0' && s[1] <= '9'))
		return (1);
	else
		return (0);
}

int	main(int argc, char **argv)
{
	int		i;
	long	num;

	if (argc != 5 && argc != 6)
	{
		printf("Error input\n");
		return (1);
	}
	i = 1;
	while (i < argc)
	{
		if (ft_num_check(argv[i]) == 0)
			return (printf("Error input\n"), 1);
		num = ft_long_atoi(argv[i]);
		i++;
	}
	printf("PHILO START\n");
	argv[1] = "a";
	return (0);
}