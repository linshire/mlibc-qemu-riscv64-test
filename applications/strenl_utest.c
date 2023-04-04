#include <rtthread.h>
#include "utest.h"
#include <string.h>

void test_strenl_base()
{
    char* str1 = "hello word!!!";
    char* str2= "hellosfsfddddhkalsfjdhjkl;asdfhjellosfsfddddhkalsfjdhjkl;asdfhjklfasdhsdfhjkashkalsfjdfsfddddhkalsfjdhjkl;asdfhjellosfsfdddhjkl;asdfhjellosfsfddddhkalsfjdhjkl;asdfhdfklhsdfhuioerfhuioerfjnkel;klfasdhsdfhjkasdfklhsdfhuioerfhuioerfjnkel;skarhjfowe\;;';.//;[[[]lsdat[paertadrgyaer@%^*&(&*)*()#%$&*(*(&%^&$%^#%$^$%&$%^%^&*())&*()*(_++=78908678%^&$%$%^@#@FYUGYUHJKLIHUtui6y))]]]";
    char* str3 = "5";

    int i1 = 0,i2 = 0, i3 = 0,sizelen = 0;

    i1 =  strlen(str1) ;
    uassert_int_equal(i1, 13);


    i2 =  strlen(str2);
    sizelen = sizeof(*str1);

    // uassert_int_equal(i2,sizeof(*str2));

    i3 =  strlen(str3) ;

    
    uassert_int_equal(i3,1);

    
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
    UTEST_UNIT_RUN(test_strenl_base);
}
UTEST_TC_EXPORT(testcase, "strenl test", utest_tc_init, utest_tc_cleanup, 10);