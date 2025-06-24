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
	else
		input->meal_num = -1;
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

void	*philo_loop(void *arg)
{
	t_philo	*philo = (t_philo *)arg;

	if (philo->id % 2 != 0)
		usleep(500);
	while (!is_dead(philo))
	{
		print_status(philo, "is thinking");
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->fork_right);
			pthread_mutex_lock(&philo->fork_left);
		}
		else
		{
			pthread_mutex_lock(&philo->fork_left);
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
			break;

		print_status(philo, "is sleeping");

		smart_sleep(philo->input->sleep_time, philo);
	}
	return (NULL);
}

void	*death_monitoring(void *arg)
{
	t_monitoring	*mon;
	t_philo			*p;
	int				i;
	int				full_count;
	long			time_since_meal;

	mon = (t_monitoring *)arg;
	while (1)
	{
		full_count = 0;
		i = 0;
		while (i < mon->input->philo_num)
		{
			p = &mon->philos[i];

			pthread_mutex_lock(&p->meal_mutex);
			time_since_meal = get_current_ms(mon->input) - p->last_meal_time;
			if (mon->input->meal_num != -1 && p->meals_eaten >= mon->input->meal_num)
				full_count++;
			pthread_mutex_unlock(&p->meal_mutex);

			if (time_since_meal >= mon->input->die_time + CHECK_DELAY)
			{
				pthread_mutex_lock(mon->print_mutex);
				printf("time: %ld, id: %d died\n", get_current_ms(mon->input), p->id);
				pthread_mutex_unlock(mon->print_mutex);

				pthread_mutex_lock(&mon->input->death_mutex);
				mon->input->death_flag = 1;
				pthread_mutex_unlock(&mon->input->death_mutex);

				return (NULL);
			}
			i++;
		}
		if (mon->input->meal_num != -1 && full_count == mon->input->philo_num)
		{
			pthread_mutex_lock(mon->print_mutex);
			printf("All philosophers have eaten %d times. Exiting.\n", mon->input->meal_num);
			pthread_mutex_unlock(mon->print_mutex);

			pthread_mutex_lock(&mon->input->death_mutex);
			mon->input->death_flag = 1;
			pthread_mutex_unlock(&mon->input->death_mutex);
			return (NULL);
		}
		usleep(100);
	}
	return (NULL);
}

int	init_philos(t_input *input)
{
	pthread_t		*threads;
	t_philo			*philos;
	t_monitoring	mon;
	pthread_mutex_t	print_mutex;
	pthread_t		monitoring_thread;
	int				i;

	threads = malloc(sizeof(pthread_t) * input->philo_num);
	philos = malloc(sizeof(t_philo) * input->philo_num);
	if (!threads || !philos)
		return (0);

	pthread_mutex_init(&print_mutex, NULL);
	pthread_mutex_init(&input->death_mutex, NULL);
	input->death_flag = 0;

	i = 0;
	while (i < input->philo_num)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].input = input;
		philos[i].print_mutex = &print_mutex;
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

	i = 0;
	while (i < input->philo_num)
	{
		pthread_create(&threads[i], NULL, philo_loop, &philos[i]);
		usleep(100);
		i++;
	}

	mon.philos = philos;
	mon.input = input;
	mon.print_mutex = &print_mutex;
	pthread_create(&monitoring_thread, NULL, death_monitoring, &mon);

	i = 0;
	while (i < input->philo_num)
		pthread_join(threads[i++], NULL);

	pthread_join(monitoring_thread, NULL);

	i = 0;
	while (i < input->philo_num)
	{
		pthread_mutex_destroy(&philos[i].fork_left);
		pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&print_mutex);
	pthread_mutex_destroy(&input->death_mutex);

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
	if (!init_philos(&input))
		return (1);
	return (0);
}
//100 800 200 200

// ==88== HEAP SUMMARY:
// ==88==     in use at exit: 1,632 bytes in 3 blocks
// ==88==   total heap usage: 14 allocs, 11 frees, 5,376 bytes allocated
// ==88==
// ==88== LEAK SUMMARY:
// ==88==    definitely lost: 1,360 bytes in 2 blocks
// ==88==    indirectly lost: 0 bytes in 0 blocks
// ==88==      possibly lost: 272 bytes in 1 blocks
// ==88==    still reachable: 0 bytes in 0 blocks
// ==88==         suppressed: 0 bytes in 0 blocks
// ==88== Rerun with --leak-check=full to see details of leaked memory
// ==88==
// ==88== For lists of detected and suppressed errors, rerun with: -s
// ==88== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)