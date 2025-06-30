/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:25:39 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 13:14:05 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_dead(t_philo *philo)
{
	int	flag;

	pthread_mutex_lock(&philo->input->death_mutex);
	flag = philo->input->death_flag;
	pthread_mutex_unlock(&philo->input->death_mutex);
	return (flag);
}

void	print_status(t_philo *philo, const char *msg)
{
	if (is_dead(philo))
		return ;
	pthread_mutex_lock(philo->print_mutex);
	if (!is_dead(philo))
		printf("time: %ld, id: %d %s\n",
			get_current_ms(philo->input), philo->id, msg);
	pthread_mutex_unlock(philo->print_mutex);
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
	if (s[0] == '+' && s[1] == '0' && (s[2] >= '0' && s[2] <= '9'))
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

int	allocate_memory(t_input *input, pthread_t **threads, t_philo **philos)
{
	*threads = malloc(sizeof(pthread_t) * input->philo_num);
	*philos = malloc(sizeof(t_philo) * input->philo_num);
	if (!*threads || !*philos)
		return (0);
	return (1);
}
