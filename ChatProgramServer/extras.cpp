/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Close
--
-- DATE: March 29, 2018
--
-- REVISIONS: March 29, 2018
--
-- DESIGNER: Alfred Swinton
--
-- PROGRAMMER: Alfred Swinton
--
-- INTERFACE: int Close(int filedes)
--
-- RETURNS: int
--
-- NOTES:
-- This fucntion is a wrapper function for the c close function.
----------------------------------------------------------------------------------------------------------------------*/

#include <unistd.h>

int Close(int filedes)
{
    int ret;
    ret = close(filedes);
    return ret;
}
