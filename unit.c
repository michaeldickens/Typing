//
//  unit.c
//  Typing
//
//  Created by Michael Dickens on 3/20/13.
//

#include <stdio.h>
#include "accessories.h"

#define PRINT_VERBOSE 0

/* 
 * Takes the expected value, the name of the function, and a list of arguments 
 * to the function. Calls the named function and passes its result into 
 * verify(). Passes in fun (converted to a string literal) as the message.
 */
#define VERIFY(expected, fun, ...) verify(expected, fun(__VA_ARGS__), #fun)

/* 
 * Compares expected to found. If they are not equal, prints an error message.
 */
int verify(int expected, int found, const char *message)
{
    int res = (expected == found);
    
    if (!res && PRINT_VERBOSE) printf("##### FAILURE: #####");
    if (!res || PRINT_VERBOSE)
        printf("%s: %d expected, %d found.\n", message, expected, found);
    
    return res;
}

int main()
{
    /* Initialize. */
    setksize(K_NO);
    
    /* TODO: Write some unit tests. */
                 
    return 0;
}
