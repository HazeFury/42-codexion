#include <pthread.h>
#include <stdio.h>

// 1. Define a context structure to hold all shared resources
typedef struct s_bank_context {
    int             shared_balance;
} t_bank_context;

// 2. The routine function
void *deposit_money(void *args) {
    // Cast the void pointer back to our context structure pointer
    t_bank_context *ctx = (t_bank_context *)args;
    int i = 0;

    while (i < 1000000) {
        // Read, increment, and write the new balance
        ctx->shared_balance = ctx->shared_balance + 1;
        i++;
    }
    return NULL;
}

// 3. Main program execution
int main(void) {
    // Declare the context on the main stack
    t_bank_context ctx;
    
    // Initialize our shared variables properly
    ctx.shared_balance = 0;
    
    // Declare thread variables
    pthread_t thread_1;
    pthread_t thread_2;
    
    // Create threads, passing the address of our context structure
    pthread_create(&thread_1, NULL, deposit_money, &ctx);
    pthread_create(&thread_2, NULL, deposit_money, &ctx);
    
    // Wait for both threads to finish
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    
    // Print the final result
    printf("Final balance: %d\n", ctx.shared_balance);
    
    return 0;
}