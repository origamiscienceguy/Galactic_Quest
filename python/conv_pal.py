def convert_jasc_pal_to_clrx(input_file, output_file):
    try:
        with open(input_file, 'r') as f:
            lines = f.readlines()
        
        if len(lines) < 3:
            raise ValueError("Invalid input data format. Expected at least 3 lines.")

        # Extract number of colors
        num_colors = int(lines[2].strip())
        
        if len(lines) < 3 + num_colors:
            raise ValueError(f"Expected {num_colors} color entries, but found {len(lines) - 3}.")

        # Prepare CLRX format header
        clrx_lines = []
        clrx_lines.append(bytearray(b"CLRX 8 " + str(num_colors).encode()))
        clrx_lines.append(bytearray(b"\x0D\x0A"))

        # Process color entries
        for i in range(3, 3 + num_colors):
            rgb_values = lines[i].strip().split()
            if len(rgb_values) != 4:
                raise ValueError(f"Invalid RGB format in line {i + 1}. Expected 4 values (R G B A).")

            # Extract RGB values (ignore alpha)
            r = int(rgb_values[0])
            g = int(rgb_values[1])
            b = int(rgb_values[2])

            # Convert RGB values to CLRX format (0x00bbggrr)
            clrx_color = bytearray(f"0x00{b:02x}{g:02x}{r:02x}".encode())  # Hex in uppercase

            # Append delimiter "20" (space) in hex
            clrx_lines.append(clrx_color)
            
            # Check if this is the 4th color (0-based index: i - 3 is divisible by 4)
            if (i - 2) % 4 == 0:
                clrx_lines.append(bytearray(b"\x20\x0D\x0A"))  # Space, CR, LF
            else:
                clrx_lines.append(bytearray(b"\x20"))  # Only space delimiter
        
        # Remove the last space delimiter if it exists
        if clrx_lines:
            clrx_lines.pop()  # Remove the last space delimiter

        # Append footer "20 0D 0A"
        clrx_lines.append(bytearray(b"\x20\x0D\x0A"))

        # Write to output file
        with open(output_file, 'wb') as f:
            for line in clrx_lines:
                f.write(line)
        
        print(f"Conversion successful. Output written to {output_file}")

    except FileNotFoundError:
        print(f"Error: File {input_file} not found.")
    except ValueError as ve:
        print(f"Error: {ve}")

import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: conv_pal.py aseprite_format.pal usenti_format.pal")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        convert_jasc_pal_to_clrx(input_file, output_file)
