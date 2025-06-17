#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

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

pthread_mutex_t	*init_forks(int n)
{
	pthread_mutex_t	*forks;
	int				i;

	forks = malloc(sizeof(pthread_mutex_t) * n);
	if (!forks)
		return (NULL);
	i = 0;
	while (i < n)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	return (forks);
}

void	*philo_loop(void *arg)
{
	t_philo	*philo = (t_philo *)arg;

	while (1)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf("time: %ld, id: %d is sleeping\n", get_time(), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		usleep(philo->input->sleep_time * 1000);
	}
	return (NULL);
}

int	init_philos(t_input *input)
{
	pthread_t		*threads = malloc(sizeof(pthread_t) * input->philo_num);
	t_philo			*philos = malloc(sizeof(t_philo) * input->philo_num);
	pthread_mutex_t	*forks = init_forks(input->philo_num);
	pthread_mutex_t	*print_mutex = malloc(sizeof(pthread_mutex_t));
	int				i;

	if (!threads || !philos || !forks || !print_mutex)
		return (0);
	pthread_mutex_init(print_mutex, NULL);
	i = 0;
	while (i < input->philo_num)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].input = input;
		philos[i].forks = forks;
		philos[i].print_mutex = print_mutex;
		pthread_create(&threads[i], NULL, philo_loop, &philos[i]);
		i++;
	}
	i = 0;
	while (i < input->philo_num)
		pthread_join(threads[i++], NULL);
	return (1);
}

int	main(int argc, char **argv)
{
	t_input	input;

	if (!get_input(argc, argv, &input))
		return (1);
	printf("PHILO START\n");
	if (!init_philos(&input))
		return (1);
	return (0);
}
