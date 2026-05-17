#include <pthread.h>
#include <stdio.h>

int shared_account_balance = 0;

// 2. Declare a global mutex to protect the resource
pthread_mutex_t account_mutex;

// 3. Create the routine function that each thread will execute
void deposit_money(void *arguments) {
	// Loop one million times to increment the balance
	int	i;
	i = 0;

	while (i < 1000000)
	{
		// --- CRITICAL SECTION START ---
		// Lock the mutex before touching the shared variable
		lock_mutex(&account_mutex);
		
		// Read, increment, and write the new balance
		shared_account_balance = shared_account_balance + 1;
		
		// Unlock the mutex so the other thread can access it
		unlock_mutex(&account_mutex);
		// --- CRITICAL SECTION END ---

	}
    return NULL;
}

// 4. Main program execution
int main()
{
	// Initialize the mutex with default attributes
    init_mutex(&account_mutex, NULL);
    
    // Declare thread variables
    pthread_t thread_1;
    pthread_t thread_2;
    
    // Create the threads and assign them the routine
    create_thread(&thread_1, NULL, deposit_money, NULL);
    create_thread(&thread_2, NULL, deposit_money, NULL);
    
    // Wait for both threads to finish their execution
    join_thread(thread_1, NULL);
    join_thread(thread_2, NULL);
    
    // Clean up memory by destroying the mutex
    destroy_mutex(&account_mutex);
    
    // Print the final result (Should be exactly 2,000,000)
    print("Final balance: ", shared_account_balance);
    
    return 0;
}