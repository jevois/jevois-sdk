#ifndef __HDMI_EDID_H_
#define __HDMI_EDID_H_

#define Abort_Current_Operation             0
#define Special_Offset_Address_Read         1
#define Explicit_Offset_Address_Write       2
#define Implicit_Offset_Address_Write       3
#define Explicit_Offset_Address_Read        4
#define Implicit_Offset_Address_Read        5
#define Explicit_Offset_Address_E_DDC_Read  6
#define Implicit_Offset_Address_E_DDC_Read  7

extern __u8 Device_Support_VIC[512];
extern __s32 ParseEDID (void);
extern __u32 GetIsHdmi (void);
extern __u32 GetIsYUV (void);
extern __s32 GetEdidInfo (void);

#endif