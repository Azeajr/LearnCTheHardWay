#define NDEBUG

#include "dbg.h"
#include <stdlib.h>
#include <stdio.h>



void test_debug()
{
    debug("I have Brown Hair.");

    debug("I am %d years old.", 39);
}

void test_log_err()
{
    log_err("I believe everything is broken.");
    log_err("There are %d problems in %s.", 0, "space");
}

void test_log_warn()
{
    log_warn("You can safely ignore this.");
    log_warn("Maybe consider looking at: %s.", "/etc/passwd");
}

void test_log_info()
{
    log_info("Well I did something mundane.");
    log_info("It happened %f times today.", 1.3f);
}

int test_check(char *file_name)
{
    FILE *input = NULL;
    char *block = NULL;

    block = malloc(100);
    check_mem(block);

    input = fopen(file_name, "r");
    check(input, "Failed to open %s.", file_name);

    free(block);
    fclose(input);
    return 0;

error:
    if(block) free(block);
    if(input) fclose(input);
    return -1;

}

int test_sentinel(int code)
{
    char *temp = malloc(100);
    check_mem(temp);

    switch(code)
    {
        case 1:
            log_info("It worked.");
            break;
        default:
            sentinel("I shouldn't run.");
    }

    free(temp);
    return 0;

    error:
        if(temp) free(temp);
        return -1;
}

int test_check_mem()
{
    char *test = NULL;
    check_mem(test);

    free(test);
    return 1;

error:
    return -1;
}

int test_check_debug()
{
    int i = 0;
    check_debug(i != 0, "Oops, I was 0.");

    return 0;
error:
    return -1;
}

int main(int argc, char *argv[])
{
    check(argc == 2, "Need and argument");

    test_debug();
    test_log_err();
    test_log_warn();
    test_log_info();
    

    check(test_check("ex20.c") == 0, "failed with ex20.c");


    return 0;

error:
    return 1;
}
