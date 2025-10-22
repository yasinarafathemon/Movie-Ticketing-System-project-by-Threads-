/*
================================================================================
                        MOVIE TICKET BOOKING SYSTEM
                        Star Cineplex - Bangladesh
================================================================================

PROJECT DESCRIPTION:
This program simulates an online movie ticket booking system using POSIX threads,
mutex locks, and semaphores to handle concurrent user requests safely.

REQUIREMENTS IMPLEMENTED:
✓ POSIX threads for concurrent user simulation
✓ Mutex locks for protecting shared ticket data
✓ Semaphores for controlling concurrent access
✓ Dynamic memory allocation based on command-line arguments
✓ Thread-safe ticket booking without data loss
✓ Proper resource cleanup and thread termination

COMPILATION: gcc -o movie_booking movie_ticket_booking.c -pthread
USAGE: ./movie_booking <num_users> <num_tickets> <num_shows>
EXAMPLE: ./movie_booking 10 5 3

================================================================================
*/

#include <stdio.h>          // For input/output operations (printf, fprintf)
#include <stdlib.h>         // For memory allocation (malloc, free) and exit functions
#include <pthread.h>        // For POSIX threads (pthread_create, pthread_join, pthread_mutex)
#include <semaphore.h>      // For semaphore operations (sem_init, sem_wait, sem_post)
#include <unistd.h>         // For usleep() function to add delays
#include <string.h>         // For string operations
#include <time.h>           // For random number generation seeding

/*
================================================================================
                            DATA STRUCTURES
================================================================================
*/

/*
 * SHOW STRUCTURE
 * Represents a single movie show with its tickets and synchronization
 * This is the SHARED DATA that multiple threads will access concurrently
 */
typedef struct {
    int show_id;                    // Unique identifier for this show (1, 2, 3, ...)
    int available_tickets;          // Current number of tickets available (CRITICAL SHARED DATA)
    int initial_tickets;            // Original number of tickets (for display purposes)
    pthread_mutex_t mutex;          // Mutex lock for this specific show
                                   // Each show has its own mutex to allow concurrent 
                                   // bookings for different shows while protecting same show data
} Show;

/*
 * USER THREAD DATA STRUCTURE
 * Contains all information a user thread needs to operate
 * Passed as parameter to each thread function
 */
typedef struct {
    int user_id;                    // Unique identifier for this user thread
    Show *shows;                    // Pointer to array of all shows (shared data)
    int num_shows;                  // Total number of shows available
    sem_t *semaphore;              // Pointer to the global semaphore
} UserData;

/*
================================================================================
                            GLOBAL VARIABLES
================================================================================
*/

Show *shows = NULL;                 // Dynamic array of all movie shows
int total_users = 0;                // Total number of user threads to create
int total_tickets_per_show = 0;     // Number of tickets per show
int total_shows = 0;                // Total number of shows
sem_t global_semaphore;             // Semaphore to control concurrent access

/*
================================================================================
                         UTILITY FUNCTIONS
================================================================================
*/

/*
 * FUNCTION: print_header
 * PURPOSE: Display program startup information
 * PARAMETERS: None
 * RETURNS: void
 */
void print_header() {
    printf("\n");
    printf("================================================================\n");
    printf("              STAR CINEPLEX TICKET BOOKING SYSTEM              \n");
    printf("                    Concurrent Booking Simulation              \n");
    printf("================================================================\n");
    printf("System Configuration:\n");
    printf("- Total Users: %d\n", total_users);
    printf("- Tickets per Show: %d\n", total_tickets_per_show);
    printf("- Number of Shows: %d\n", total_shows);
    printf("- Concurrent Booking Limit: 3 users at a time\n");
    printf("================================================================\n\n");
}

/*
 * FUNCTION: initialize_shows
 * PURPOSE: Dynamically allocate and initialize all movie shows
 * PARAMETERS: 
 *   - num_shows: Number of shows to create
 *   - tickets_per_show: Initial tickets for each show
 * RETURNS: void
 * 
 * MEMORY MANAGEMENT: Uses malloc() to allocate show array dynamically
 * SYNCHRONIZATION: Initializes mutex for each show
 */
void initialize_shows(int num_shows, int tickets_per_show) {
    printf("🎬 Initializing %d shows with %d tickets each...\n", num_shows, tickets_per_show);

    // DYNAMIC MEMORY ALLOCATION
    // Allocate memory for array of Show structures
    shows = (Show *)malloc(num_shows * sizeof(Show));

    // ERROR HANDLING: Check if memory allocation failed
    if (shows == NULL) {
        fprintf(stderr, "❌ CRITICAL ERROR: Memory allocation failed for shows\n");
        fprintf(stderr, "   Requested: %d shows (%zu bytes)\n", 
                num_shows, num_shows * sizeof(Show));
        exit(EXIT_FAILURE);
    }

    // Initialize each show
    for (int i = 0; i < num_shows; i++) {
        shows[i].show_id = i + 1;                    // Show IDs start from 1
        shows[i].available_tickets = tickets_per_show; // Set initial ticket count
        shows[i].initial_tickets = tickets_per_show;   // Remember original count

        // MUTEX INITIALIZATION
        // Each show gets its own mutex for fine-grained locking
        int mutex_result = pthread_mutex_init(&shows[i].mutex, NULL);
        if (mutex_result != 0) {
            fprintf(stderr, "❌ ERROR: Failed to initialize mutex for Show %d\n", i + 1);
            // Clean up already initialized mutexes
            for (int j = 0; j < i; j++) {
                pthread_mutex_destroy(&shows[j].mutex);
            }
            free(shows);
            exit(EXIT_FAILURE);
        }

        printf("   ✓ Show %d: %d tickets available\n", shows[i].show_id, shows[i].available_tickets);
    }

    printf("✅ All shows initialized successfully!\n\n");
}

/*
 * FUNCTION: display_show_status
 * PURPOSE: Display current status of all shows (used during execution)
 * PARAMETERS: 
 *   - user_id: ID of the user requesting status (for logging)
 * RETURNS: void
 */
void display_show_status(int user_id) {
    printf("\n📊 [User %d] Current Show Status:\n", user_id);
    printf("┌─────────┬──────────────┬─────────────┐\n");
    printf("│ Show ID │ Available    │ Booked      │\n");
    printf("├─────────┼──────────────┼─────────────┤\n");

    for (int i = 0; i < total_shows; i++) {
        int booked = shows[i].initial_tickets - shows[i].available_tickets;
        printf("│   %2d    │     %2d       │     %2d      │\n", 
               shows[i].show_id, shows[i].available_tickets, booked);
    }

    printf("└─────────┴──────────────┴─────────────┘\n\n");
}

/*
================================================================================
                         CORE BOOKING FUNCTION
================================================================================
*/

/*
 * FUNCTION: book_ticket
 * PURPOSE: Main thread function that simulates a user booking a ticket
 * PARAMETERS: 
 *   - arg: Pointer to UserData structure containing thread information
 * RETURNS: NULL (as required by pthread function signature)
 * 
 * THREAD SAFETY: Uses semaphore and mutex for synchronization
 * CRITICAL SECTION: The ticket checking and decrementing operation
 */
void* book_ticket(void *arg) {
    // CAST the void pointer back to UserData pointer
    UserData *user_data = (UserData *)arg;
    int user_id = user_data->user_id;

    printf("🧑‍💻 User %d: Starting booking process...\n", user_id);

    // RANDOM SHOW SELECTION
    // Simulate user choosing a show randomly
    int selected_show_index = rand() % user_data->num_shows;
    int selected_show_id = shows[selected_show_index].show_id;

    printf("🎯 User %d: Selected Show %d for booking\n", user_id, selected_show_id);

    /*
    ============================================================================
                              SYNCHRONIZATION BEGINS
    ============================================================================
    */

    // STEP 1: SEMAPHORE WAIT
    // This controls the overall number of concurrent bookings
    // If 3 users are already booking, this user will wait here
    printf("⏳ User %d: Waiting for booking slot (semaphore)...\n", user_id);
    sem_wait(user_data->semaphore);
    printf("✅ User %d: Got booking slot! Proceeding to book Show %d\n", user_id, selected_show_id);

    // STEP 2: MUTEX LOCK
    // Lock the specific show's mutex to enter critical section
    // Only one thread can modify this show's data at a time
    printf("🔒 User %d: Locking Show %d mutex...\n", user_id, selected_show_id);
    pthread_mutex_lock(&shows[selected_show_index].mutex);
    printf("✅ User %d: Acquired Show %d mutex lock\n", user_id, selected_show_id);

    /*
    ============================================================================
                              CRITICAL SECTION
    ============================================================================
    This is the most important part - only one thread can execute this
    section for each show at any given time. This prevents race conditions.
    */

    // STEP 3: CHECK TICKET AVAILABILITY AND BOOK
    printf("🔍 User %d: Checking ticket availability for Show %d...\n", user_id, selected_show_id);

    if (shows[selected_show_index].available_tickets > 0) {
        // TICKETS AVAILABLE - PROCEED WITH BOOKING

        // Display current status before booking
        printf("✅ User %d: Found %d tickets available for Show %d\n", 
               user_id, shows[selected_show_index].available_tickets, selected_show_id);

        // SIMULATE BOOKING PROCESS TIME
        // In real system, this would be database operations, payment processing, etc.
        printf("💳 User %d: Processing booking for Show %d...\n", user_id, selected_show_id);
        usleep(100000); // 0.1 second delay to simulate booking time

        // DECREMENT TICKET COUNT (THE CRITICAL OPERATION)
        shows[selected_show_index].available_tickets--;

        // BOOKING SUCCESS
        printf("🎉 User %d: ✅ BOOKING SUCCESSFUL for Show %d!\n", user_id, selected_show_id);
        printf("📊 User %d: Show %d now has %d tickets remaining\n", 
               user_id, selected_show_id, shows[selected_show_index].available_tickets);

    } else {
        // NO TICKETS AVAILABLE
        printf("❌ User %d: 😞 SOLD OUT! Show %d has no tickets available\n", 
               user_id, selected_show_id);
        printf("💔 User %d: Better luck next time!\n", user_id);
    }

    /*
    ============================================================================
                         END OF CRITICAL SECTION
    ============================================================================
    */

    // STEP 4: MUTEX UNLOCK
    // Release the show's mutex lock
    printf("🔓 User %d: Releasing Show %d mutex lock...\n", user_id, selected_show_id);
    pthread_mutex_unlock(&shows[selected_show_index].mutex);
    printf("✅ User %d: Show %d mutex released\n", user_id, selected_show_id);

    // STEP 5: SEMAPHORE POST
    // Signal the semaphore to allow another user to start booking
    printf("📢 User %d: Releasing booking slot (semaphore)...\n", user_id);
    sem_post(user_data->semaphore);
    printf("✅ User %d: Booking slot released for next user\n", user_id);

    /*
    ============================================================================
                           SYNCHRONIZATION COMPLETE
    ============================================================================
    */

    printf("👋 User %d: Booking process completed. Thread terminating.\n\n", user_id);

    // MEMORY CLEANUP
    // Free the UserData structure that was allocated for this thread
    free(user_data);

    // THREAD TERMINATION
    // Properly exit the thread
    pthread_exit(NULL);
}

/*
================================================================================
                         CLEANUP FUNCTIONS
================================================================================
*/

/*
 * FUNCTION: cleanup_resources
 * PURPOSE: Clean up all allocated resources and synchronization primitives
 * PARAMETERS: None
 * RETURNS: void
 * 
 * IMPORTANT: This prevents memory leaks and properly destroys synchronization objects
 */
void cleanup_resources() {
    printf("🧹 Cleaning up system resources...\n");

    if (shows != NULL) {
        // DESTROY ALL MUTEXES
        // Each show's mutex must be properly destroyed
        for (int i = 0; i < total_shows; i++) {
            printf("   🗑️  Destroying mutex for Show %d\n", shows[i].show_id);
            pthread_mutex_destroy(&shows[i].mutex);
        }

        // FREE DYNAMIC MEMORY
        printf("   🗑️  Freeing shows memory\n");
        free(shows);
        shows = NULL;
    }

    // DESTROY SEMAPHORE
    printf("   🗑️  Destroying semaphore\n");
    sem_destroy(&global_semaphore);

    printf("✅ All resources cleaned up successfully!\n");
}

/*
 * FUNCTION: display_final_status
 * PURPOSE: Show the final booking results for all shows
 * PARAMETERS: None
 * RETURNS: void
 */
void display_final_status() {
    printf("\n");
    printf("================================================================\n");
    printf("                    FINAL BOOKING REPORT                       \n");
    printf("================================================================\n");

    int total_initial_tickets = 0;
    int total_remaining_tickets = 0;
    int total_booked_tickets = 0;

    printf("┌─────────┬─────────────┬─────────────┬──────────────┐\n");
    printf("│ Show ID │ Initial     │ Remaining   │ Booked       │\n");
    printf("├─────────┼─────────────┼─────────────┼──────────────┤\n");

    for (int i = 0; i < total_shows; i++) {
        int booked = shows[i].initial_tickets - shows[i].available_tickets;

        printf("│   %2d    │     %2d      │     %2d      │     %2d       │\n",
               shows[i].show_id, 
               shows[i].initial_tickets,
               shows[i].available_tickets, 
               booked);

        total_initial_tickets += shows[i].initial_tickets;
        total_remaining_tickets += shows[i].available_tickets;
        total_booked_tickets += booked;
    }

    printf("├─────────┼─────────────┼─────────────┼──────────────┤\n");
    printf("│ TOTAL   │     %2d      │     %2d      │     %2d       │\n",
           total_initial_tickets, total_remaining_tickets, total_booked_tickets);
    printf("└─────────┴─────────────┴─────────────┴──────────────┘\n");

    // BOOKING STATISTICS
    printf("\n📈 BOOKING STATISTICS:\n");
    printf("   👥 Total Users: %d\n", total_users);
    printf("   🎫 Total Tickets Available: %d\n", total_initial_tickets);
    printf("   ✅ Total Tickets Booked: %d\n", total_booked_tickets);
    printf("   📊 Booking Success Rate: %.1f%%\n", 
           (total_booked_tickets * 100.0) / total_users);

    if (total_remaining_tickets == 0) {
        printf("   🎉 ALL SHOWS SOLD OUT!\n");
    }

    printf("================================================================\n");
}

/*
================================================================================
                            MAIN FUNCTION
================================================================================
*/

int main(int argc, char *argv[]) {
    printf("🚀 Starting Star Cineplex Booking System...\n\n");

    /*
    ============================================================================
                           COMMAND LINE ARGUMENT PARSING
    ============================================================================
    */

    // CHECK ARGUMENT COUNT
    // Program expects: ./program_name <users> <tickets> <shows>
    if (argc != 4) {
        fprintf(stderr, "❌ USAGE ERROR: Incorrect number of arguments\n");
        fprintf(stderr, "\n📋 CORRECT USAGE:\n");
        fprintf(stderr, "   %s <num_users> <num_tickets> <num_shows>\n\n", argv[0]);
        fprintf(stderr, "📝 EXAMPLES:\n");
        fprintf(stderr, "   %s 5 10 2    # 5 users, 10 tickets per show, 2 shows\n", argv[0]);
        fprintf(stderr, "   %s 20 15 3   # 20 users, 15 tickets per show, 3 shows\n", argv[0]);
        fprintf(stderr, "   %s 50 25 5   # 50 users, 25 tickets per show, 5 shows\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // PARSE ARGUMENTS
    total_users = atoi(argv[1]);                    // Convert string to integer
    total_tickets_per_show = atoi(argv[2]);
    total_shows = atoi(argv[3]);

    // VALIDATE ARGUMENTS
    // All arguments must be positive integers
    if (total_users <= 0 || total_tickets_per_show <= 0 || total_shows <= 0) {
        fprintf(stderr, "❌ VALIDATION ERROR: All arguments must be positive integers\n");
        fprintf(stderr, "   Users: %d (must be > 0)\n", total_users);
        fprintf(stderr, "   Tickets per show: %d (must be > 0)\n", total_tickets_per_show);
        fprintf(stderr, "   Number of shows: %d (must be > 0)\n", total_shows);
        exit(EXIT_FAILURE);
    }

    // REASONABLE LIMITS CHECK (optional)
    if (total_users > 1000) {
        printf("⚠️  WARNING: Large number of users (%d) may cause performance issues\n", total_users);
    }

    /*
    ============================================================================
                              SYSTEM INITIALIZATION
    ============================================================================
    */

    // DISPLAY SYSTEM INFORMATION
    print_header();

    // SEED RANDOM NUMBER GENERATOR
    // This ensures different random show selections each time
    srand(time(NULL));

    // INITIALIZE SEMAPHORE
    // Limit concurrent bookings to 3 users at a time
    int concurrent_limit = 3;  // You can adjust this value
    printf("🔧 Initializing semaphore with limit of %d concurrent bookings...\n", concurrent_limit);

    int sem_result = sem_init(&global_semaphore, 0, concurrent_limit);
    if (sem_result != 0) {
        fprintf(stderr, "❌ CRITICAL ERROR: Failed to initialize semaphore\n");
        exit(EXIT_FAILURE);
    }
    printf("✅ Semaphore initialized successfully!\n\n");

    // INITIALIZE SHOWS
    initialize_shows(total_shows, total_tickets_per_show);

    /*
    ============================================================================
                              THREAD CREATION
    ============================================================================
    */

    printf("👥 Creating %d user threads...\n", total_users);

    // ALLOCATE MEMORY FOR THREAD IDs
    pthread_t *threads = (pthread_t *)malloc(total_users * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "❌ CRITICAL ERROR: Memory allocation failed for thread IDs\n");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // CREATE USER THREADS
    for (int i = 0; i < total_users; i++) {
        // ALLOCATE MEMORY FOR THREAD DATA
        // Each thread gets its own UserData structure
        UserData *user_data = (UserData *)malloc(sizeof(UserData));
        if (user_data == NULL) {
            fprintf(stderr, "❌ ERROR: Memory allocation failed for User %d data\n", i + 1);
            // Continue with other threads rather than failing completely
            continue;
        }

        // INITIALIZE THREAD DATA
        user_data->user_id = i + 1;                    // User IDs start from 1
        user_data->shows = shows;                      // Pointer to shared show array
        user_data->num_shows = total_shows;            // Number of shows available
        user_data->semaphore = &global_semaphore;      // Pointer to global semaphore

        // CREATE PTHREAD
        int thread_result = pthread_create(&threads[i], NULL, book_ticket, (void *)user_data);

        if (thread_result != 0) {
            fprintf(stderr, "❌ ERROR: Failed to create thread for User %d\n", i + 1);
            free(user_data);  // Clean up allocated memory
            continue;
        }

        printf("   ✅ User %d thread created successfully\n", i + 1);

        // SMALL DELAY BETWEEN THREAD CREATION
        // This simulates users arriving at slightly different times
        usleep(50000); // 0.05 second delay
    }

    printf("\n🎬 All user threads created! Booking simulation started...\n");
    printf("================================================================\n\n");

    /*
    ============================================================================
                              THREAD SYNCHRONIZATION
    ============================================================================
    */

    printf("⏳ Main thread waiting for all users to complete booking...\n\n");

    // WAIT FOR ALL THREADS TO COMPLETE
    // pthread_join() blocks until the specified thread terminates
    for (int i = 0; i < total_users; i++) {
        int join_result = pthread_join(threads[i], NULL);
        if (join_result != 0) {
            fprintf(stderr, "⚠️  WARNING: Failed to join thread for User %d\n", i + 1);
        } else {
            printf("✅ User %d thread completed successfully\n", i + 1);
        }
    }

    printf("\n🎉 All user threads have completed!\n");

    /*
    ============================================================================
                              PROGRAM TERMINATION
    ============================================================================
    */

    // DISPLAY FINAL RESULTS
    display_final_status();

    // CLEANUP RESOURCES
    free(threads);  // Free thread ID array
    cleanup_resources();  // Clean up shows, mutexes, and semaphore

    printf("\n🏁 Star Cineplex Booking System terminated successfully!\n");
    printf("   Thank you for using our ticket booking system!\n\n");

    return 0;  // Successful program termination
}

/*
================================================================================
                              END OF PROGRAM
================================================================================

SYNCHRONIZATION SUMMARY:
========================

1. SEMAPHORE (global_semaphore):
   - Purpose: Controls the maximum number of concurrent bookings (set to 3)
   - Operations: sem_wait() to acquire slot, sem_post() to release slot
   - Benefit: Prevents system overload during peak usage

2. MUTEX LOCKS (one per show):
   - Purpose: Protects the ticket count of each individual show
   - Operations: pthread_mutex_lock() to acquire, pthread_mutex_unlock() to release
   - Benefit: Prevents race conditions on shared ticket data

3. THREAD SYNCHRONIZATION:
   - pthread_create(): Creates user threads
   - pthread_join(): Waits for thread completion
   - pthread_exit(): Properly terminates threads

CRITICAL SECTION:
================
The critical section is the code that checks ticket availability and decrements
the counter. This MUST be protected by mutex to prevent:
- Race conditions (multiple threads reading same value)
- Data corruption (negative ticket counts)
- Overbooking (selling more tickets than available)

MEMORY MANAGEMENT:
==================
- Dynamic allocation: malloc() for shows, threads, and user data
- Proper cleanup: free() for all allocated memory
- Resource destruction: pthread_mutex_destroy(), sem_destroy()

ERROR HANDLING:
===============
- Command-line argument validation
- Memory allocation failure checks
- Thread creation error handling
- Synchronization primitive initialization checks

================================================================================
*/
