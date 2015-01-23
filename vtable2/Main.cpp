/*
 * =====================================================================================
 *
 *       Filename:  Main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2013 03:32:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Costaxu (), costaxu@tencent.com
 *        Company:  Tencent, China
 *
 * =====================================================================================
 */

#include "Derive.h"

int main()
{
    Derive *obj = new Derive;
    obj->Base1::f();
    return 0;

}
