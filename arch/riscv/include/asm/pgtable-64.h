/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2012 Regents of the University of California
 */

#ifndef _ASM_RISCV_PGTABLE_64_H
#define _ASM_RISCV_PGTABLE_64_H

#include <linux/const.h>


#ifdef CONFIG_RV64_5LEVEL // lhy_add：用于开启5级分页
#define PGDIR_SHIFT	48
#define PGDIR_SIZE      (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SIZE - 1))

#define P4D_SHIFT	39
#define P4D_SIZE	(_AC(1, UL) << P4D_SHIFT)
#define P4D_MASK	(~(P4D_SIZE - 1))

#define MAX_POSSIBLE_PHYSMEM_BITS	56 // 根据RV64的草案，这里理应是56，但是ARM中是52

#else
#define PGDIR_SHIFT     39
#define PGDIR_SIZE      (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SIZE - 1))

#endif /* CONFIG_RV64_5LEVEL */

#define PUD_SHIFT	30
#define PUD_SIZE        (_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SIZE - 1))

#define PMD_SHIFT       21
#define PMD_SIZE        (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE - 1))

/**
 * TODO：
 * page.h中已经有pgd_t和pte_t的定义，这里需要去重
 * 在对PTRS_PER_PXD进行赋值的时候，是这里的定义优先，还是pgtable-nopxd.h中的定义优先
 * */
/** Page Global Directory entry */
//typedef struct {
//	unsigned long pgd;
//} pgd_t;
//#define pgd_val(x)      ((x).pgd) // 将pgd页表项类型的值转换为无符号类型
//#define __pgd(x)        ((pgd_t) { (x) }) // 将无符号类型转换为pgd页表项类型
//#define PTRS_PER_PGD    (PAGE_SIZE / sizeof(pgd_t)) // PGD的大小为一页，这里用一页的大小来除以页表项的大小，得到PGD中页表项的数目(详见pgtable.h中的128行)

/** Page 4 Directory entry */
typedef struct {
	unsigned long p4d;
} p4d_t; // p4d_t的定义已经在include/asm-generic/pgtable-nop4d.h中
#define p4d_val(x)      ((x).p4d) // 将p4d页表项类型的值转换为无符号类型
#define __p4d(x)        ((p4d_t) { (x) }) // 将无符号类型转换为p4d页表项类型
#define PTRS_PER_P4D    (PAGE_SIZE / sizeof(p4d_t)) // PGD的大小为一页，这里用一页的大小来除以页表项的大小，得到PGD中页表项的数目

/** Page Upper Directory entry */
typedef struct {
	unsigned long pud;
} pud_t;
#define pud_val(x)      ((x).pud) // 将pud页表项类型的值转换为无符号类型
#define __pud(x)        ((pud_t) { (x) }) // 将无符号类型转换为pud页表项类型
#define PTRS_PER_PUD    (PAGE_SIZE / sizeof(pud_t)) // PUD的大小为一页，这里用一页的大小来除以页表项的大小，得到PUD中页表项的数目

/** Page Middle Directory entry */
typedef struct {
	unsigned long pmd; // 此结构体用于描述pmd中的页表项
} pmd_t;
#define pmd_val(x)      ((x).pmd) // 将pmd页表项类型的值转换为无符号类型
#define __pmd(x)        ((pmd_t) { (x) }) // 将无符号类型转换为pmd页表项类型
#define PTRS_PER_PMD    (PAGE_SIZE / sizeof(pmd_t)) // PUD的大小为一页，这里用一页的大小来除以页表项的大小，得到PUD中页表项的数目

/** Page Table entry */
//typedef struct {
//	unsigned long pte;
//} pte_t; // 描述pte中的页表项s
//#define pte_val(x)	((x).pte) // 将页表项类型转换为无符号类型
//#define __pte(x)	((pte_t) { (x) } ) // 将无符号类型转换为页表项类型
//#define PTRS_PER_PTE    (PAGE_SIZE / sizeof(pte_t)) //(详见pgtable.h中的130行)

/**
 * 判断页表项是否有效的函数
 * #define _PAGE_PRESENT	0x100	// V-bit : page is valid
 * */
static inline int p4d_present(p4d_t p4d)
{
	return (p4d_val(p4d) & _PAGE_PRESENT);
}

static inline int pud_present(pud_t pud)
{
	return (pud_val(pud) & _PAGE_PRESENT);
}

/**
 * 判断页表项是否为0的函数
 * */
static inline int p4d_none(p4d_t p4d)
{
	return (p4d_val(p4d) == 0);
}

static inline int pud_none(pud_t pud)
{
	return (pud_val(pud) == 0);
}

/**
 * 判断页表项无效的函数
 * */
static inline int p4d_bad(p4d_t p4d)
{
	return !p4d_present(p4d);
}

static inline int pud_bad(pud_t pud)
{
	return !pud_present(pud);
}

/**
 * 判断是否为leaf pte的函数
 * 根据草案中P74的介绍，当R、W、X不全为0时，表示这是一个leaf pte
 * */
#define p4d_leaf	p4d_leaf
static inline int p4d_leaf(p4d_t p4d)
{
	return p4d_present(p4d) && (p4d_val(p4d) & (_PAGE_READ | _PAGE_WRITE | _PAGE_EXEC));
}

#define pud_leaf	pud_leaf
static inline int pud_leaf(pud_t pud)
{
	return pud_present(pud) && (pud_val(pud) & (_PAGE_READ | _PAGE_WRITE | _PAGE_EXEC));
}

/**
 * 为页表项赋值的函数
 * 令一个pud类型的指针指向pud中的页表项
 * */
static inline void set_p4d(p4d_t *p4dp, p4d_t p4d)
{
	*p4dp = p4d;
}

static inline void set_pud(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

/**
 * 清空页表项的函数
 * 给指向该页表项的指针赋值为0
 * */
static inline void p4d_clear(p4d_t *p4dp)
{
	set_p4d(p4dp, __p4d(0));
}

static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));
}

/**
 * 页帧号 -> 页表项
 * #define _PAGE_PFN_SHIFT 10
 * */
static inline p4d_t pfn_p4d(unsigned long pfn, pgprot_t prot)
{
	return __p4d((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

static inline pud_t pfn_pud(unsigned long pfn, pgprot_t prot)
{
	return __pud((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

/**
 * 页表项 -> 页帧号
 * #define _PAGE_PFN_SHIFT 10
 * */
static inline unsigned long _p4d_pfn(p4d_t p4d)
{
	return p4d_val(p4d) >> _PAGE_PFN_SHIFT;
}

static inline unsigned long _pud_pfn(pud_t pud)
{
	return pud_val(pud) >> _PAGE_PFN_SHIFT;
}

/**
 * 页表项 -> 页帧号 —> 页帧结构体
 * */
static inline struct page *p4d_page(p4d_t p4d)
{
	return pfn_to_page(p4d_val(p4d) >> _PAGE_PFN_SHIFT);
}

static inline struct page *pud_page(pud_t pud)
{
	return pfn_to_page(pud_val(pud) >> _PAGE_PFN_SHIFT);
}

/**
 * pud页表项 -> 页帧号 -> 页帧对应的虚拟地址
 * */
static inline unsigned long p4d_page_vaddr(p4d_t p4d)
{
	return (unsigned long)pfn_to_virt(p4d_val(p4d) >> _PAGE_PFN_SHIFT);
}

static inline unsigned long pud_page_vaddr(pud_t pud)
{
	return (unsigned long)pfn_to_virt(pud_val(pud) >> _PAGE_PFN_SHIFT);
}

/**
 * TODO：
 * 参照Alex的补丁，这里可能需要定义mm_p4d_folded和mm_pud_folded
 * Alex利用了pgtable_l4_enabled这个变量，但是这里的值如何确定还有待商榷
 * 同时可能还需要定义p4d_index、pud_index和pmd_index，但是一方面不确定定义的位置，另一方面不确定为什么pmd要在这里定义
 * */

/**
 * 错误信息处理
 * */
#define p4d_ERROR(e) \
	pr_err("%s:%d: bad p4d %016lx.\n", __FILE__, __LINE__, p4d_val(e))

#define pud_ERROR(e) \
	pr_err("%s:%d: bad pud %016lx.\n", __FILE__, __LINE__, pud_val(e))

#define pmd_ERROR(e) \
	pr_err("%s:%d: bad pmd %016lx.\n", __FILE__, __LINE__, pmd_val(e))

#endif /* _ASM_RISCV_PGTABLE_64_H */
