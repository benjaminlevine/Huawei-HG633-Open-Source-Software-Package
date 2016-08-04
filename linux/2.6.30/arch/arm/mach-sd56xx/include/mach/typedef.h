/******************************************************************************
  文件名称: typedef.h
  功能描述: 头文件
  版本描述: V1.0

  创建日期: D2011_07_19
  创建作者: luocheng 00183967

  修改记录: 
            生成初稿.
******************************************************************************/

#ifndef __HI_TYPEDEF_H__
#define __HI_TYPEDEF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*********************************************************/
#ifndef hi_void
#define   hi_void              void
#endif

#ifndef hi_char8
#define   hi_char8             char
#endif

#ifndef hi_short16
#define   hi_short16           short
#endif

#ifndef hi_int32
#define   hi_int32             int
#endif

#ifndef hi_int64
#define   hi_int64             long long
#endif

/*********************************************************/

#ifndef hi_uchar8
#define   hi_uchar8            unsigned char
#endif

#ifndef hi_ushort16
#define   hi_ushort16          unsigned short
#endif

#ifndef hi_uint32
#define   hi_uint32            unsigned int
#endif

#ifndef hi_uint64
#define   hi_uint64            unsigned long long
#endif

/*********************************************************/
#ifndef hi_size_t
typedef hi_uint32       hi_size_t;
#endif

/*********************************************************/
#define HI_NULL                 NULL
#define hi_printf               printk
#define hi_memcpy               memcpy
#define hi_memcmp               memcmp
#define hi_memset               memset

#define hi_malloc(size)         kmalloc(size, GFP_KERNEL)
#define hi_free                 kfree

#define hi_mdelay               mdelay

#define HI_BUG(condition)       BUG()
#define HI_MIN                  min

#define HI_FALSE                0
#define HI_TRUE                 1

/*********************************************************/
#define HI_REG_WRITE(addr, val) (*(volatile hi_uint32*)(addr) = (val))
#define HI_REG_READ(addr, val)  ((val) = *(volatile hi_uint32*)(addr))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_TYPEDEF_H__ */
