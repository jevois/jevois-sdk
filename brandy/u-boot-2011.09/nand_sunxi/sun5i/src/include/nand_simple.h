#ifndef __PHY_BOOT__
#define  __PHY_BOOT__

#include "nand_drv_cfg.h"

struct boot_physical_param{
	__u8   chip;
	__u16  block;
	__u16  page;
	__u64  sectorbitmap;
	void   *mainbuf;
	void   *oobbuf;
};

extern __s32 PHY_SimpleErase(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleRead(struct boot_physical_param * readop);
extern __s32 PHY_SimpleWrite(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_1K(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_Seq(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleRead_Seq(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_1K(struct boot_physical_param * readop);
#endif
