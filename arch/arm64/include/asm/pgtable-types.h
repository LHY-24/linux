/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Page table types definitions.
 *
 * Copyright (C) 2014 ARM Ltd.
 * Author: Catalin Marinas <catalin.marinas@arm.com>
 */

#ifndef __ASM_PGTABLE_TYPES_H
#define __ASM_PGTABLE_TYPES_H

#include <asm/types.h>

typedef u64 pteval_t;
typedef u64 pmdval_t;
typedef u64 pudval_t;
typedef u64 p4dval_t;
typedef u64 pgdval_t;

/*
 * These are used to make use of C type-checking..
 */
typedef struct { pteval_t pte; } pte_t; // 描述pte中的页表项
#define pte_val(x)	((x).pte) // 将页表项类型转换为无符号类型
#define __pte(x)	((pte_t) { (x) } ) // 将无符号类型转换为页表项类型

#if CONFIG_PGTABLE_LEVELS > 2
typedef struct { pmdval_t pmd; } pmd_t; // 描述pmd中的页表项
#define pmd_val(x)	((x).pmd) // 将页表项类型转换为无符号类型
#define __pmd(x)	((pmd_t) { (x) } ) // 将无符号类型转换为页表项类型
#endif

#if CONFIG_PGTABLE_LEVELS > 3
typedef struct { pudval_t pud; } pud_t; // 描述pud中的页表项
#define pud_val(x)	((x).pud) // 将页表项类型转换为无符号类型
#define __pud(x)	((pud_t) { (x) } ) // 将无符号类型转换为页表项类型
#endif

typedef struct { pgdval_t pgd; } pgd_t; // 描述pgd中的页表项
#define pgd_val(x)	((x).pgd) // 将页表项类型转换为无符号类型
#define __pgd(x)	((pgd_t) { (x) } ) // 将无符号类型转换为页表项类型

typedef struct { pteval_t pgprot; } pgprot_t;
#define pgprot_val(x)	((x).pgprot) // 将页表项类型转换为无符号类型
#define __pgprot(x)	((pgprot_t) { (x) } ) // 将无符号类型转换为页表项类型

/**
 * 根据页表级数加载不同的文件，从而对相关页表进行折叠
 * */
#if CONFIG_PGTABLE_LEVELS == 2
#include <asm-generic/pgtable-nopmd.h>
#elif CONFIG_PGTABLE_LEVELS == 3
#include <asm-generic/pgtable-nopud.h>
#elif CONFIG_PGTABLE_LEVELS == 4
#include <asm-generic/pgtable-nop4d.h>
#endif

#endif	/* __ASM_PGTABLE_TYPES_H */
