#!/bin/bash

# Compares the output with the input
compare_output() {
    local test_name="$1"
    local actual_output="$2"
    local expected_output="$3"

    if [ "$actual_output" = "$expected_output" ]; then
        echo "$test_name passed"
    else
        echo "$test_name failed"
    fi
}

# Runs the test cases
run_test() {
    local test_name="$1"
    local expected_output="$2"

    # Executes the test cases
    actual_output=$(./mtll < "${test_name}")

    # Compares the outputs
    compare_output "${test_name}" "${actual_output}" "${expected_output}"
}

main() {
    # List of test cases
    test_cases=(
        "tests/corner_test.in"
        "tests/corner_test2.in"
        "tests/corner_test3.in"
        "tests/normal_test.in"
        "tests/large_test.in"
    )

    # Expected outputs
    expected_outputs=(
        "tests/corner_test.out"
        "tests/corner_test2.out"
        "tests/corner_test3.out"
        "tests/normal_test.out"
        "tests/large_test.out"
    )

    # Run tests
    for ((i = 0; i < ${#test_cases[@]}; i++)); do
        run_test "${test_cases[$i]}" "$(cat "${expected_outputs[$i]}")"
    done
}

# Call main function
main
