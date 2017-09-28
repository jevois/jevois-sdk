/************************************************************************
 *
 *	IONSP.H		Definitions for I/O Networks Serial Protocol
 *
 *	Copyright (C) 1997-1998 Inside Out Networks, Inc.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	These definitions are used by both kernel-mode driver and the
 *	peripheral firmware and MUST be kept in sync.
 *
 ************************************************************************/

/************************************************************************

The data to and from all ports on the peripheral is multiplexed
through a single endpoint pair (EP1 since it supports 64-byte
MaxPacketSize). Therefore, the data, commands, and status for
each port must be preceded by a short header identifying the
destination port. The header also identifies the bytes that follow
as data or as command/status info.

Header format, first byte:

    CLLLLPPP
    --------
    | |	 |------ Port Number:	0-7
    | |--------- Length:	MSB bits of length
    |----------- Data/Command:	0 = Data header
				1 = Cmd / Status (Cmd if OUT, Status if IN)

This gives 2 possible formats:


    Data header:		0LLLLPPP	LLLLLLLL
    ============

    Where (LLLL,LLLLLLL) is 12-bit length of data that follows for
    port number (PPP). The length is 0-based (0-FFF means 0-4095
    bytes). The ~4K limit allows the host driver (which deals in
    transfer requests instead of individual packets) to write a
    large chunk of data in a single request. Note, however, that
    the length must always be <= the current TxCredits for a given
    port due to buffering limitations on the peripheral.


    Cmd/Status header:		1ccccPPP	[ CCCCCCCC,	 Params ]...
    ==================

    Where (cccc) or (cccc,CCCCCCCC) is the cmd or status identifier.
    Frequently-used values are encoded as (cccc), longer ones using
    (cccc,CCCCCCCC). Subsequent bytes are optional parameters and are
    specific to the cmd or status code. This may include a length
    for command and status codes that need variable-length parameters.


In addition, we use another interrupt pipe (endpoint) which the host polls
periodically for flow control information. The peripheral, when there has
been a change, sends the following 10-byte packet:

	RRRRRRRRRRRRRRRR
	T0T0T0T0T0T0T0T0
	T1T1T1T1T1T1T1T1
	T2T2T2T2T2T2T2T2
	T3T3T3T3T3T3T3T3

The first field is the 16-bit RxBytesAvail field, which indicates the
number of bytes which may be read by the host from EP1. This is necessary:
(a) because OSR2.1 has a bug which causes data loss if the peripheral returns
fewer bytes than the host expects to read, and (b) because, on Microsoft
platforms at least, an outstanding read posted on EP1 consumes about 35% of
the CPU just polling the device for data.

The next 4 fields are the 16-bit TxCredits for each port, which indicate how
many bytes the host is allowed to send on EP1 for transmit to a given port.
After an OPEN_PORT command, the Edgeport sends the initial TxCredits for that
port.

All 16-bit fields are sent in little-endian (Intel) format.

************************************************************************/


struct int_status_pkt {
	__u16 RxBytesAvail;		
	__u16 TxCredits[MAX_RS232_PORTS];
};


#define GET_INT_STATUS_SIZE(NumPorts) (sizeof(__u16) + (sizeof(__u16) * (NumPorts)))




#define	IOSP_DATA_HDR_SIZE		2
#define	IOSP_CMD_HDR_SIZE		2

#define	IOSP_MAX_DATA_LENGTH		0x0FFF	

#define	IOSP_PORT_MASK			0x07	
#define	IOSP_CMD_STAT_BIT		0x80	

#define IS_CMD_STAT_HDR(Byte1)		((Byte1) & IOSP_CMD_STAT_BIT)
#define IS_DATA_HDR(Byte1)		(!IS_CMD_STAT_HDR(Byte1))

#define	IOSP_GET_HDR_PORT(Byte1)		((__u8) ((Byte1) & IOSP_PORT_MASK))
#define	IOSP_GET_HDR_DATA_LEN(Byte1, Byte2)	((__u16) (((__u16)((Byte1) & 0x78)) << 5) | (Byte2))
#define	IOSP_GET_STATUS_CODE(Byte1)		((__u8) (((Byte1) &  0x78) >> 3))


#define	IOSP_BUILD_DATA_HDR1(Port, Len)		((__u8) (((Port) | ((__u8) (((__u16) (Len)) >> 5) & 0x78))))
#define	IOSP_BUILD_DATA_HDR2(Port, Len)		((__u8) (Len))


#define	IOSP_BUILD_CMD_HDR1(Port, Cmd)		((__u8) (IOSP_CMD_STAT_BIT | (Port) | ((__u8) ((Cmd) << 3))))



#define	IOSP_WRITE_UART_REG(n)	((n) & 0x07)


#define	IOSP_EXT_CMD			0x09	



#define	IOSP_CMD_OPEN_PORT		0x00	
#define	IOSP_CMD_CLOSE_PORT		0x01	
#define	IOSP_CMD_CHASE_PORT		0x02	
#define IOSP_CMD_SET_RX_FLOW		0x03	
#define IOSP_CMD_SET_TX_FLOW		0x04	
#define IOSP_CMD_SET_XON_CHAR		0x05	
#define IOSP_CMD_SET_XOFF_CHAR		0x06	
#define IOSP_CMD_RX_CHECK_REQ		0x07	


#define IOSP_CMD_SET_BREAK		0x08	
#define IOSP_CMD_CLEAR_BREAK		0x09	



#define MAKE_CMD_WRITE_REG(ppBuf, pLen, Port, Reg, Val)			\
do {									\
	(*(ppBuf))[0] = IOSP_BUILD_CMD_HDR1((Port),			\
					    IOSP_WRITE_UART_REG(Reg));	\
	(*(ppBuf))[1] = (Val);						\
									\
	*ppBuf += 2;							\
	*pLen  += 2;							\
} while (0)

#define MAKE_CMD_EXT_CMD(ppBuf, pLen, Port, ExtCmd, Param)		\
do {									\
	(*(ppBuf))[0] = IOSP_BUILD_CMD_HDR1((Port), IOSP_EXT_CMD);	\
	(*(ppBuf))[1] = (ExtCmd);					\
	(*(ppBuf))[2] = (Param);					\
									\
	*ppBuf += 3;							\
	*pLen  += 3;							\
} while (0)





#define IOSP_RX_FLOW_RTS		0x01
#define IOSP_RX_FLOW_DTR		0x02
#define IOSP_RX_FLOW_DSR_SENSITIVITY	0x04

#define IOSP_RX_FLOW_XON_XOFF		0x08



#define IOSP_TX_FLOW_CTS		0x01
#define IOSP_TX_FLOW_DSR		0x02
#define IOSP_TX_FLOW_DCD		0x04
#define IOSP_TX_FLOW_XON_XOFF		0x08

#define IOSP_TX_FLOW_XOFF_CONTINUE	0x10

#define IOSP_TX_TOGGLE_RTS		0x20













/****************************************************
 *	SSSS values for 2-byte status messages (0-8)
 ****************************************************/

#define	IOSP_STATUS_LSR			0x00


#define	IOSP_STATUS_MSR			0x01




/****************************************************
 *	SSSS values for 3-byte status messages (8-A)
 ****************************************************/

#define	IOSP_STATUS_LSR_DATA		0x08


#define	IOSP_EXT_STATUS			0x09



#define	IOSP_EXT_STATUS_CHASE_RSP	0
#define	IOSP_EXT_STATUS_CHASE_PASS	0
#define	IOSP_EXT_STATUS_CHASE_FAIL	1


#define	IOSP_EXT_STATUS_RX_CHECK_RSP	1


#define IOSP_STATUS_OPEN_RSP		0x0A



#define GET_TX_BUFFER_SIZE(P2) (((P2) + 1) * 64)




/****************************************************
 *	SSSS values for 4-byte status messages
 ****************************************************/

#define IOSP_EXT4_STATUS		0x0C






#define	IOSP_GET_STATUS_LEN(code)	((code) < 8 ? 2 : ((code) < 0x0A ? 3 : 4))

#define	IOSP_STATUS_IS_2BYTE(code)	((code) < 0x08)
#define	IOSP_STATUS_IS_3BYTE(code)	(((code) >= 0x08) && ((code) <= 0x0B))
#define	IOSP_STATUS_IS_4BYTE(code)	(((code) >= 0x0C) && ((code) <= 0x0D))

