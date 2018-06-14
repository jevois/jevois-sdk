/*
  JeVois Smart Embedded Machine Vision Toolkit - Copyright (C) 2016 by Laurent Itti, the University of Southern
  California (USC), and iLab at USC. See http://iLab.usc.edu and http://jevois.org for information about this project.
  
  This file is part of the JeVois Smart Embedded Machine Vision Toolkit.  This program is free software; you can
  redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
  Foundation, version 2.  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.  You should have received a copy of the GNU General Public License along with this program;
  if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
  
  Contact information: Laurent Itti - 3641 Watt Way, HNB-07A - Los Angeles, CA 90089-2520 - USA.
  Tel: +1 213 740 3527 - itti@pollux.usc.edu - http://iLab.usc.edu - http://jevois.org
*/


#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/usb/video.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <asm/div64.h>

#define JEVOIS_SERIAL
#define JEVOIS_STORAGE

#include "f_uvc.h"
#ifdef JEVOIS_SERIAL
#include "u_serial.h"
#endif

#include "composite.c"
#include "usbstring.c"
#include "config.c"
#include "epautoconf.c"

#include "uvc_queue.c"
#include "uvc_video.c"
#include "uvc_v4l2.c"
#include "f_uvc.c"
#ifdef JEVOIS_SERIAL
#include "u_serial.c"
#include "f_acm.c"
#endif

#ifdef JEVOIS_STORAGE
#include "f_mass_storage.c"
#endif

static char * modes = "0/1448695129-320x240:333333";
module_param(modes, charp, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(modes, "Supported USB/UVC video modes, as compiled by the jevois-module-param utility.");

static bool use_serial = 1;
module_param(use_serial, bool, S_IRUGO);
MODULE_PARM_DESC(use_serial, "Add a CDC ACM serial-over-USB port when true");

static bool use_storage = 1;
module_param(use_storage, bool, S_IRUGO);
MODULE_PARM_DESC(use_storage, "Add access to microSD over USB when true");

static bool max_bandwidth = 1;
module_param(max_bandwidth, bool, S_IRUGO);
MODULE_PARM_DESC(max_bandwidth, "Request maximum USB isochronous bandwidth when true");

/* --------------------------------------------------------------------------
 * Device descriptor
 */

#define WEBCAM_VENDOR_ID        0x1d6b  /* Linux Foundation */
#define WEBCAM_PRODUCT_ID       0x0102  /* Webcam A/V gadget */
#define WEBCAM_DEVICE_BCD       0x0100  /* 1.00 */

static char webcam_vendor_label[] = "JeVois Inc";
static char webcam_product_label[] = "JeVois-A33 Smart Camera";
static char webcam_config_label[] = "Video";

/* string IDs are assigned dynamically */
#define STRING_MANUFACTURER_IDX 0
#define STRING_PRODUCT_IDX      1
#define STRING_DESCRIPTION_IDX  2

static struct usb_string webcam_strings[] = {
  [STRING_MANUFACTURER_IDX].s = webcam_vendor_label,
  [STRING_PRODUCT_IDX].s = webcam_product_label,
  [STRING_DESCRIPTION_IDX].s = webcam_config_label,
  {  }
};

static struct usb_gadget_strings webcam_stringtab = {
  .language = 0x0409,   /* en-us */
  .strings = webcam_strings,
};

static struct usb_gadget_strings *webcam_device_strings[] = {
  &webcam_stringtab,
  NULL,
};

static struct usb_device_descriptor webcam_device_descriptor = {
  .bLength            = USB_DT_DEVICE_SIZE,
  .bDescriptorType    = USB_DT_DEVICE,
  .bcdUSB             = cpu_to_le16(0x0200),
  .bDeviceClass       = USB_CLASS_MISC,
  .bDeviceSubClass    = 0x02,
  .bDeviceProtocol    = 0x01,
  .bMaxPacketSize0    = 0, /* dynamic */
  .idVendor           = cpu_to_le16(WEBCAM_VENDOR_ID),
  .idProduct          = cpu_to_le16(WEBCAM_PRODUCT_ID),
  .bcdDevice          = cpu_to_le16(WEBCAM_DEVICE_BCD),
  .iManufacturer      = 0, /* dynamic */
  .iProduct           = 0, /* dynamic */
  .iSerialNumber      = 0, /* dynamic */
  .bNumConfigurations = 0, /* dynamic */
};

DECLARE_UVC_HEADER_DESCRIPTOR(1);

static const struct UVC_HEADER_DESCRIPTOR(1) uvc_control_header = {
  .bLength            = UVC_DT_HEADER_SIZE(1),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubType = UVC_VC_HEADER,
  .bcdUVC             = cpu_to_le16(0x100),
  .wTotalLength       = 0, /* dynamic */
  .dwClockFrequency   = cpu_to_le32(48000000),
  .bInCollection      = 0, /* dynamic */
  .baInterfaceNr[0]   = 0, /* dynamic */
};


#define CT1_SCANNING_MODE_CONTROL        		0x01
#define CT1_AE_MODE_CONTROL              		0x02
#define CT1_AE_PRIORITY_CONTROL          		0x04
#define CT1_EXPOSURE_TIME_ABSOLUTE_CONTROL      0x08
#define CT1_EXPOSURE_TIME_RELATIVE_CONTROL      0x10
#define CT1_FOCUS_ABSOLUTE_CONTROL       		0x20
#define CT1_FOCUS_RELATIVE_CONTROL       		0x40
#define CT1_FOCUS_AUTO_CONTROL           		0x80

#define CT2_IRIS_ABSOLUTE_CONTROL        		0x01
#define CT2_IRIS_RELATIVE_CONTROL        		0x02
#define CT2_ZOOM_ABSOLUTE_CONTROL        		0x04
#define CT2_ZOOM_RELATIVE_CONTROL        		0x08
#define CT2_PANTILT_ABSOLUTE_CONTROL     		0x10
#define CT2_PANTILT_RELATIVE_CONTROL     		0x20
#define CT2_ROLL_ABSOLUTE_CONTROL        		0x40
#define CT2_ROLL_RELATIVE_CONTROL        		0x80

#define CT3_PRIVACY_CONTROL              		0x01
#define CT3_FOCUS_SIMPLE_CONTROL                0x02
#define CT3_WINDOW_CONTROL                      0x04
#define CT3_REGION_OF_INTEREST_CONTROL          0x08

static const struct uvc_camera_terminal_descriptor uvc_camera_terminal = {
  .bLength                  = UVC_DT_CAMERA_TERMINAL_SIZE(3),
  .bDescriptorType          = USB_DT_CS_INTERFACE,
  .bDescriptorSubType       = UVC_VC_INPUT_TERMINAL,
  .bTerminalID              = 1,
  .wTerminalType            = cpu_to_le16(0x0201),
  .bAssocTerminal           = 0,
  .iTerminal                = 0,
  .wObjectiveFocalLengthMin = cpu_to_le16(0),
  .wObjectiveFocalLengthMax = cpu_to_le16(0),
  .wOcularFocalLength       = cpu_to_le16(0),
  .bControlSize             = 3,
  .bmControls[0]            = CT1_AE_MODE_CONTROL | /*CT1_AE_PRIORITY_CONTROL |*/ CT1_EXPOSURE_TIME_ABSOLUTE_CONTROL,
  .bmControls[1]            = 0,
  .bmControls[2]            = 0,
};


#define PU1_BRIGHTNESS		0x01
#define PU1_CONTRAST		0x02
#define PU1_HUE				0x04
#define	PU1_SATURATION		0x08
#define PU1_SHARPNESS		0x10
#define PU1_GAMMA			0x20
#define	PU1_WB_TEMP			0x40
#define	PU1_WB_COMPO		0x80

#define	PU2_BACKLIGHT		0x01
#define PU2_GAIN			0x02
#define PU2_POWER_LF		0x04
#define PU2_HUE_AUTO		0x08
#define PU2_WB_TEMP_AUTO	0x10
#define PU2_WB_COMPO_AUTO	0x20
#define PU2_DIGITAL_MULT	0x40
#define PU2_DIGITAL_LIMIT	0x80

#define PU3_AVIDEO_STD		0x01
#define PU3_AVIDEO_LOCK		0x02
#define PU3_CONTRAST_AUTO	0x04

static const struct uvc_processing_unit_descriptor uvc_processing = {
  .bLength            = UVC_DT_PROCESSING_UNIT_SIZE(2),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubType = UVC_VC_PROCESSING_UNIT,
  .bUnitID            = 2,
  .bSourceID          = 1,
  .wMaxMultiplier     = cpu_to_le16(0),
  .bControlSize       = 2,
  .bmControls[0]      = PU1_BRIGHTNESS | PU1_CONTRAST | PU1_SATURATION | PU1_SHARPNESS | PU1_WB_COMPO,
  .bmControls[1]      = PU2_GAIN | PU2_WB_COMPO_AUTO | PU2_POWER_LF,
  .iProcessing        = 0,
};

DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1 /* num inputs */, 1 /* controls size*/);

static const struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 1) uvc_extension = {
  .bLength            = UVC_DT_EXTENSION_UNIT_SIZE(1, 1),
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubType = UVC_VC_EXTENSION_UNIT,
  .bUnitID            = 4,
  .guidExtensionCode  = { 0x26, 0x52, 0x21, 0x5a, 0x89, 0x32, 0x56, 0x41, 0x89, 0x4a, 0x5c,
                          0x55, 0x7c, 0xdf, 0x96, 0x64 },
  .bNumControls       = 1,
  .bNrInPins          = 1,
  .baSourceID[0]      = 2,
  .bControlSize       = 1,
  .bmControls[0]      = 0x01, 
  .iExtension         = 0,
};

static const struct uvc_output_terminal_descriptor uvc_output_terminal = {
  .bLength            = UVC_DT_OUTPUT_TERMINAL_SIZE,
  .bDescriptorType    = USB_DT_CS_INTERFACE,
  .bDescriptorSubType = UVC_VC_OUTPUT_TERMINAL,
  .bTerminalID        = 3,
  .wTerminalType      = cpu_to_le16(0x0101),
  .bAssocTerminal     = 0,
  .bSourceID          = 2,
  .iTerminal          = 0,
};



DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1 /* bControlSize */, 6 /* num of bmaControls */);

static struct UVC_INPUT_HEADER_DESCRIPTOR(1, 6) uvc_input_header = {
  .bLength             = UVC_DT_INPUT_HEADER_SIZE(1, 6),
  .bDescriptorType     = USB_DT_CS_INTERFACE,
  .bDescriptorSubType  = UVC_VS_INPUT_HEADER,
  .bNumFormats         = 0,
  .wTotalLength        = 0, /* dynamic */
  .bEndpointAddress    = 0, /* dynamic */
  .bmInfo              = 0,
  .bTerminalLink       = 3,
  .bStillCaptureMethod = 0,
  .bTriggerSupport     = 0,
  .bTriggerUsage       = 0,
  .bControlSize        = 1,
  .bmaControls[0][0]   = 0,
  .bmaControls[1][0]   = 0,
  .bmaControls[2][0]   = 0,
  .bmaControls[3][0]   = 0,
  .bmaControls[4][0]   = 0,
  .bmaControls[5][0]   = 0,
};

#define UVC_GUID_FORMAT_YUY2 \
  { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
#define UVC_GUID_FORMAT_Y800 \
  { 'Y',  '8',  '0',  '0', 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
#define UVC_GUID_FORMAT_RGGB \
  { 'R',  'G',  'G',  'B', 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
#define UVC_GUID_FORMAT_RGBP \
  { 'R',  'G',  'B',  'P', 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
#define UVC_GUID_FORMAT_BGR3 \
  { 0x7d, 0xeb, 0x36, 0xe4, 0x4f, 0x52, 0xce, 0x11, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70 }

static struct uvc_format_uncompressed uvc_format_yuv = {
  .bLength              = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
  .bDescriptorType      = USB_DT_CS_INTERFACE,
  .bDescriptorSubType   = UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 1,
  .bNumFrameDescriptors = 1,
  .guidFormat           = UVC_GUID_FORMAT_YUY2,
  .bBitsPerPixel        = 16,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterfaceFlags     = 0,
  .bCopyProtect         = 0,
};

static struct uvc_format_uncompressed uvc_format_grey = {
  .bLength              = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
  .bDescriptorType      = USB_DT_CS_INTERFACE,
  .bDescriptorSubType   = UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 2,
  .bNumFrameDescriptors = 1,
  .guidFormat           = UVC_GUID_FORMAT_Y800,
  .bBitsPerPixel        = 8,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterfaceFlags     = 0,
  .bCopyProtect         = 0,
};

static struct uvc_format_mjpeg uvc_format_mjpg = {
  .bLength		        = UVC_DT_FORMAT_MJPEG_SIZE,
  .bDescriptorType	    = USB_DT_CS_INTERFACE,
  .bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
  .bFormatIndex		    = 3,
  .bNumFrameDescriptors	= 1,
  .bmFlags		        = 0,
  .bDefaultFrameIndex	= 1,
  .bAspectRatioX		= 0,
  .bAspectRatioY		= 0,
  .bmInterfaceFlags	    = 0,
  .bCopyProtect		    = 0,
};

static struct uvc_format_uncompressed uvc_format_bayer = {
  .bLength              = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
  .bDescriptorType      = USB_DT_CS_INTERFACE,
  .bDescriptorSubType   = UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 4,
  .bNumFrameDescriptors = 1,
  .guidFormat           = UVC_GUID_FORMAT_RGGB,
  .bBitsPerPixel        = 8,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterfaceFlags     = 0,
  .bCopyProtect         = 0,
};

static struct uvc_format_uncompressed uvc_format_rgb565 = {
  .bLength              = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
  .bDescriptorType      = USB_DT_CS_INTERFACE,
  .bDescriptorSubType   = UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 5,
  .bNumFrameDescriptors = 1,
  .guidFormat           = UVC_GUID_FORMAT_RGBP,
  .bBitsPerPixel        = 16,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterfaceFlags     = 0,
  .bCopyProtect         = 0,
};

static struct uvc_format_uncompressed uvc_format_bgr888 = {
  .bLength              = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
  .bDescriptorType      = USB_DT_CS_INTERFACE,
  .bDescriptorSubType   = UVC_VS_FORMAT_UNCOMPRESSED,
  .bFormatIndex         = 6,
  .bNumFrameDescriptors = 1,
  .guidFormat           = UVC_GUID_FORMAT_BGR3,
  .bBitsPerPixel        = 24,
  .bDefaultFrameIndex   = 1,
  .bAspectRatioX        = 0,
  .bAspectRatioY        = 0,
  .bmInterfaceFlags     = 0,
  .bCopyProtect         = 0,
};



DECLARE_UVC_FRAME_UNCOMPRESSED(1);
DECLARE_UVC_FRAME_MJPEG(1);       

static const struct uvc_color_matching_descriptor uvc_color_matching = {
  .bLength                  = UVC_DT_COLOR_MATCHING_SIZE,
  .bDescriptorType          = USB_DT_CS_INTERFACE,
  .bDescriptorSubType       = UVC_VS_COLORFORMAT,
  .bColorPrimaries          = 1,
  .bTransferCharacteristics = 1,
  .bMatrixCoefficients      = 4,
};

static const struct uvc_descriptor_header * const uvc_control_cls[] = {
  (const struct uvc_descriptor_header *) &uvc_control_header,
  (const struct uvc_descriptor_header *) &uvc_camera_terminal,
  (const struct uvc_descriptor_header *) &uvc_processing,
  (const struct uvc_descriptor_header *) &uvc_output_terminal,
  NULL,
};

static struct uvc_descriptor_header ** uvc_hs_streaming_cls;

/* --------------------------------------------------------------------------
 * USB configuration
 */

enum parse_state { PARSE_DEF, PARSE_FCC, PARSE_W, PARSE_H, PARSE_INTV };

static int parsemodes(void)
{
  unsigned int nmodes, nframes; char * ptr; unsigned int entry = 0;
  enum parse_state state = PARSE_DEF;
  __u32 def = 0, fcc = 0, i, currfcc = ~0U;
  __u16 w = 0, h = 0;
  __u32 * intv; __u8 intvidx = 0U;
  __u8 formatidx = 0U;
  __u8 * currbnf = NULL;
  __u8 bitsperpix = 0U;

  intv = (__u32 *)kmalloc(256 * sizeof(__u32), GFP_KERNEL);
  if (intv == NULL) { printk("PARSEMODES: out of memory\n"); return -1; }


  
  ptr = modes; nmodes = 0; while (*ptr) if (*ptr++ == '/') ++nmodes;

  ptr = modes; nframes = 0; while (*ptr) if (*ptr++ == '-') ++nframes;

  printk("PARSEMODES: found %u modes and %u frames\n", nmodes, nframes);

  uvc_hs_streaming_cls = (struct uvc_descriptor_header **)
    kmalloc( (nmodes + nframes + 3) * sizeof(struct uvc_descriptor_header *), GFP_KERNEL);
  if (uvc_hs_streaming_cls == NULL) { printk("PARSEMODES: out of memory\n"); goto error1; }
  
  uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *) &uvc_input_header;

  ptr = modes;
  while (*ptr)
  {
    switch (*ptr)
    {
    case '/': fcc = 0U; state = PARSE_FCC; break;
    case '-': w = 0U; state = PARSE_W; break;
    case 'x': h = 0U; state = PARSE_H; break;

    case ':':
      if (state == PARSE_INTV) { ++intvidx; intv[intvidx] = 0U; }
      else { intvidx = 0U; intv[0] = 0U; state = PARSE_INTV; }
      break;

    case '0' ... '9':
      switch (state)
      {
      case PARSE_DEF:  def = def * 10 + (*ptr - '0'); break;
      case PARSE_FCC:  fcc = fcc * 10 + (*ptr - '0'); break;
      case PARSE_W:    w = w * 10 + (*ptr - '0'); break;
      case PARSE_H:    h = h * 10 + (*ptr - '0'); break;
      case PARSE_INTV: intv[intvidx] = intv[intvidx] * 10 + (*ptr - '0'); break;
      default: printk("PARSEMODES: internal inconsistency\n"); goto error2;
      }
      break;
      
    case '\n': break;
    case '\r': break;

    default: printk("PARSEMODES: bad format\n"); goto error2;
    }
    
    ++ptr;
    
    if (state == PARSE_INTV && (*ptr == 0 || *ptr == '\r' || *ptr == '\n' || *ptr == '/' || *ptr == '-'))
    {
      __u64 bitsiz, minrate, maxrate;
      
      if (fcc != currfcc)
      {
        ++formatidx;
        currfcc = fcc;
        ++uvc_input_header.bNumFormats;
        
        switch (fcc)
        {
        case V4L2_PIX_FMT_YUYV:
          uvc_format_yuv.bFormatIndex = formatidx;
          currbnf = &uvc_format_yuv.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_yuv);
          bitsperpix = uvc_format_yuv.bBitsPerPixel;
          break;

        case V4L2_PIX_FMT_GREY:
          uvc_format_grey.bFormatIndex = formatidx;
          currbnf = &uvc_format_grey.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_grey);
          bitsperpix = uvc_format_grey.bBitsPerPixel;
          break;

        case V4L2_PIX_FMT_MJPEG:
          uvc_format_mjpg.bFormatIndex = formatidx;
          currbnf = &uvc_format_mjpg.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_mjpg);
          bitsperpix = 16U;
          break;

        case V4L2_PIX_FMT_SRGGB8:
          uvc_format_bayer.bFormatIndex = formatidx;
          currbnf = &uvc_format_bayer.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_bayer);
          bitsperpix = uvc_format_bayer.bBitsPerPixel;
          break;
          
        case V4L2_PIX_FMT_RGB565:
          uvc_format_rgb565.bFormatIndex = formatidx;
          currbnf = &uvc_format_rgb565.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_rgb565);
          bitsperpix = uvc_format_rgb565.bBitsPerPixel;
          break;

        case V4L2_PIX_FMT_BGR24:
          uvc_format_bgr888.bFormatIndex = formatidx;
          currbnf = &uvc_format_bgr888.bNumFrameDescriptors;
          uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)(&uvc_format_bgr888);
          bitsperpix = uvc_format_bgr888.bBitsPerPixel;
          break;

        default: printk("PARSEMODES: unsupported format %u\n", fcc); goto error2;
        }
      }
      else ++(*currbnf);

      bitsiz = (__u64)(bitsperpix) * w * h;
      minrate = bitsiz * 10000000ULL; do_div(minrate, intv[intvidx]);
      maxrate = bitsiz * 10000000ULL; do_div(maxrate, intv[0]);

      
      switch (fcc)
      {
      case V4L2_PIX_FMT_MJPEG:
      {
        struct uvc_frame_mjpeg * frame = (struct uvc_frame_mjpeg *)
          kmalloc(UVC_DT_FRAME_MJPEG_SIZE(intvidx + 1), GFP_KERNEL);
        if (frame == NULL) { printk("PARSEMODES: out of memory\n"); goto error2; }
        memset(frame, 0, UVC_DT_FRAME_MJPEG_SIZE(intvidx + 1));
        
        frame->bLength	                 = UVC_DT_FRAME_MJPEG_SIZE(intvidx + 1);
        frame->bDescriptorType           = USB_DT_CS_INTERFACE;
        frame->bDescriptorSubType        = UVC_VS_FRAME_MJPEG;
        frame->bFrameIndex	             = *currbnf;
        frame->bmCapabilities	         = 0;
        frame->wWidth	                 = cpu_to_le16(w);
        frame->wHeight	                 = cpu_to_le16(h);
        frame->dwMinBitRate	             = cpu_to_le32((__u32)(minrate));
        frame->dwMaxBitRate	             = cpu_to_le32((__u32)(maxrate));
        frame->dwMaxVideoFrameBufferSize = cpu_to_le32((__u32)(bitsiz >> 3));
        frame->dwDefaultFrameInterval    = cpu_to_le32(intv[0]);
        frame->bFrameIntervalType        = intvidx + 1;
        for (i = 0; i <= intvidx; ++i) frame->dwFrameInterval[i] = cpu_to_le32(intv[i]);
        
        uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)frame;
      }
      break;

      default:
      {
        struct uvc_frame_uncompressed * frame = (struct uvc_frame_uncompressed *)
          kmalloc(UVC_DT_FRAME_UNCOMPRESSED_SIZE(intvidx + 1), GFP_KERNEL);
        if (frame == NULL) { printk("PARSEMODES: out of memory\n"); goto error2; }
        memset(frame, 0, UVC_DT_FRAME_UNCOMPRESSED_SIZE(intvidx + 1));

        frame->bLength	                 = UVC_DT_FRAME_UNCOMPRESSED_SIZE(intvidx + 1);
        frame->bDescriptorType           = USB_DT_CS_INTERFACE;
        frame->bDescriptorSubType        = UVC_VS_FRAME_UNCOMPRESSED;
        frame->bFrameIndex	             = *currbnf;
        frame->bmCapabilities	         = 0;
        frame->wWidth	                 = cpu_to_le16(w);
        frame->wHeight	                 = cpu_to_le16(h);
        frame->dwMinBitRate	             = cpu_to_le32((__u32)(minrate));
        frame->dwMaxBitRate	             = cpu_to_le32((__u32)(maxrate));
        frame->dwMaxVideoFrameBufferSize = cpu_to_le32((__u32)(bitsiz >> 3));
        frame->dwDefaultFrameInterval    = cpu_to_le32(intv[0]);
        frame->bFrameIntervalType        = intvidx + 1;
        for (i = 0; i <= intvidx; ++i) frame->dwFrameInterval[i] = cpu_to_le32(intv[i]);
        
        uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *)frame;
      }
      }
    }
  }
  
  uvc_hs_streaming_cls[entry++] = (struct uvc_descriptor_header *) &uvc_color_matching;
  uvc_hs_streaming_cls[entry++] = NULL;

  uvc_input_header.bLength = UVC_DT_INPUT_HEADER_SIZE(1, uvc_input_header.bNumFormats);

  return 0;

 error2:
  for (i = 0; i < entry; ++i)
    if (uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_yuv) &&
        uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_grey) &&
        uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_mjpg) &&
        uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_bayer) &&
        uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_rgb565) &&
        uvc_hs_streaming_cls[i] != (struct uvc_descriptor_header *)(&uvc_format_bgr888))
      kfree(uvc_hs_streaming_cls[i]);
  kfree(uvc_hs_streaming_cls);

 error1:
  kfree(intv);
  
  return -1;
}

#ifdef JEVOIS_STORAGE

static struct fsg_module_parameters fsg_mod_data = {
  .stall = 0 
};
FSG_MODULE_PARAMETERS(/* no prefix */, fsg_mod_data);

static struct fsg_common fsg_common;

#endif

static int __init webcam_config_bind(struct usb_configuration *c)
{
  int status;

  status = parsemodes();
  if (status < 0) return status;

  status = uvc_bind_config(c, uvc_control_cls,
                           (const struct uvc_descriptor_header * const *)uvc_hs_streaming_cls,
                           (const struct uvc_descriptor_header * const *)uvc_hs_streaming_cls,
			   max_bandwidth);
  if (status < 0) return status;

#ifdef JEVOIS_SERIAL
  if (use_serial)
  {
    status = acm_bind_config(c, 0);
    if (status < 0) return status;
  }
#endif

#ifdef JEVOIS_STORAGE
  if (use_storage)
  {
	status = fsg_bind_config(c->cdev, c, &fsg_common);
    if (status < 0) return status;
  }
#endif
  
  return status;
}

static struct usb_configuration webcam_config_driver = {
  .label            = webcam_config_label,
  .bConfigurationValue  = 1,
  .iConfiguration       = 0, /* dynamic */
  .bmAttributes     = USB_CONFIG_ATT_SELFPOWER,
  .bMaxPower        = CONFIG_USB_GADGET_VBUS_DRAW / 2,
};

static int /* __init_or_exit */ webcam_unbind(struct usb_composite_dev *cdev)
{
#ifdef JEVOIS_SERIAL
  if (use_serial) gserial_cleanup();
#endif
  return 0;
}

static int __init webcam_bind(struct usb_composite_dev *cdev)
{
  int ret; void * retp;

  if ((ret = usb_string_id(cdev)) < 0) goto error3;
  webcam_strings[STRING_MANUFACTURER_IDX].id = ret;
  webcam_device_descriptor.iManufacturer = ret;
  
  if ((ret = usb_string_id(cdev)) < 0) goto error3;
  webcam_strings[STRING_PRODUCT_IDX].id = ret;
  webcam_device_descriptor.iProduct = ret;
  
  if ((ret = usb_string_id(cdev)) < 0) goto error3;
  webcam_strings[STRING_DESCRIPTION_IDX].id = ret;
  webcam_config_driver.iConfiguration = ret;
  
#ifdef JEVOIS_SERIAL
  if (use_serial)
  {
    if ((ret = gserial_setup(cdev->gadget, 1)) < 0) goto error;
  }
#endif

#ifdef JEVOIS_STORAGE
  if (use_storage)
  {
	struct fsg_config cfg;

	fsg_config_from_params(&cfg, &fsg_mod_data);
    cfg.vendor_name = "JeVois";
    cfg.product_name = "Smart Camera";
	retp = fsg_common_init(&fsg_common, cdev, &cfg);

    if (IS_ERR(retp)) { ret = PTR_ERR(retp); goto error2; }
  }
#endif

  /* Register our configuration. */
  if ((ret = usb_add_config(cdev, &webcam_config_driver, webcam_config_bind)) < 0) goto error3;

#ifdef JEVOIS_STORAGE
  if (use_storage)
  {
    fsg_common_put(&fsg_common);
  }
#endif
  
  INFO(cdev, "JeVois-A33 Smart Camera Gadget\n");
  return 0;

  
 error3:
#ifdef JEVOIS_STORAGE
  fsg_common_put(&fsg_common);
#endif
  
 error2:
#ifdef JEVOIS_SERIAL
  if (use_serial) gserial_cleanup();
#endif
  
 error:
  return ret;
}

/* --------------------------------------------------------------------------
 * Driver
 */

static struct usb_composite_driver webcam_driver = {
  .name      = "g_jevoisa33",
  .dev       = &webcam_device_descriptor,
  .strings   = webcam_device_strings,
  .max_speed = USB_SPEED_HIGH,
  .unbind    = webcam_unbind,
};

static int __init webcam_init(void)
{
  return usb_composite_probe(&webcam_driver, webcam_bind);
}

static void __exit webcam_cleanup(void)
{
  usb_composite_unregister(&webcam_driver);
}

module_init(webcam_init);
module_exit(webcam_cleanup);

MODULE_AUTHOR("JeVois Inc");
MODULE_DESCRIPTION("JeVois-A33 Smart Camera Gadget");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1.0");

