/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_info.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:39:23 by omizin            #+#    #+#             */
/*   Updated: 2025/06/25 12:17:09 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	init_philos_forks(t_philo *philos, t_input *input,
pthread_mutex_t *print_mutex)
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

static void	create_threads(t_input *input, pthread_t *threads, t_philo *philos)
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

static void	join_threads(t_input *input, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
		pthread_join(threads[i++], NULL);
}

static void	destroy_all_mutex(t_input *input, t_philo *philos,
pthread_mutex_t *print_mutex)
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
