#include <stdio.h>
class C
{
    public:
        virtual void a1(){};
        virtual void b(){};
        virtual void c(){};
        virtual void d(){};
        virtual void e(){};

};
class A
{
    public:
    virtual int a()
    {
        b();
        c();
        d();
        e();
        printf("a\n");  
        return 0;
    }

    virtual void b(){};
    virtual void c(){};
    virtual void d(){};
    virtual void e(){};
    
};

class B:public A,public C
{
};

int main()
{
    B b,*pb=&b;
    pb->a();
    pb->a1();
    
    printf("Address of b:\n");
    printf("%08x\n",&b);
    printf("sizeof b:\n");
    printf("%d\n",sizeof(b));

    printf("Address of vtable of b[0]:\n");
    printf("%08x\n",((int*)&b)[0]);

    printf("vtable 0\n");
    int * pVtable =(int *) (((int*) &b)[0]);
    
    printf("%08x\n", pVtable[0]);
    printf("%08x\n", pVtable[1]);
    printf("%08x\n", pVtable[2]);
    printf("%08x\n", pVtable[3]);
    printf("%08x\n", pVtable[4]);
    printf("%08x\n", pVtable[5]);
    printf("%08x\n", pVtable[6]);
    printf("%08x\n", pVtable[7]);
    printf("%08x\n", pVtable[8]);

    printf("Address of A:a():\n");
    printf("%08x\n", (&A::a));

    printf("Address of A:c():\n");
    printf("%08x\n", (&A::c));

    printf("Address of A:d():\n");
    printf("%08x\n", (&A::d));
    return 0;
}
