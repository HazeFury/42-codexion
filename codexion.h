/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:40:51 by marberge          #+#    #+#             */
/*   Updated: 2026/06/05 11:52:32 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}				t_scheduler;

typedef struct s_args
{
	size_t			number_of_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	size_t			number_of_compiles_required;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
}				t_args;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex; // Le verrou physique du dongle
	long long		available_at; // timestamp de quand le cooldown est fini
	void			*wait_queue; // Pointeur vers futur (Heap/Priority Queue)
}				t_dongle;

// FORWARD DECLARATION
typedef struct s_sim	t_sim;

typedef struct s_coder
{
	int				id;
	pthread_t		thread; // L'identifiant du thread POSIX
	t_dongle		*left_dongle; // Pointeur vers le dongle de gauche
	t_dongle		*right_dongle; // Pointeur vers le dongle de droite
	long long		last_compile_start; // Timestamp crucial to éviter burnout
	size_t			compiles_done; // Nombre de fois qu'il a compilé
	long long		request_time;
	pthread_mutex_t	state_mutex; // Protège accès à last_compile/compiles_done
	t_sim			*sim; // Lien vers le contexte global
}				t_coder;

typedef struct s_sim
{
	t_args			args; // Les paramètres qu'on a déjà parsés
	long long		start_time; // Le T0 de la simulation (calculer timestamps)
	int				is_running; // Flag (1 = en cours, 0 = terminé)
	pthread_mutex_t	sim_mutex; // Protège la variable is_running
	pthread_mutex_t	write_mutex; // Protège les printf logs ne se mélangent pas
	t_dongle		*dongles; // Tableau alloué dynamiquement pour les dongles
	t_coder			*coders; // Tableau alloué dynamiquement pour les codeurs
}				t_sim;

typedef struct s_heap
{
	t_coder			**array;
	size_t			size;
	size_t			capacity;
}				t_heap;

// ================================  PARSING  ================================
int			parse_arguments(int argc, char **argv, t_args *args);

// ================================ INIT =================================
int			init_simulation(t_sim *sim);

// ================================  CLEANUP  ================================
void		free_simulation(t_sim *sim);

// ===============================  HEAP UTILS  ==============================
int			is_more_vip(t_coder *c1, t_coder *c2, t_scheduler policy);
void		swap_coders(t_coder **a, t_coder **b);
void		sift_up(t_heap *heap, size_t idx, t_scheduler policy);
void		sift_down(t_heap *heap, size_t idx, t_scheduler policy);

// ================================ UTILS ================================
long long	get_time_in_ms(void);
void		print_state(t_coder *coder, const char *msg);
void		smart_sleep(long long time_to_sleep, t_sim *sim);

// ===============================  SCHEDULER  ===============================
int			init_heap(t_heap *heap, size_t capacity);
void		free_heap(t_heap *heap);
int			heap_push(t_heap *heap, t_coder *coder, t_scheduler policy);
t_coder		*heap_pop(t_heap *heap, t_scheduler policy);

// =======================  ROUTINE, DONGLES & MONITOR =======================
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			is_sim_running(t_coder *coder);
void		take_dongles(t_coder *coder);
void		drop_dongles(t_coder *coder);

// ================================  LAUNCHER  ================================
int			launch_simulation(t_sim *sim);

#endif