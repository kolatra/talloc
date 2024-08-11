run:
    gcc malloc.c -g -o the_binary && ./the_binary

clean:
    rm the_binary && rm a.out