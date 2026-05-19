#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_bank_context {
    int             shared_balance;
    pthread_mutex_t account_mutex;
    pthread_cond_t  turn_condition;
    pthread_cond_t  threshold_reached;
    int             ticket_counter;
    int             current_turn;
} t_bank_context;

typedef struct s_roommate {
    int             id;
    t_bank_context *ctx;
} t_roommate;

void *deposit_money(void *args) {
    t_roommate *roommate = (t_roommate *)args;
    t_bank_context *ctx = roommate->ctx;

    pthread_mutex_lock(&ctx->account_mutex);

    int my_ticket = ctx->ticket_counter;
    ctx->ticket_counter++;
    printf("Roommate %d took ticket %d\n", roommate->id, my_ticket);

    while (my_ticket != ctx->current_turn) {
        pthread_cond_wait(&ctx->turn_condition, &ctx->account_mutex);
    }

    printf("Roommate %d is at the terminal. Depositing...\n", roommate->id);
    
    int i = 0;
    while (i < 1000000) {
        ctx->shared_balance = ctx->shared_balance + 1;
        if (ctx->shared_balance == 2000000) {
            pthread_cond_signal(&ctx->threshold_reached);
        }
        i++;
    }

    ctx->current_turn++;
    pthread_cond_broadcast(&ctx->turn_condition);

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
    ctx.shared_balance = 0;
    ctx.ticket_counter = 0;
    ctx.current_turn = 0;

    pthread_mutex_init(&ctx.account_mutex, NULL);
    pthread_cond_init(&ctx.turn_condition, NULL);
    pthread_cond_init(&ctx.threshold_reached, NULL);

    t_roommate roommates[2];
    pthread_t threads[2];
    pthread_t landlord;

    pthread_create(&landlord, NULL, withdraw_rent, &ctx);

    int i = 0;
    while (i < 2) {
        roommates[i].id = i + 1;
        roommates[i].ctx = &ctx;
        pthread_create(&threads[i], NULL, deposit_money, &roommates[i]);
        usleep(1000);
        i++;
    }

    pthread_join(landlord, NULL);
    
    i = 0;
    while (i < 2) {
        pthread_join(threads[i], NULL);
        i++;
    }

    pthread_mutex_destroy(&ctx.account_mutex);
    pthread_cond_destroy(&ctx.turn_condition);
    pthread_cond_destroy(&ctx.threshold_reached);

    printf("Final balance: %d\n", ctx.shared_balance);
    return 0;
}