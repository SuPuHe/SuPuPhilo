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

#define CHECK_DELAY 1

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

int	is_dead(t_philo *philo)
{
	int	flag;

	pthread_mutex_lock(&philo->input->death_mutex);
	flag = philo->input->death_flag;
	pthread_mutex_unlock(&philo->input->death_mutex);
	return (flag);
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

int	set_input_info(t_input *input, int argc, char **argv)
{
	input->philo_num = ft_long_atoi(argv[1]);
	if (input->philo_num == 0)
		return (0);
	input->fork_num = ft_long_atoi(argv[1]);
	input->die_time = ft_long_atoi(argv[2]);
	input->eat_time = ft_long_atoi(argv[3]);
	input->sleep_time = ft_long_atoi(argv[4]);
	input->start_time = get_time();
	if (argc == 6)
		input->meal_num = ft_long_atoi(argv[5]);
	else
		input->meal_num = -1;
	return (1);
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
			return (printf("Error input\n"), 0);
		i++;
	}
	if (!set_input_info(input, argc, argv))
	{
		printf("Error input\n");
		return (0);
	}
	return (1);
}

void	smart_sleep(long ms, t_philo *philo)
{
	long	start;

	start = get_time();
	while (!is_dead(philo) && get_time() - start < ms)
		usleep(500);
}

void	print_status(t_philo *philo, const char *msg)
{
	if (is_dead(philo))
		return;
	pthread_mutex_lock(philo->print_mutex);
	if (!is_dead(philo))
		printf("time: %ld, id: %d %s\n", get_current_ms(philo->input), philo->id, msg);
	pthread_mutex_unlock(philo->print_mutex);
}

void	*handle_one_philo(t_philo *philo)
{
	print_status(philo, "is thinking");
	pthread_mutex_lock(philo->fork_right);
	print_status(philo, "has taken a fork");
	smart_sleep(philo->input->die_time, philo);
	pthread_mutex_unlock(philo->fork_right);
	return (NULL);
}

int	philo_thinking(t_philo *philo)
{
	print_status(philo, "is thinking");
	return (!is_dead(philo));
}

int	philo_eating(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->fork_left);
	}
	else
	{
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->fork_left);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
	}
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = get_current_ms(philo->input);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_mutex);
	print_status(philo, "is eating");
	smart_sleep(philo->input->eat_time, philo);
	pthread_mutex_unlock(&philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	if (philo->input->meal_num != -1 && philo->meals_eaten >= philo->input->meal_num)
		return (0);
	return (!is_dead(philo));
}

int	philo_sleeping(t_philo *philo)
{
	print_status(philo, "is sleeping");
	smart_sleep(philo->input->sleep_time, philo);
	return (!is_dead(philo));
}

void	*philo_loop(void *arg)
{
	t_philo	*philo = (t_philo *)arg;

	if (philo->id % 2 != 0)
		usleep(500);
	if (philo->input->philo_num == 1)
		return (handle_one_philo(philo));
	while (!is_dead(philo))
	{
		if (!philo_thinking(philo))
			break;
		if (!philo_eating(philo))
			break;
		if (!philo_sleeping(philo))
			break;
	}
	return (NULL);
}

int	check_if_died(t_philo *p, t_input *input, pthread_mutex_t *print_mutex)
{
	long	time_since_meal;

	pthread_mutex_lock(&p->meal_mutex);
	time_since_meal = get_current_ms(input) - p->last_meal_time;
	pthread_mutex_unlock(&p->meal_mutex);
	if (time_since_meal >= input->die_time + CHECK_DELAY)
	{
		pthread_mutex_lock(print_mutex);
		printf("time: %ld, id: %d died\n", get_current_ms(input), p->id);
		pthread_mutex_unlock(print_mutex);
		pthread_mutex_lock(&input->death_mutex);
		input->death_flag = 1;
		pthread_mutex_unlock(&input->death_mutex);
		return (1);
	}
	return (0);
}

int	check_all_full(t_philo *philos, t_input *input, pthread_mutex_t *print_mutex)
{
	int	i;
	int	full_count;

	i = 0;
	full_count = 0;
	while (i < input->philo_num)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (input->meal_num != -1 && philos[i].meals_eaten >= input->meal_num)
			full_count++;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		i++;
	}
	if (full_count == input->philo_num)
	{
		pthread_mutex_lock(print_mutex);
		printf("All philosophers have eaten %d times.\n", input->meal_num);
		pthread_mutex_unlock(print_mutex);
		pthread_mutex_lock(&input->death_mutex);
		input->death_flag = 1;
		pthread_mutex_unlock(&input->death_mutex);
		return (1);
	}
	return (0);
}

void	*death_monitoring(void *arg)
{
	t_monitoring	*mon;
	t_input			*input;
	t_philo			*philos;
	int				i;

	mon = (t_monitoring *)arg;
	input = mon->input;
	philos = mon->philos;
	while (1)
	{
		i = 0;
		while (i < input->philo_num)
		{
			if (check_if_died(&philos[i], input, mon->print_mutex))
				return (NULL);
			i++;
		}
		if (input->meal_num != -1 && check_all_full(philos, input, mon->print_mutex))
			return (NULL);
		usleep(100);
	}
	return (NULL);
}

void	init_philos_forks(t_philo *philos, t_input *input, pthread_mutex_t *print_mutex)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].input = input;
		philos[i].print_mutex = print_mutex;
		pthread_mutex_init(&philos[i].fork_left, NULL);
		pthread_mutex_init(&philos[i].meal_mutex, NULL);
		philos[i].last_meal_time = get_current_ms(input);
		i++;
	}
	i = 0;
	while (i < input->philo_num)
	{
		if (i == 0)
			philos[i].fork_right = &philos[input->philo_num - 1].fork_left;
		else
			philos[i].fork_right = &philos[i - 1].fork_left;
		i++;
	}
}

int	allocate_memory(t_input *input, pthread_t **threads, t_philo **philos)
{
	*threads = malloc(sizeof(pthread_t) * input->philo_num);
	*philos = malloc(sizeof(t_philo) * input->philo_num);
	if (!*threads || !*philos)
		return (0);
	return (1);
}

void	create_threads(t_input *input, pthread_t *threads, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		pthread_create(&threads[i], NULL, philo_loop, &philos[i]);
		usleep(100);
		i++;
	}
}

void	join_threads(t_input *input, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
		pthread_join(threads[i++], NULL);
}

void	destroy_all_mutex(t_input *input, t_philo *philos, pthread_mutex_t *print_mutex)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		pthread_mutex_destroy(&philos[i].fork_left);
		pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(print_mutex);
	pthread_mutex_destroy(&input->death_mutex);
}

int	init_info(t_input *input)
{
	pthread_t		*threads;
	t_philo			*philos;
	t_monitoring	mon;
	pthread_mutex_t	print_mutex;
	pthread_t		monitoring_thread;

	if (!allocate_memory(input, &threads, &philos))
		return (0);
	pthread_mutex_init(&print_mutex, NULL);
	pthread_mutex_init(&input->death_mutex, NULL);
	input->death_flag = 0;
	init_philos_forks(philos, input, &print_mutex);
	create_threads(input, threads, philos);
	mon.philos = philos;
	mon.input = input;
	mon.print_mutex = &print_mutex;
	pthread_create(&monitoring_thread, NULL, death_monitoring, &mon);
	join_threads(input, threads);
	pthread_join(monitoring_thread, NULL);
	destroy_all_mutex(input, philos, &print_mutex);
	free(threads);
	free(philos);
	return (1);
}

int	main(int argc, char **argv)
{
	t_input	input;

	if (!get_input(argc, argv, &input))
		return (1);
	printf("PHILO START\n");
	if (!init_info(&input))
		return (1);
	return (0);
}

//100 800 200 200
