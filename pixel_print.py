from PIL import Image

# Load the image
img = Image.open('Pngsuite/basn2c08.png')  # Replace with your PNG file

# Convert image to RGB (if not already in that mode)
img = img.convert('RGB')

# Get the pixel data
width, height = img.size
hex_values = []

for y in range(height):
    for x in range(width):
        rgb = img.getpixel((x, y))
        # Convert RGB to hex
        hex_code = '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])
        hex_values.append((x, y, hex_code))  # Store the coordinates and hex code

# Output the hex codes
for pixel in hex_values:
    print(pixel[2])

