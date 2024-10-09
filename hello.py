
def print_colors():
    for r in range(0, 256, 51):  # Adjust the step for fewer colors
        for g in range(0, 256, 51):
            for b in range(0, 256, 51):
                print(f'\033[48;2;{r};{g};{b}m {r},{g},{b} \033[0m')

print_colors()

