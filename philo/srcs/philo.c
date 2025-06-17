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

void	set_input_info(t_input *input, int argc, int var[argc - 1])
{
	input->philo_num = var[0];
	input->fork_num = var[0];
	input->die_time = var[1];
	input->eat_time = var[2];
	input->sleep_time = var[3];
	if (argc == 6)
		input->meal_num = var[4];
}

int	get_input(int argc, char **argv, t_input *input)
{
	int		i;
	long	num;
	int		j;
	int		var[argc - 1];

	if (argc != 5 && argc != 6)
	{
		printf("Error input\n");
		return (0);
	}
	i = 1;
	j = 0;
	while (i < argc)
	{
		if (ft_num_check(argv[i]) == 0)
			return (printf("Error input\n"), 1);
		num = ft_long_atoi(argv[i]);
		var[j] = num;
		j++;
		i++;
	}
	set_input_info(input, argc, var);
	return (1);
}

int	main(int argc, char **argv)
{
	t_input	input;

	if (!get_input(argc, argv, &input))
		return (1);
	printf("PHILO START\n");
	printf("%d\n%d\n%d\n%d\n", input.philo_num, input.eat_time, input.sleep_time, input.meal_num);
	return (0);
}