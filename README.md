# PT-Cruisers Project

## Overview
This project is a terminal-based racing game where multiple racer threads race across the terminal window, or burn out before the finish line. The project is implemented in C and utilizes POSIX threads (pthread) for concurrency.

## Usage

### Compilation
To compile the project, use the provided `Makefile`. Ensure you have `make` installed, then run:
```sh
make
```
### Running the Program
To run the program, use the following command:

```sh
./pt-cruisers [max-speed-delay] name1 name2 [name3...]
```
- `max-speed-delay` (optional): The maximum delay (in milliseconds) for the racers after moving. If not provided, the default delay is used. Minimum of 100.
- `name1`, `name2`, `[name3...]`: Names of the racers. Each name should not exceed 6 characters.

