#include "rc522.h"
#include "pubdata.h"
#define  MAXRLEN            30
#define  COM_BUF_LEN    128
void write_rc522_reg(u8 reg_addr, u8 data);
void ClearBitMask(unsigned char reg,unsigned char mask);
void SetBitMask(unsigned char reg,unsigned char mask);
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit);




void delay_us(uint32_t nus)
{
  u32 ticks;
  u32 told,tnow,tcnt=0u;
  u32 reload=SysTick->LOAD;
  ticks=nus*(SystemCoreClock/1000000u);
  told=SysTick->VAL;
  while(1)
  {
    tnow=SysTick->VAL;
    if(tnow!=told)
    {
      if(tnow<told)
      {
        tcnt+=told-tnow;
      }
      else 
      {
        tcnt+=reload-tnow+told;
      }
      told=tnow;
      if(tcnt>=ticks)
      {
        break;
      }
    }
  }
}

 u8 read_rc522_reg(u8 reg_addr)
{
     u8 i, addr;
     u8 recv = 0u;

    RC522_SCK_0;
    RC522_CS_0;
    addr = ((reg_addr<<1)&0x7Eu)|0x80u;               //确保首位为1，表示读

    for(i=8u;i>0u;i--)
    {
        if(addr&0x80u)
        {
          RC522_SI_1;
        }
        else
        {
          RC522_SI_0;
        }

        RC522_SCK_1;
        addr <<= 1;
        RC522_SCK_0;
        delay_us(10u);
    }
    for(i=8u;i>0u;i--)
    {
        RC522_SCK_1;
        recv <<= 1;
        recv |= RC522_SO;
        RC522_SCK_0;
        delay_us(10u);
    }

    RC522_CS_1;
    RC522_SCK_1;
    delay_us(10u);
    return recv;
}

void write_rc522_reg(u8 reg_addr, u8 data)
{
    u8 i, addr;

    RC522_SCK_0;
    RC522_CS_0;
    addr = ((reg_addr<<1)&0x7Eu);

    for(i=8u;i>0u;i--)
    {
        if(addr&0x80u)
        {
          RC522_SI_1;
        }
        else
        {
          RC522_SI_0;
        }

        RC522_SCK_1;
        addr <<= 1;
        RC522_SCK_0;
        delay_us(10u);
    }

    for(i=8u;i>0u;i--)
    {
        if(data&0x80u)
        {
          RC522_SI_1;
        }
        else
        {
          RC522_SI_0;
        }

        RC522_SCK_1;
        data <<= 1;
        RC522_SCK_0;
        delay_us(10u);
    }
    RC522_CS_1;
    RC522_SCK_1;
    delay_us(10u);
}





/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0u;
    tmp = read_rc522_reg(reg);
    write_rc522_reg(reg, tmp & (u8)(~mask));  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0u;
    tmp = read_rc522_reg(reg);
    write_rc522_reg(reg,tmp | mask);  // set bit mask
}
/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask((u8)DivIrqReg,0x04u);
    write_rc522_reg((u8)CommandReg,(u8)PCD_IDLE);
    SetBitMask((u8)FIFOLevelReg,0x80u);
    for (i=0u; i<len; i++)
    {   write_rc522_reg((u8)FIFODataReg, *(pIndata+i));   }
    write_rc522_reg((u8)CommandReg, (u8)PCD_CALCCRC);
    i = 0xFFu;
    do
    {
        n = read_rc522_reg((u8)DivIrqReg);
        i--;
    }
    while ((i!=0u) && (!(n&0x04u)));
    pOutData[0] = read_rc522_reg((u8)CRCResultRegL);
    pOutData[1] = read_rc522_reg((u8)CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit)
{
    char status = (char)MI_ERR;
    unsigned char irqEn   = 0x00u;
    unsigned char waitFor = 0x00u;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12u;      //允许空闲中断和错误中断
          waitFor = 0x10u;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77u;      //允许发送和接收中断，定时中断
          waitFor = 0x30u;
          break;
       default:
         break;
    }

    write_rc522_reg((u8)ComIEnReg,irqEn|0x80u);
    ClearBitMask((u8)ComIrqReg,0x80u);
    write_rc522_reg((u8)CommandReg,(u8)PCD_IDLE);
    SetBitMask((u8)FIFOLevelReg,0x80u);

	for (i=0u; i<InLenByte; i++){
		write_rc522_reg((u8)FIFODataReg, pInData[i]);
	}

	write_rc522_reg((u8)CommandReg, Command);


    if (Command == (u8)PCD_TRANSCEIVE)
    {    SetBitMask((u8)BitFramingReg,0x80u);  }

     i = 600u;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
         n = read_rc522_reg((u8)ComIrqReg);
         i--;
         //osDelay(1);
    }while ((i!=0u) && (!(n&0x01u)) &&( !(n&waitFor)));

    ClearBitMask((u8)BitFramingReg,0x80u);

    if (i!=0u)
    {
         if(!(read_rc522_reg((u8)ErrorReg)&0x1Bu))
         {
             status = (u8)MI_OK;
             if (n & irqEn & 0x01u)
             {   status = (char)MI_NOTAGERR;   }
             if (Command == (u8)PCD_TRANSCEIVE)
             {
               	n = read_rc522_reg((u8)FIFOLevelReg);         //存储在此寄存器中的数据位数
              	lastBits = read_rc522_reg((u8)ControlReg) & 0x07u;
                if (lastBits)
                {   *pOutLenBit = (n-1u)*8u + lastBits;   }
                else
                {   *pOutLenBit = n*8u;   }
                if (n == 0u)
                {   n = 1u;    }
                if (n > (u8)MAXRLEN)
                {   n = (u8)MAXRLEN;   }
                for (i=0u; i<n; i++)
                {   pOutData[i] = read_rc522_reg((u8)FIFODataReg);    }
            }
         }
         else
         {
           status = (char)MI_ERR;
         }

   }

	if(Command!=(u8)PCD_AUTHENT){//
	   SetBitMask((u8)ControlReg,0x80u);           // stop timer now
	   write_rc522_reg((u8)CommandReg,(u8)PCD_IDLE);
	}
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;
   unsigned int  unLen = 0u;
   unsigned char ucComMF522Buf[MAXRLEN];
   ClearBitMask((u8)Status2Reg,0x08u);
   write_rc522_reg((u8)BitFramingReg,0x07u);

   SetBitMask((u8)TxControlReg,0x03u);

   ucComMF522Buf[0] = req_code;

   status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,1u,ucComMF522Buf,&unLen);
   if ((status == (char)MI_OK) && (unLen == 0x10u))
   {
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }else{
   		status = (char)MI_ERR;
	}

   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0u;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask((u8)Status2Reg,0x08u);
    write_rc522_reg((u8)BitFramingReg,0x00u);
    ClearBitMask((u8)CollReg,0x80u);

    ucComMF522Buf[0] = (u8)PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20u;

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,2u,ucComMF522Buf,&unLen);

    if (status == (char)MI_OK)
    {
    	 for (i=0u; i<4u; i++)
         {
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = (char)MI_ERR;    }
    }

    SetBitMask((u8)CollReg,0x80u);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = (u8)PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70u;
    ucComMF522Buf[6] = 0u;
    for (i=0u; i<4u; i++)
    {
    	ucComMF522Buf[i+2u] = *(pSnr+i);               //存储卡片序列号
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7u,&ucComMF522Buf[7]);

    ClearBitMask((u8)Status2Reg,0x08u);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,9u,ucComMF522Buf,&unLen);

    if ((status == (char)MI_OK) && (unLen == 0x18u))
    {   status = (char)MI_OK;  }
    else
    {   status = (char)MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0u; i<6u; i++)
    {    ucComMF522Buf[i+2u] = *(pKey+i);   }
    for (i=0u; i<4u; i++)
    {    ucComMF522Buf[i+8u] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6);
 //   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522((u8)PCD_AUTHENT,ucComMF522Buf,12u,ucComMF522Buf,&unLen);
    if (((!(read_rc522_reg((u8)Status2Reg) & 0x08u)) ||(status != (char)MI_OK)))
    {   status = (char)MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = (u8)PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);
    if ((status == (u8)MI_OK) && (unLen == 0x90u))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0u; i<16u; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = (char)MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = (u8)PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);

    if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
    {   status = (char)MI_ERR;   }

    if (status == (char)MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0u; i<16u; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16u,&ucComMF522Buf[16]);

        status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,18u,ucComMF522Buf,&unLen);
        if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
        {   status = (char)MI_ERR;   }
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);

    if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
    {   status = (char)MI_ERR;   }

    if (status == (char)MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4u);
 //       for (i=0; i<16; i++)
 //       {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4u,&ucComMF522Buf[4]);
        unLen = 0u;
        status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,6u,ucComMF522Buf,&unLen);
        if (status != (char)MI_ERR)
        {    status = (char)MI_OK;    }
    }

    if (status == (char)MI_OK)
    {
        ucComMF522Buf[0] = (u8)PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

        status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);

        if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
        {   status = (char)MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask((u8)TxControlReg, 0x03u);
}

/////////////////////////////////////////////////////////////////////
//开启天线
/////////////////////////////////////////////////////////////////////
//void PcdAntennaOn(void)
//{
//    SetBitMask(TxControlReg, 0x03);
//}

void PcdAntennaOn(void) {
    uint8_t temp;

    temp = read_rc522_reg((u8)TxControlReg);
    if (!(temp & 0x03u)) 
    {
      SetBitMask((u8)TxControlReg, 0x03u);
    }
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = (u8)PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);

    if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
    {   status = (char)MI_ERR;   }

    if (status == (char)MI_OK)
    {
        ucComMF522Buf[0] = 0u;
        ucComMF522Buf[1] = 0u;
        ucComMF522Buf[2] = 0u;
        ucComMF522Buf[3] = 0u;
        CalulateCRC(ucComMF522Buf,4u,&ucComMF522Buf[4]);

        status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,6u,ucComMF522Buf,&unLen);
        if (status != (char)MI_ERR)
        {    status = (char)MI_OK;    }
    }

    if (status != (char)MI_OK)
    {    return (char)MI_ERR;   }

    ucComMF522Buf[0] = (u8)PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);

    status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Buf,&unLen);

    if ((status != (char)MI_OK) || (unLen != 4u) || ((ucComMF522Buf[0] & 0x0Fu) != 0x0Au))
    {   status = (char)MI_ERR;   }

    return status;
}


/*************************************CPU卡专用指令**************************/
u8 Pcb = 0x00u;          //CPU卡APDU指令分组号
void PcdSwitchPCB(void);

//*************************************************************************
// 函数名  ：PcdSwitchPCB(void)
// 描述   ：切换分组号
// 入口   ：
// 出口   ：
// 返回   ：成功返回MI_OK
//*************************************************************************
void PcdSwitchPCB(void)
{
  switch(Pcb)
  {
    case 0x00:
      Pcb=0x0Au;
      break;
    case 0x0A:
      Pcb=0x0Bu;
      break;
    case 0x0B:
      Pcb=0x0Au;
      break;
    default:
      break;
  }
}

//*************************************************************************
// 函数名  ：PcdRats
// 描述   ：转入APDU命令格式
// 入口   ： 无
// 出口   ： 无
// 返回   ：成功返回MI_OK
//*************************************************************************

char PcdRats(void)
{
  char status =(char)MI_ERR;
  unsigned int unLen;
  u8 ucComMF522Buf[64];
  u8 ucComMF522Rec[64];
//int i;
  ClearBitMask((u8)Status2Reg,0x08u);  // 清空校验成功标志,清除MFCrypto1On位

  memset(ucComMF522Buf, 0x00, (u8)MAXRLEN);
  ucComMF522Buf[0] = 0xE0u;
  ucComMF522Buf[1] = 0x51u;
  CalulateCRC(ucComMF522Buf,2u,&ucComMF522Buf[2]);
  status = PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,4u,ucComMF522Rec,&unLen);

  if (status == (char)MI_OK)
  {
    status = (char)MI_OK;
  }
  else
  {
    status = (char)MI_ERR;
  }
  return status;                //返回结果
}


//*************************************************************************
// 函数名  ：Pcd_Cmd
// 描述   ：执行上位机发来的指令
// 入口   ：pDataIn : 要执行的指令 In_Len  指令数据长度
// 出口   ：pDataOut：输出执行后的返回数据   Out_Len输出的数据长度
// 返回   ：MI_OK
//*************************************************************************

u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len)
{
  s8 status =MI_ERR;
  unsigned int  unLen=0u;
  u8 ucComMF522Buf[64];
  u8 i;

  ClearBitMask((u8)Status2Reg,0x08u);          // 清空校验成功标志,清除MFCrypto1On位
  memset(ucComMF522Buf, 0x00, 64u);

  PcdSwitchPCB();
  ucComMF522Buf[0] = Pcb;
  ucComMF522Buf[1] = 0x01u;
  ucComMF522Buf[2] = pDataIn[0];        // CLA
  ucComMF522Buf[3] = pDataIn[1];        // INS
  ucComMF522Buf[4] = pDataIn[2];        // P1
  ucComMF522Buf[5] = pDataIn[3];        // P2
  ucComMF522Buf[6] = pDataIn[4];        // LEN

  for(i=0u;i<ucComMF522Buf[6];i++)       //DATA
  {
    ucComMF522Buf[7u+i] = pDataIn[5u+i];
  }
  CalulateCRC(ucComMF522Buf,In_Len+2u,&ucComMF522Buf[In_Len+2u]); // 生成发送内容的CRC校验,保存到最后两个字节

  status = (s8)PcdComMF522((u8)PCD_TRANSCEIVE,ucComMF522Buf,In_Len+4u,pDataOut,&unLen);

  if (status == MI_OK)
  {
    Out_Len[0] = (u8)(unLen/8u-4u);  //接收到数据的长度,不要前面和后面的各两个字节，才是返回的有用数据
    return (char)MI_OK;
  }
  else
  {
      Out_Len[0] = (u8)(unLen/8u-4u);
    if((pDataOut[2]==0x90u)&&(pDataOut[3]==0x00u))
    {
      return (char)MI_OK;
    }
    else
    {
      return (char)MI_ERR;
    }

  }
}

//uint8_t g_rc522_test[8] = 0;
bool init_mfrc522(void)
{
    RC522_RST_0;
    delay_us(100u);
    RC522_RST_1;
    delay_us(200u);

    write_rc522_reg((u8)RFCfgReg,0x58u);
    write_rc522_reg((u8)ModeReg,0x3Du);            //和Mifare卡通讯，CRC初始值0x6363
    write_rc522_reg((u8)TReloadRegL,30u);          //16位定时器自动重装值低8位
    write_rc522_reg((u8)TReloadRegH,0u);           //16位定时器自动重装值高8位
    write_rc522_reg((u8)TModeReg,0x8Du);
    write_rc522_reg((u8)TPrescalerReg,0x3Eu);
    write_rc522_reg((u8)TxAutoReg,0x40u);
    Pcb = 0x00u;
    
    //test code
    //g_rc522_test[0] = read_rc522_reg((u8)RFCfgReg);
    //g_rc522_test[1] = read_rc522_reg((u8)ModeReg);
    //g_rc522_test[2] = read_rc522_reg((u8)TReloadRegL);
    //g_rc522_test[3] = read_rc522_reg((u8)TReloadRegH);

    return true;
}







void Card_Task(void const * argument)
{
  /* USER CODE BEGIN Card_Task */
  //init_mfrc522();
  /* Infinite loop */
  for(;;)
  {
//      if((status=PcdRequest(REQ_ALL,CardType))==MI_OK)//寻卡
//       {
//            printf("xun ka ok \r\n");
//            
//            status=PcdAnticoll(CardUID);//防冲撞
//          
//          if(status==MI_OK)
//          {
//            
//            printf("chong zhuang  ok \r\n");
//            
//              
//              if((status=PcdSelect(CardUID))==MI_OK)//选卡
//                {
//                  printf("UID：%02x %02x %02x %02x\r\n",CardUID[0],CardUID[1],CardUID[2],CardUID[3]);
//                  printf("xuan ka   ok \r\n");
//                
//                  status = PcdAuthState((u8)KEYA, 0x08, KEY, CardUID); //验证卡片密码
//                  status = PcdRead(0x08, CardID);//读卡ID
//                  printf("CardID:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",CardID[8],CardID[9],CardID[10],CardID[11],CardID[12],CardID[13],CardID[14],CardID[15]);
//                  HAL_Delay(100);
//                  if(status==MI_OK)
//                    {
//                        for(block = 0; block< 4 ; block++)//读取卡的内容
//                        {
//                          status = PcdRead((section*4+block), block_value);  
//                          printf("SQ:%d , QK:%d     ",section,block);
//                          printf("passA	");
//                          for (int i = 0 ; i<6; i++)
//                          {
//                          printf("%02X ",block_value[i]);
//                          }
//                          printf(" Value ");
//                          for (int i = 6; i<10; i++)
//                          {
//                          printf("%02X ",block_value[i]);
//                          }
//                          printf(" passB ");
//                          for (int i = 10; i<16; i++)
//                          {    
//                          printf("%02X ",block_value[i]);
//                          }
//                          printf("\r\n");
//                         }
//
//                    }
//                }
//                else
//                {
//                  
//                   printf("PcdSelect is error");
//                }
//          }
//          else
//         {
//            
//            printf("PcdAnticoll is error");
//            
//         }
//        }
//        else
//       {
//          
//          printf("xun card is error");
//       }
     HAL_Delay(1000);
  }
  /* USER CODE END Card_Task */
}

