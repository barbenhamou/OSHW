#!/bin/bash

# Compile the program
make

# Array of test configurations
declare -a configs=("config1" "config2" "config3") # Add more configuration names as needed

# Function to generate a configuration file
generate_config() {
    local filename=$1
    local num_producers=$2
    local total_products=0

    for ((i=1; i<=num_producers; i++)); do
        local num_products=$((RANDOM % 20 + 1))
        local queue_size=$((RANDOM % 10 + 1))
        total_products=$((total_products + num_products))

        # Append producer information to the file
        echo -e "PRODUCER $i\n$num_products\nqueue size = $queue_size\n" >> "$filename"
    done

    # Co-Editor queue size (randomly chosen for demonstration)
    local co_editor_queue_size=$((RANDOM % 10 + 1))

    # Create the file and write Co-Editor queue size
    echo -e "Co-Editor queue size = $co_editor_queue_size" >> "$filename"

    # Return the total number of products expected
    echo $total_products
}

# Test each configuration
for config in "${configs[@]}"; do
    # Generate configuration file
    total_products=$(generate_config "$config" $((RANDOM % 5 + 1))) # Random number of producers for demonstration

    # Run the program with the configuration file with a timeout to prevent deadlock
    timeout 10s ./ex3.out "./$config" > output.txt
    exit_status=$?

    # Check if the program entered a deadlock
    if [ "$exit_status" -eq 124 ]; then
        echo "Test $config FAILED: Program entered a deadlock."
        rm "$config"
        exit 1
    fi
    # Verify the output
    expected_lines=$((total_products + 1)) # Total products + 1 for "DONE"
    actual_lines=$(wc -l < output.txt)

    if [ "$actual_lines" -eq "$expected_lines" ]; then
        echo "Test $config PASSED"
    else
        echo "Test $config FAILED: Expected $expected_lines lines, got $actual_lines"
    fi

    # Clean up
    rm output.txt
    rm "$config"
done
make clean