#ifndef   _RC522_H_
#define   _RC522_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "main.h"
#include "stdbool.h"
#include "pubdata.h"


#define    RC522_RST_0   HAL_GPIO_WritePin(MCU_RC522_NIRST_GPIO_Port, MCU_RC522_NIRST_Pin, GPIO_PIN_RESET)
#define    RC522_RST_1   HAL_GPIO_WritePin(MCU_RC522_NIRST_GPIO_Port, MCU_RC522_NIRST_Pin, GPIO_PIN_SET)


#define    RC522_CS_0    HAL_GPIO_WritePin(MCU_RC522_NSS_GPIO_Port, MCU_RC522_NSS_Pin, GPIO_PIN_RESET)
#define    RC522_CS_1    HAL_GPIO_WritePin(MCU_RC522_NSS_GPIO_Port, MCU_RC522_NSS_Pin, GPIO_PIN_SET)

#define    RC522_SI_0    HAL_GPIO_WritePin(MCU_RC522_MOSI_GPIO_Port, MCU_RC522_MOSI_Pin, GPIO_PIN_RESET)
#define    RC522_SI_1    HAL_GPIO_WritePin(MCU_RC522_MOSI_GPIO_Port, MCU_RC522_MOSI_Pin, GPIO_PIN_SET)

#define    RC522_SO      HAL_GPIO_ReadPin(MCU_RC522_MISO_GPIO_Port, MCU_RC522_MISO_Pin)

#define    RC522_SCK_0   HAL_GPIO_WritePin(MCU_RC522_SCK_GPIO_Port, MCU_RC522_SCK_Pin, GPIO_PIN_RESET)
#define    RC522_SCK_1   HAL_GPIO_WritePin(MCU_RC522_SCK_GPIO_Port, MCU_RC522_SCK_Pin, GPIO_PIN_SET)


/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬      ����֮�������ȡ
#define PICC_REQALL           0x52               //Ѱ��������ȫ����       �ȴ���Ƭ�뿪���÷�Χ
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
///////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00
#define     CommandReg            0x01
#define     ComIEnReg             0x02
#define     DivlEnReg             0x03
#define     ComIrqReg             0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2
#define     RFU20                 0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     RFU3C                 0x3C
#define     RFU3D                 0x3D
#define     RFU3E                 0x3E
#define     RFU3F		          0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_NOTAGERR                    (-1)
#define MI_ERR                         (-2)


#define  ZLG600Mode   2                      //1��Uart�������Զ����ģ��   2�������ʹ̶�19200
#define  delaytime    200 //50                      //������ʱʱ��  ��ʾ��ʱN����

//============= �������� ==========================================================================
#define  ZLG522S                   0
#define  ZLG522SP                  1
#define  ZLG600SP                  2
//============= �������� ==========================================================================
//����A�Ϳ�����
#define REQ_STD                     0x26        // ����(IDLE ��)������  
#define REQ_ALL                     0x52        // ����(ALL  ��)������
#define SEL_CODE_LV1                0x93        // ����ײ/ѡ��ȼ�1������
#define SEL_CODE_LV2                0x95        // ����ײ/ѡ��ȼ�2������
#define SEL_CODE_LV3                0x97        // ����ײ/ѡ��ȼ�3������

#define KEYA                        0x60
#define KEYB                        0x61
#define DECREMENT                   0xc0
#define INCREMENT                   0xc1
#define RESTORE                     0xc2
#define ISO14443A                   0x00//������IC���ӿ�Э��
#define ISO14443B                   0x04
// ͨ����������
#define CMD_TYPE_DVC_CONTROL        1           // �豸����������
#define CMD_TYPE_TYPE_A             2           // TypeA(����Mifare��)������                
#define CMD_TYPE_ISO7816            5           // ISO7816������
#define CMD_TYPE_COMM_PROT          6           // ͨ��Э��(����ISO14443-4)������
#define CMD_TYPE_PLUS_CPU           7           // PLUS CPU��(������Mifare��)������
// ����������

// ���建������С
#define MAX_SER_SIZE                200//72//64+2+6

#define ZLG522S_DELAY_20MS          2
#define ZLG522S_DELAY_500MS         50
#define ZLG522S_DELAY_1000MS        1000
// ���崮�����ݻ��������ַ���Ԫ����ֵ
#define FRAMELEN                    0               // ֡����
#define CMDTYPE                     1               // ����
#define COMMAND                     2               // ����
#define STATUS                      2               // ��Ӧ״̬
#define LENGTH                      3               // ���ݳ���
#define DATA                        4               // ������ʼλ��
#define PRTCLEN                     6               // Э���ַ�����

// ͨ��Э�鳣��
#define STX                         0x20
#define ETX                         0x03
#define ACK                         0x06

#define    TRUE     1
#define    FALSE    0





bool init_mfrc522(void);
 u8 read_rc522_reg(u8 reg_addr);
void PcdAntennaOff(void);
void PcdAntennaOn(void);
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
char PcdAnticoll(unsigned char *pSnr);
char PcdSelect(unsigned char *pSnr);
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
char PcdRead(unsigned char addr,unsigned char *pData);
char PcdWrite(unsigned char addr,unsigned char *pData);
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue);
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);
void Card_Task(void const * argument);

/***********CPU��ר��ָ��*************/
char PcdRats(void);
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len);
#endif