#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// 1. We define the context with our new scheduling variables
typedef struct s_bank_context {
    int             shared_balance;
    pthread_mutex_t account_mutex;
    
    // Condition variable for the turn-based queue (the roommates)
    pthread_cond_t  turn_condition;
    // Condition variable strictly for the landlord
    pthread_cond_t  threshold_reached;
    
    // The ticket dispenser (increments every time a thread arrives)
    int             ticket_counter;
    // The display board showing whose turn it is
    int             current_turn;
} t_bank_context;

// Structure to pass individual ID and shared context to each roommate
typedef struct s_roommate {
    int             id;
    t_bank_context *ctx;
} t_roommate;

void *deposit_money(void *args) {
    t_roommate *roommate = (t_roommate *)args;
    t_bank_context *ctx = roommate->ctx;

    // Lock the mutex as soon as we arrive
    pthread_mutex_lock(&ctx->account_mutex);

    // --- SCHEDULER LOGIC: Take a ticket ---
    int my_ticket = ctx->ticket_counter;
    ctx->ticket_counter++;
    printf("Roommate %d took ticket %d\n", roommate->id, my_ticket);

    // Wait in the condition room as long as it's not our turn
    while (my_ticket != ctx->current_turn) {
        pthread_cond_wait(&ctx->turn_condition, &ctx->account_mutex);
    }
    // --------------------------------------

    printf("Roommate %d is at the terminal. Depositing...\n", roommate->id);
    
	printf("Final balance: %d\n", ctx->shared_balance);
    // Do the actual work (deposit money)
    int i = 0;
    while (i < 1000000) {
        ctx->shared_balance = ctx->shared_balance + 1;
        
        // We use a SIGNAL here because only ONE landlord needs to wake up
        if (ctx->shared_balance == 2000000) {
            pthread_cond_signal(&ctx->threshold_reached);
        }
        i++;
    }

    // --- SCHEDULER LOGIC: Pass the turn ---
    ctx->current_turn++;
    
    // We use BROADCAST here because we need ALL roommates to wake up
    // so they can check their tickets and see who is next.
    pthread_cond_broadcast(&ctx->turn_condition);
    // --------------------------------------

    pthread_mutex_unlock(&ctx->account_mutex);
    return NULL;
}

void *withdraw_rent(void *args) {
    t_bank_context *ctx = (t_bank_context *)args;

    pthread_mutex_lock(&ctx->account_mutex);

    while (ctx->shared_balance < 2000000) {
        pthread_cond_wait(&ctx->threshold_reached, &ctx->account_mutex);
    }

    printf("Landlord: The rent is ready (%d). Withdrawing...\n", ctx->shared_balance);
    ctx->shared_balance = 0;

    pthread_mutex_unlock(&ctx->account_mutex);
    return NULL;
}

int main(void) {
    t_bank_context ctx;
    
    // Initialize shared variables and our scheduler's starting state
    ctx.shared_balance = 0;
    ctx.ticket_counter = 0;
    ctx.current_turn = 0;

    // Initialize mutex and both condition variables
    pthread_mutex_init(&ctx.account_mutex, NULL);
    pthread_cond_init(&ctx.turn_condition, NULL);
    pthread_cond_init(&ctx.threshold_reached, NULL);

    t_roommate roommates[2];
    pthread_t threads[2];
    pthread_t landlord;

    // Start the landlord thread first
    pthread_create(&landlord, NULL, withdraw_rent, &ctx);

    // Start the roommate threads with a slight delay to ensure they arrive in order
    int i = 0;
    while (i < 2) {
        roommates[i].id = i + 1;
        roommates[i].ctx = &ctx;
        pthread_create(&threads[i], NULL, deposit_money, &roommates[i]);
        usleep(1000);
        i++;
    }

    // Wait for all threads to complete their execution
    pthread_join(landlord, NULL);
    
    i = 0;
    while (i < 2) {
        pthread_join(threads[i], NULL);
        i++;
    }

    // Clean up all initialized POSIX objects
    pthread_mutex_destroy(&ctx.account_mutex);
    pthread_cond_destroy(&ctx.turn_condition);
    pthread_cond_destroy(&ctx.threshold_reached);

    printf("Final balance: %d\n", ctx.shared_balance);
    return 0;
}