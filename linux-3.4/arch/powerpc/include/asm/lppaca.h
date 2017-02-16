/*
 * lppaca.h
 * Copyright (C) 2001  Mike Corrigan IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#ifndef _ASM_POWERPC_LPPACA_H
#define _ASM_POWERPC_LPPACA_H
#ifdef __KERNEL__

/* These definitions relate to hypervisors that only exist when using
 * a server type processor
 */
#ifdef CONFIG_PPC_BOOK3S

#include <linux/cache.h>
#include <linux/threads.h>
#include <asm/types.h>
#include <asm/mmu.h>

/*
 * We only have to have statically allocated lppaca structs on
 * legacy iSeries, which supports at most 64 cpus.
 */
#define NR_LPPACAS  1


/* The Hypervisor barfs if the lppaca crosses a page boundary.  A 1k
 * alignment is sufficient to prevent this */
struct lppaca {
  u32 desc;    
  u16 size;    
  u16 reserved1;   
  u16 reserved2: 14; 
  u8  shared_proc: 1;  
  u8  secondary_thread: 1;
  volatile u8 dyn_proc_status: 8;
  u8  secondary_thread_count;
  volatile u16 dyn_hv_phys_proc_index;// Dynamic HV Physical Proc Index0C-x0D
  volatile u16 dyn_hv_log_proc_index;// Dynamic HV Logical Proc Indexx0E-x0F
  u32 decr_val;      
  u32 pmc_val;         
  volatile u32 dyn_hw_node_id; 
  volatile u32 dyn_hw_proc_id; 
  volatile u32 dyn_pir;  
  u32 dsei_data;           
  u64 sprg3;               
  u8  reserved3[40];   
  volatile u8 vphn_assoc_counts[8];
  u8  reserved4[32];   
  
  union {
    u64 any_int;
    struct {
      u16 reserved;
      u8  xirr_int;
      u8  ipi_cnt; 
      u8  decr_int;
      u8  pdc_int; 
      u8  quantum_int; 
      u8  old_plic_deferred_ext_int; 
    } fields;
  } int_dword;
  
  u64 plic_defer_ints_area;
  
  u64 saved_srr0;  
  u64 saved_srr1;  
  
  u64 saved_gpr3;  
  u64 saved_gpr4;  
  union {
    u64 saved_gpr5; /* Saved GPR5               x30-x37 */
    struct {
      u8  cede_latency_hint;  /*      x30 */
      u8  reserved[7];        /*        x31-x36 */
    } fields;
  } gpr5_dword;
  
  
  u8  dtl_enable_mask; 
  u8  donate_dedicated_cpu;
  u8  fpregs_in_use;   
  u8  pmcregs_in_use;  
  volatile u32 saved_decr; 
  volatile u64 emulated_time_base;// Emulated TB for this thread  x40-x47
  volatile u64 cur_plic_latency; 
  u64 tot_plic_latency;
  u64 wait_state_cycles; 
  u64 end_of_quantum;  
  u64 pdc_saved_sprg1; 
  u64 pdc_saved_srr0;  
  volatile u32 virtual_decr; 
  u16 slb_count;   
  u8  idle;    
  u8  vmxregs_in_use;  
  
  
  volatile u32 yield_count;
  volatile u32 dispersion_count; 
  volatile u64 cmo_faults; 
  volatile u64 cmo_fault_time; 
  u8  reserved7[104];  
  
  u32 page_ins;  
  u8  reserved8[148];  
  volatile u64 dtl_idx;  
  u8  reserved9[96];   
} __attribute__ ( (__aligned__ (0x400) ) );

extern struct lppaca lppaca[];

#define lppaca_of(cpu)  (*paca[cpu].lppaca_ptr)

/*
 * SLB shadow buffer structure as defined in the PAPR.  The save_area
 * contains adjacent ESID and VSID pairs for each shadowed SLB.  The
 * ESID is stored in the lower 64bits, then the VSID.
 */
struct slb_shadow {
  u32 persistent;  
  u32 buffer_length;   
  u64 reserved;  
  struct  {
    u64     esid;
    u64 vsid;
  } save_area[SLB_NUM_BOLTED]; 
} ____cacheline_aligned;

extern struct slb_shadow slb_shadow[];

/*
 * Layout of entries in the hypervisor's dispatch trace log buffer.
 */
struct dtl_entry {
  u8  dispatch_reason;
  u8  preempt_reason;
  u16 processor_id;
  u32 enqueue_to_dispatch_time;
  u32 ready_to_enqueue_time;
  u32 waiting_to_ready_time;
  u64 timebase;
  u64 fault_addr;
  u64 srr0;
  u64 srr1;
};

#define DISPATCH_LOG_BYTES  4096  /* bytes per cpu */
#define N_DISPATCH_LOG    (DISPATCH_LOG_BYTES / sizeof(struct dtl_entry))

extern struct kmem_cache * dtl_cache;

/*
 * When CONFIG_VIRT_CPU_ACCOUNTING = y, the cpu accounting code controls
 * reading from the dispatch trace log.  If other code wants to consume
 * DTL entries, it can set this pointer to a function that will get
 * called once for each DTL entry that gets processed.
 */
extern void (*dtl_consumer) (struct dtl_entry * entry, u64 index);

#endif /* CONFIG_PPC_BOOK3S */
#endif /* __KERNEL__ */
#endif /* _ASM_POWERPC_LPPACA_H */
