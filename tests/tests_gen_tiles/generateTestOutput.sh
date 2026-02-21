#!/bin/bash

# Script to generate test case output for tileUtil program with input files and pipe output to output folder

# Ensure output directory exists
mkdir -p output

# Iterate through each .in file in the input directory
for input_file in input/*.in; do
    # Extract the base name (e.g., "1" from "1.in")
    base_name=$(basename "$input_file" .in)

    # Define the output file path
    output_file="output/${base_name}.out"

    echo -n "Processing: $input_file -> $output_file"

    # Run the tileUtil program with input piped in and output piped to file
    ../../src/tileUtils < "$input_file" > "$output_file"

    # Check if the command was successful
    if [ $? -eq 0 ]; then
        echo "  ✓ Success"
    else
        echo "  ✗ Failed"
    fi
done

echo "All tests completed!"
