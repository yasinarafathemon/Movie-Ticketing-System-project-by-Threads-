# Create a comprehensive Makefile for easy compilation

makefile_content = '''# ============================================================================
#                          MOVIE TICKET BOOKING SYSTEM
#                                 Makefile
# ============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
LDFLAGS = -pthread

# Program name
PROGRAM = movie_booking

# Source file
SOURCE = movie_ticket_booking_complete.c

# Default target
all: $(PROGRAM)

# Compile the program
$(PROGRAM): $(SOURCE)
	@echo "🔨 Compiling Movie Ticket Booking System..."
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SOURCE) $(LDFLAGS)
	@echo "✅ Compilation successful!"
	@echo "🚀 Run with: ./$(PROGRAM) <users> <tickets> <shows>"
	@echo "📝 Example: ./$(PROGRAM) 10 5 3"

# Debug version with additional debugging symbols
debug: $(SOURCE)
	@echo "🐛 Compiling debug version..."
	$(CC) $(CFLAGS) -g -DDEBUG -o $(PROGRAM)_debug $(SOURCE) $(LDFLAGS)
	@echo "✅ Debug version compiled!"
	@echo "🔍 Run with: ./$(PROGRAM)_debug <users> <tickets> <shows>"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(PROGRAM) $(PROGRAM)_debug
	@echo "✅ Clean complete!"

# Test targets for different scenarios
test-basic:
	@echo "🧪 Running basic test (5 users, 10 tickets, 2 shows)..."
	./$(PROGRAM) 5 10 2

test-overbook:
	@echo "🧪 Running overbooking test (15 users, 5 tickets, 1 show)..."
	./$(PROGRAM) 15 5 1

test-stress:
	@echo "🧪 Running stress test (50 users, 20 tickets, 5 shows)..."
	./$(PROGRAM) 50 20 5

# Help target
help:
	@echo "🆘 Available targets:"
	@echo "  all         - Compile the program (default)"
	@echo "  debug       - Compile debug version"
	@echo "  clean       - Remove compiled files"
	@echo "  test-basic  - Run basic functionality test"
	@echo "  test-overbook - Run overbooking scenario test"
	@echo "  test-stress - Run stress test"
	@echo "  help        - Show this help message"

# Install target (optional)
install: $(PROGRAM)
	@echo "📦 Installing $(PROGRAM)..."
	cp $(PROGRAM) /usr/local/bin/
	@echo "✅ Installation complete!"

# Uninstall target (optional)
uninstall:
	@echo "🗑️  Uninstalling $(PROGRAM)..."
	rm -f /usr/local/bin/$(PROGRAM)
	@echo "✅ Uninstallation complete!"

# Phony targets
.PHONY: all debug clean test-basic test-overbook test-stress help install uninstall
'''

with open('Makefile', 'w') as f:
    f.write(makefile_content)

print("Created: Makefile")
print("\nMakefile features:")
print("- Simple compilation with 'make'")
print("- Debug version with 'make debug'")
print("- Built-in test cases")
print("- Clean target for cleanup")
print("- Help target for usage information")