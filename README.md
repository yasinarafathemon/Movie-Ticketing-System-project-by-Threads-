
# Movie Ticketing System

A multi-threaded movie ticketing booking system showcasing threading concepts (e.g., concurrent bookings, thread-safety) built by Threads.

## 🚀 Project Overview

This project implements a movie theatre ticketing system where multiple users (threads) can simultaneously book seats, ensuring correct handling of concurrency, race conditions, and data integrity. It serves as both a functional ticket-booking application and a demonstration of threads/multi-threading in action.

## 📋 Features

* Display movies / showtimes and available seats
* Multiple threads representing users trying to book tickets concurrently
* Seat allocation & booking logic with thread-safe operations
* Handling of race conditions (e.g., two threads trying for the same seat)
* Cancellation or release of seats (if implemented)
* Logging / output showing thread actions, booking successes/failures
* (Optional) Admin interface to add movies/showtimes or view bookings

## 🧩 Tech Stack

* Language: (e.g., Java)
* Concurrency: Threads, synchronization primitives (e.g., `synchronized`, locks)
* Data structures: Collections to hold movies, showtimes, seats
* Build: (e.g., Maven / Gradle / command-line)
* (Optional) UI: console-based or simple GUI

## 🗂️ Project Structure

```
Movie-Ticketing-System/
│
├── src/                  # source code (threads, movie booking logic)
├── test/                 # (optional) unit / concurrency tests
├── resources/            # (optional) config files, data files
├── README.md             # this file
├── LICENSE               # (if included)
└── build.gradle / pom.xml / Makefile   # build file
```

*(Adjust based on your actual directory names)*

## ✅ Getting Started

### Prerequisites

* Install the language runtime (e.g., Java 8+)
* Install the build tool (Maven or Gradle)
* (Optional) For testing frameworks if used

### Running the Project

1. Clone the repository:

   ```bash
   git clone https://github.com/yasinarafathemon/Movie-Ticketing-System-project-by-Threads-.git
   cd Movie-Ticketing-System-project-by-Threads-
   ```
2. Build the project:

   ```bash
   # If using Maven
   mvn clean package
   ```

   or

   ```bash
   # If using Gradle
   gradle build
   ```
3. Run the application:

   ```bash
   java -jar target/MovieTicketingSystem-1.0.jar
   ```

   or according to your entry point.
4. Follow prompts/console output to select movie/showtime and attempt bookings.

## 🧪 Usage

* On running, the system will list available movies and showtimes.
* Each booking attempt will be handled possibly by a separate thread.
* The output will demonstrate which thread succeeded, which seat was booked, and if any race condition was prevented correctly.
* You may try running multiple threads/bookings simultaneously to test concurrency.
* (Optional) Administrator user can add movies or showtimes before bookings start.

## 🧷 Future Enhancements

* Add GUI (Swing, JavaFX) or web interface for better usability.
* Persist data to a database instead of in-memory collections.
* Support booking cancellation and seat release.
* Use more advanced concurrency constructs (e.g., Executors, thread pools, Futures).
* Add unit tests for concurrency scenarios, and performance/load testing.
* Add logging framework (e.g., Log4J) and analytics of thread behavior.

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details (if you have one).

## 👤 Author

**Yasin Arafat Emon**
GitHub: [yasinarafathemon](https://github.com/yasinarafathemon)

