/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2009 Chen Liqin <liqin.chen@sunplusct.com>
 * Copyright (C) 2012 Regents of the University of California
 */

#ifndef _ASM_RISCV_PGALLOC_H
#define _ASM_RISCV_PGALLOC_H

#include <linux/mm.h>
#include <asm/tlb.h>

#ifdef CONFIG_MMU
#include <asm-generic/pgalloc.h>

static inline void pmd_populate_kernel(struct mm_struct *mm,
	pmd_t *pmd, pte_t *pte)
{
	unsigned long pfn = virt_to_pfn(pte);

	set_pmd(pmd, __pmd((pfn << _PAGE_PFN_SHIFT) | _PAGE_TABLE));
}

static inline void pmd_populate(struct mm_struct *mm,
	pmd_t *pmd, pgtable_t pte) // 物理页升级？TODO：查询这个函数的作用,编译解析
{
	unsigned long pfn = virt_to_pfn(page_address(pte));

	set_pmd(pmd, __pmd((pfn << _PAGE_PFN_SHIFT) | _PAGE_TABLE));
}

/* TODO：这里需要对P4D和PUD进行处理
 * 目前不确定判断的依据，是CONFIG_PGTABLE_LEVELS还是__PAGETABLE_PXD_FOLDED
 * */
#ifndef __PAGETABLE_PMD_FOLDED
static inline void pud_populate(struct mm_struct *mm, pud_t *pud, pmd_t *pmd)
{
	unsigned long pfn = virt_to_pfn(pmd);

	set_pud(pud, __pud((pfn << _PAGE_PFN_SHIFT) | _PAGE_TABLE));
}
#endif /* __PAGETABLE_PMD_FOLDED */

#ifndef __PAGETABLE_PUD_FOLDED
static inline void p4d_populate(struct mm_struct *mm, p4d_t *p4d, pud_t *pud)
{
	unsigned long pfn = virt_to_pfn(pud);

	set_p4d(p4d, __p4d((pfn << _PAGE_PFN_SHIFT) | _PAGE_TABLE));
}
#endif /* __PAGETABLE_PUD_FOLDED */

#define pmd_pgtable(pmd)	pmd_page(pmd)

static inline pgd_t *pgd_alloc(struct mm_struct *mm)
{
	pgd_t *pgd;

	pgd = (pgd_t *)__get_free_page(GFP_KERNEL);
	if (likely(pgd != NULL)) {
		memset(pgd, 0, USER_PTRS_PER_PGD * sizeof(pgd_t));
		/* Copy kernel mappings */
		memcpy(pgd + USER_PTRS_PER_PGD,
			init_mm.pgd + USER_PTRS_PER_PGD,
			(PTRS_PER_PGD - USER_PTRS_PER_PGD) * sizeof(pgd_t));
	}
	return pgd;
}

// TODO：这里需要对P4D和PUD进行处理
#ifndef __PAGETABLE_PMD_FOLDED
#define __pmd_free_tlb(tlb, pmd, addr)  pmd_free((tlb)->mm, pmd)
#endif /* __PAGETABLE_PMD_FOLDED */

#ifndef __PAGETABLE_PUD_FOLDED
#define __pud_free_tlb(tlb, pud, addr)  pud_free((tlb)->mm, pud)
#endif /* __PAGETABLE_PMD_FOLDED */

#ifndef __PAGETABLE_P4D_FOLDED
#define __p4d_free_tlb(tlb, p4d, addr)  p4d_free((tlb)->mm, p4d)
#endif /* __PAGETABLE_P4D_FOLDED */

#define __pte_free_tlb(tlb, pte, buf)   \
do {                                    \
	pgtable_pte_page_dtor(pte);     \
	tlb_remove_page((tlb), pte);    \
} while (0)
#endif /* CONFIG_MMU */

#endif /* _ASM_RISCV_PGALLOC_H */
