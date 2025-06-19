/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 11:26:00 by omizin            #+#    #+#             */
/*   Updated: 2025/06/18 11:26:00 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	get_current_ms(t_input *input)
{
	return (get_time() - input->start_time);
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

void	set_input_info(t_input *input, int argc, char **argv)
{
	input->philo_num = ft_long_atoi(argv[1]);
	input->fork_num = ft_long_atoi(argv[1]);
	input->die_time = ft_long_atoi(argv[2]);
	input->eat_time = ft_long_atoi(argv[3]);
	input->sleep_time = ft_long_atoi(argv[4]);
	input->start_time = get_time();
	if (argc == 6)
		input->meal_num = ft_long_atoi(argv[5]);
}

int	get_input(int argc, char **argv, t_input *input)
{
	int		i;

	if (argc != 5 && argc != 6)
	{
		printf("Error input\n");
		return (0);
	}
	i = 1;
	while (i < argc)
	{
		if (ft_num_check(argv[i]) == 0)
			return (printf("Error input\n"), 1);
		i++;
	}
	set_input_info(input, argc, argv);
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

void	smart_sleep(long ms)
{
	long	start;

	start = get_time();
	while (get_time() - start < ms)
		usleep(100);
}

void	*philo_loop(void *arg)
{
	t_philo	*philo = (t_philo *)arg;

	while (1)
	{
		pthread_mutex_lock(philo->print_mutex);
		printf("time: %ld, id: %d is thinking\n", get_current_ms(philo->input), philo->id);
		pthread_mutex_unlock(philo->print_mutex);

		pthread_mutex_lock(philo->print_mutex);
		printf("time: %ld, id: %d is eating\n", get_current_ms(philo->input), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		philo->last_meal_time = get_current_ms(philo->input);
		smart_sleep(philo->input->eat_time);

		pthread_mutex_lock(philo->print_mutex);
		printf("time: %ld, id: %d is sleeping\n", get_current_ms(philo->input), philo->id);
		pthread_mutex_unlock(philo->print_mutex);
		smart_sleep(philo->input->sleep_time);
	}
	return (NULL);
}

void	*death_monitoring(void *arg)
{
	t_monitoring	*mon;
	int				i;
	long			time_since_meal;

	mon = (t_monitoring *)arg;
	while (1)
	{
		i = 0;
		while (i < mon->input->philo_num)
		{
			time_since_meal = get_current_ms(mon->input) - mon->philos[i].last_meal_time;
			if (time_since_meal >= mon->input->die_time)
			{
				pthread_mutex_lock(mon->print_mutex);
				printf("time: %ld, id: %d is died\n", get_current_ms(mon->input), mon->philos[i].id);
				pthread_mutex_unlock(mon->print_mutex);
				exit(0);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}

int	init_philos(t_input *input)
{
	pthread_t		*threads;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*print_mutex;
	pthread_t		monitoring_thread;
	t_monitoring	*mon;
	int				i;

	threads = malloc(sizeof(pthread_t) * input->philo_num);
	philos = malloc(sizeof(t_philo) * input->philo_num);
	forks = init_forks(input->philo_num);
	print_mutex = malloc(sizeof(pthread_mutex_t));
	mon = malloc(sizeof(t_monitoring));
	if (!threads || !philos || !forks || !print_mutex || !mon)
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
		philos[i].last_meal_time = get_current_ms(input);
		pthread_create(&threads[i], NULL, philo_loop, &philos[i]);
		i++;
	}
	mon->philos = philos;
	mon->input = input;
	mon->print_mutex = print_mutex;
	pthread_create(&monitoring_thread, NULL, death_monitoring, mon);
	i = 0;
	while (i < input->philo_num)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
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
