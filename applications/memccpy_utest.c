#include <rtthread.h>
#include "utest.h"
#include <string.h>

void test_memccpy_base()
{
    char* str1 = "hello word!!!";
    char* str2= "hellosfsfddddhkalsfjdhjkl;asdfhjellosfsfddddhkalsfjdhjkl;asdfhjklfasdhsdfhjkashkalsfjdfsfddddhkalsfjdhjkl;asdfhjellosfsfdddhjkl;asdfhjellosfsfddddhkalsfjdhjkl;asdfhdfklhsdfhuioerfhuioerfjnkel;klfasdhsdfhjkasdfklhsdfhuioerfhuioerfjnkel;skarhjfowe\;;';.//;[[[]lsdat[paertadrgyaer@%^*&(&*)*()#%$&*(*(&%^&$%^#%$^$%&$%^%^&*())&*()*(_++=78908678%^&$%$%^@#@FYUGYUHJKLIHUtui6y))]]]";
    char* str3 = "5";
    char* testspace[100];
    int i1 = 0,i2 = 0, i3 = 0,sizelen = 0;

    
    
    i1 =  memccpy(testspace,str2,'h',10);
printf("\n%o    %o\n",i1,testspace);
    uassert_int_equal(i1, testspace);


    i2 =  memccpy(testspace,str2,';',100);
printf("\n%o    %o\n",i2,testspace+26);
    uassert_int_equal(i2,testspace +26);

    i3 =  memccpy(testspace,str2,';',10);
printf("\n%o    %o\n",i3,testspace);
    uassert_int_equal(i3,testspace +10);

    
}



static rt_err_t utest_tc_init(void)
{
    return RT_EOK;
}

static rt_err_t utest_tc_cleanup(void)
{
    return RT_EOK;
}

static void testcase(void)
{
    UTEST_UNIT_RUN(test_memccpy_base);
}
UTEST_TC_EXPORT(testcase, "memccpy test", utest_tc_init, utest_tc_cleanup, 10);