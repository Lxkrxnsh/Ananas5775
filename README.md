cd src
gcc -Wall -Werror -Wextra game_of_life.c -lncurses -o game_of_life

gcc -Wall -Werror -Wextra gen_patterns.c -o gen_patterns
./gen_patterns

./game_of_life < pattern1.txt