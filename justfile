build:
    cmake --build /home/tyler/dev/c-experiments/build

run-test:
    ./build/malloc_test 100000

clean:
    rm the_binary && rm a.out