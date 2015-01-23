/*
 * =====================================================================================
 *
 *       Filename:  DERIVE.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2013 03:25:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Costaxu (), costaxu@tencent.com
 *        Company:  Tencent, China
 *
 * =====================================================================================
 */
#ifndef DERIVE_H
#define DERIVE_H
#include "Base.h"
#include "Base1.h"
class Derive :public Base,public Base1
{
    public:
        void g();
};

#endif
