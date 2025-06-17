/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:58:25 by omizin            #+#    #+#             */
/*   Updated: 2025/06/17 12:13:19 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO
# define PHILO

# include "ft_printf.h"
# include "libft.h"
# include "get_next_line.h"

# include <sys/time.h>
# include <pthread.h>
# include <stdio.h>

typedef struct s_input
{
	int	philo_num;
	int	fork_num;
	int	die_time;
	int	eat_time;
	int	sleep_time;
	int	meal_num;
}	t_input;

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long			last_meal_time;
	t_input			*input;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*print_mutex;
}	t_philo;

#endif
