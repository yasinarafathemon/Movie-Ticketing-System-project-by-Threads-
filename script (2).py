# Create a detailed step-by-step execution guide

execution_guide = '''# 🚀 Movie Ticket Booking System - Complete Execution Guide

## 📋 Table of Contents
1. [Prerequisites](#prerequisites)
2. [Compilation](#compilation)
3. [Basic Execution](#basic-execution)
4. [Understanding the Output](#understanding-the-output)
5. [Test Scenarios](#test-scenarios)
6. [Troubleshooting](#troubleshooting)
7. [Code Walkthrough](#code-walkthrough)

## 🔧 Prerequisites

### System Requirements:
- **Operating System**: Linux, macOS, or WSL (Windows Subsystem for Linux)
- **Compiler**: GCC with pthread support
- **Memory**: Sufficient RAM for thread creation (typically 8KB per thread)

### Check if you have required tools:
```bash
# Check GCC installation
gcc --version

# Check pthread library availability
gcc -pthread --help | grep pthread
```

## 🔨 Compilation

### Method 1: Using the provided Makefile (Recommended)
```bash
# Compile the program
make

# Or compile debug version
make debug

# View all available options
make help
```

### Method 2: Manual compilation
```bash
# Basic compilation
gcc -o movie_booking movie_ticket_booking_complete.c -pthread

# With all warnings enabled (recommended)
gcc -Wall -Wextra -o movie_booking movie_ticket_booking_complete.c -pthread

# Debug version with symbols
gcc -Wall -Wextra -g -o movie_booking_debug movie_ticket_booking_complete.c -pthread
```

## 🎮 Basic Execution

### Command Syntax:
```bash
./movie_booking <number_of_users> <tickets_per_show> <number_of_shows>
```

### Parameter Explanation:
- **number_of_users**: Total user threads to create (1-1000 recommended)
- **tickets_per_show**: Initial tickets available for each show (1-100 recommended)
- **number_of_shows**: Total number of movie shows (1-20 recommended)

### Simple Examples:
```bash
# Small test: 3 users competing for 5 tickets in 1 show
./movie_booking 3 5 1

# Medium test: 10 users, 15 tickets per show, 2 shows (30 total tickets)
./movie_booking 10 15 2

# Realistic scenario: 25 users, 20 tickets per show, 3 shows
./movie_booking 25 20 3
```

## 📊 Understanding the Output

### 1. System Initialization
```
🚀 Starting Star Cineplex Booking System...

================================================================
              STAR CINEPLEX TICKET BOOKING SYSTEM              
                    Concurrent Booking Simulation              
================================================================
System Configuration:
- Total Users: 10
- Tickets per Show: 5
- Number of Shows: 2
- Concurrent Booking Limit: 3 users at a time
================================================================
```

### 2. Show Initialization
```
🎬 Initializing 2 shows with 5 tickets each...
   ✓ Show 1: 5 tickets available
   ✓ Show 2: 5 tickets available
✅ All shows initialized successfully!
```

### 3. Thread Creation
```
👥 Creating 10 user threads...
   ✅ User 1 thread created successfully
   ✅ User 2 thread created successfully
   ...
```

### 4. Booking Process (Real-time)
```
🧑‍💻 User 1: Starting booking process...
🎯 User 1: Selected Show 2 for booking
⏳ User 1: Waiting for booking slot (semaphore)...
✅ User 1: Got booking slot! Proceeding to book Show 2
🔒 User 1: Locking Show 2 mutex...
✅ User 1: Acquired Show 2 mutex lock
🔍 User 1: Checking ticket availability for Show 2...
✅ User 1: Found 5 tickets available for Show 2
💳 User 1: Processing booking for Show 2...
🎉 User 1: ✅ BOOKING SUCCESSFUL for Show 2!
📊 User 1: Show 2 now has 4 tickets remaining
🔓 User 1: Releasing Show 2 mutex lock...
✅ User 1: Show 2 mutex released
📢 User 1: Releasing booking slot (semaphore)...
✅ User 1: Booking slot released for next user
👋 User 1: Booking process completed. Thread terminating.
```

### 5. Final Report
```
================================================================
                    FINAL BOOKING REPORT                       
================================================================
┌─────────┬─────────────┬─────────────┬──────────────┐
│ Show ID │ Initial     │ Remaining   │ Booked       │
├─────────┼─────────────┼─────────────┼──────────────┤
│    1    │      5      │      2      │      3       │
│    2    │      5      │      1      │      4       │
├─────────┼─────────────┼─────────────┼──────────────┤
│ TOTAL   │     10      │      3      │      7       │
└─────────┴─────────────┴─────────────┴──────────────┘

📈 BOOKING STATISTICS:
   👥 Total Users: 10
   🎫 Total Tickets Available: 10
   ✅ Total Tickets Booked: 7
   📊 Booking Success Rate: 70.0%
================================================================
```

## 🧪 Test Scenarios

### Test 1: Basic Functionality
```bash
# 5 users, enough tickets for all
./movie_booking 5 10 1
# Expected: All 5 users should successfully book tickets
```

### Test 2: Overbooking Prevention
```bash
# More users than tickets
./movie_booking 10 3 1
# Expected: First 3 users succeed, remaining 7 see "SOLD OUT"
```

### Test 3: Multiple Shows
```bash
# Users distributed across multiple shows
./movie_booking 15 8 3
# Expected: Users randomly select shows, total 24 tickets available
```

### Test 4: Concurrency Stress Test
```bash
# High concurrency scenario
./movie_booking 50 15 5
# Expected: Heavy contention, but no race conditions or crashes
```

### Test 5: Edge Cases
```bash
# Single user, single ticket
./movie_booking 1 1 1

# Many shows, few tickets each
./movie_booking 20 2 10

# Equal users and tickets
./movie_booking 25 25 1
```

## 🔧 Troubleshooting

### Common Issues and Solutions:

#### 1. Compilation Errors
```bash
# Error: pthread library not found
# Solution: Install build-essential package
sudo apt-get install build-essential

# Error: undefined reference to pthread functions
# Solution: Ensure -pthread flag is used
gcc -o movie_booking movie_ticket_booking_complete.c -pthread
```

#### 2. Runtime Errors
```bash
# Error: "Permission denied"
# Solution: Make file executable
chmod +x movie_booking

# Error: "Segmentation fault"
# Solution: Check input parameters are positive integers
./movie_booking 10 5 3  # ✓ Good
./movie_booking -5 0 3  # ✗ Bad - negative/zero values
```

#### 3. Performance Issues
```bash
# Issue: Very slow execution with many threads
# Solution: Reduce concurrent limit or total users
# Edit the code: int concurrent_limit = 3; // Reduce this number

# Issue: System becomes unresponsive
# Solution: Use reasonable limits (< 1000 users)
```

#### 4. Output Issues
```bash
# Issue: Garbled output (overlapping prints)
# This is normal due to concurrent execution
# Each thread's output may interleave with others
```

## 📖 Code Walkthrough

### Key Components Explained:

#### 1. Data Structures
```c
// Each show has its own mutex for fine-grained locking
typedef struct {
    int show_id;                 // Unique identifier
    int available_tickets;       // SHARED DATA - protected by mutex
    pthread_mutex_t mutex;       // Individual lock per show
} Show;
```

#### 2. Synchronization Flow
```c
// Step 1: Acquire semaphore (limits concurrent access)
sem_wait(&semaphore);

// Step 2: Lock specific show's mutex
pthread_mutex_lock(&show.mutex);

// Step 3: CRITICAL SECTION - modify shared data
if (show.available_tickets > 0) {
    show.available_tickets--;  // This line MUST be protected
}

// Step 4: Release in reverse order
pthread_mutex_unlock(&show.mutex);
sem_post(&semaphore);
```

#### 3. Why Both Semaphore AND Mutex?
- **Semaphore**: Controls OVERALL system concurrency (max 3 users booking simultaneously)
- **Mutex**: Protects INDIVIDUAL show data (prevents race conditions)
- **Together**: Provide both system-level and data-level protection

#### 4. Memory Management
```c
// Dynamic allocation based on input
shows = malloc(num_shows * sizeof(Show));
threads = malloc(num_users * sizeof(pthread_t));

// Proper cleanup
free(shows);
free(threads);
pthread_mutex_destroy(&mutex);
sem_destroy(&semaphore);
```

### Critical Success Factors:
1. **Proper Lock Ordering**: Semaphore → Mutex → Unlock Mutex → Release Semaphore
2. **Error Handling**: Check all allocation and initialization calls
3. **Resource Cleanup**: Destroy all mutexes, free all memory
4. **Thread Joining**: Wait for all threads before cleanup

## 🎯 Learning Objectives Achieved

After running this program, you'll understand:
- ✅ How POSIX threads work in practice
- ✅ Why mutex locks prevent race conditions  
- ✅ How semaphores control concurrency
- ✅ Dynamic memory allocation in multithreaded programs
- ✅ Proper resource cleanup in concurrent applications
- ✅ Real-world application of thread synchronization

## 🏆 Success Indicators

Your program is working correctly if:
1. **No negative ticket counts** appear in the final report
2. **Total booked tickets** never exceeds initial available tickets
3. **All threads terminate** without hanging
4. **No segmentation faults** or crashes occur
5. **Memory is properly cleaned up** (no leak warnings with valgrind)

## 🤝 Need Help?

If you encounter issues:
1. Check the troubleshooting section above
2. Verify your input parameters are valid
3. Ensure pthread library is properly installed
4. Try running with smaller numbers first
5. Review the extensive comments in the source code

Happy coding! 🚀
'''

with open('EXECUTION_GUIDE.md', 'w') as f:
    f.write(execution_guide)

print("Created: EXECUTION_GUIDE.md")
print("\nExecution guide includes:")
print("- Complete step-by-step instructions")
print("- Detailed output explanations")
print("- Multiple test scenarios")
print("- Troubleshooting section")
print("- Code walkthrough with explanations")
print("- Learning objectives and success indicators")