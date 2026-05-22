/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ���������
* �����Ը���������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù�������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_wifi_uart
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2022-09-15        ��W            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   RX                  �鿴 zf_device_wifi_uart.h �� WIFI_UART_RX_PIN  �궨��
*                   TX                  �鿴 zf_device_wifi_uart.h �� WIFI_UART_TX_PIN  �궨��
*                   RTS                 �鿴 zf_device_wifi_uart.h �� WIFI_UART_RTS_PIN �궨��
*                   RST                 �鿴 zf_device_wifi_uart.h �� WIFI_UART_RST_PIN �궨��
*                   VCC                 5V ��Դ
*                   GND                 ��Դ��
*                   ������������
*                   ------------------------------------
*********************************************************************************************************************/
#ifndef _zf_device_wifi_uart_h_
#define _zf_device_wifi_uart_h_

#include "zf_common_typedef.h"

#define WIFI_UART_INDEX         (UART_7)                                        // WIFI ģ�� ��ʹ�õ��Ĵ���
#define WIFI_UART_RX_PIN        (UART7_MAP3_TX_E12)                             // ���� WIFI ģ�� RX
#define WIFI_UART_TX_PIN        (UART7_MAP3_RX_E13)                             // ���� WIFI ģ�� TX
#define WIFI_UART_BAUD          (115200)                                        // ģ�鹤��������
  
#define WIFI_UART_RTS_PIN       (E8)                                            // ��������λ����  ָʾ��ǰģ���Ƿ���Խ�������  0-���Լ�������  1-�����Լ�������
#define WIFI_UART_HARDWARE_RST  (0)                                             // �����Ƿ�ʹ��Ӳ����λ���� 0-ʹ��������λ 1-ʹ��Ӳ�� RST
#if WIFI_UART_HARDWARE_RST                                                      // ����ʹ��Ӳ����λ���� �������׳��ֵ�Ƭ����λ���޷�������ʼ��ģ��
#define WIFI_UART_RST_PIN       (D10)                                           // ����Ӳ����λ����
#endif

#define WIFI_UART_BUFFER_SIZE   (256)                                           // ������ջ�������С

#define WIFI_UART_AUTO_CONNECT          (0)                                     // �����Ƿ��ʼ��ʱ����TCP����UDP����    0-������  1-�Զ�����TCP������  2-�Զ�����UDP������  3���Զ�����TCP������

#if     (WIFI_UART_AUTO_CONNECT > 3)
#error "WIFI_UART_AUTO_CONNECT ��ֵֻ��Ϊ [0,1,2,3]"
#else
// ������Ŀ��� IP���˿ںͱ��ض˿�����û�����ʵ�������Լ�����
// #define WIFI_UART_TARGET_IP     "192.168.x.x"
// #define WIFI_UART_TARGET_PORT   "xxxx"
// #define WIFI_UART_LOCAL_PORT    "xxxx"
#endif

typedef enum
{
    WIFI_UART_STATION,                                                          // �豸ģʽ
    WIFI_UART_SOFTAP,                                                           // APģʽ
}wifi_uart_mode_enum;

typedef enum
{
    WIFI_UART_COMMAND,                                                          // ʹ������ķ�ʽ��������
    WIFI_UART_SERIANET,                                                         // ʹ��͸���ķ�ʽ��������
}wifi_uart_transfer_mode_enum;

typedef enum
{
    WIFI_UART_TCP_CLIENT,                                                       // ģ������TCP������
    WIFI_UART_TCP_SERVER,                                                       // ģ����ΪTCP������
    WIFI_UART_UDP_CLIENT,                                                       // ģ������UDP����
}wifi_uart_connect_mode_enum;

typedef enum
{
    WIFI_UART_SERVER_OFF,                                                       // ģ��δ���ӷ�����
    WIFI_UART_SERVER_ON,                                                        // ģ���Ѿ����ӷ�����
}wifi_uart_connect_state_enum;

typedef enum
{
    WIFI_UART_LINK_0,                                                           // ģ�鵱ǰ���� 0
    WIFI_UART_LINK_1,                                                           // ģ�鵱ǰ���� 1
    WIFI_UART_LINK_2,                                                           // ģ�鵱ǰ���� 2
    WIFI_UART_LINK_3,                                                           // ģ�鵱ǰ���� 3
    WIFI_UART_LINK_4,                                                           // ģ�鵱ǰ���� 4
}wifi_uart_link_id_enum;

typedef struct
{
    uint8                           wifi_uart_version[12];                      // �̼��汾         �ַ�����ʽ
    uint8                           wifi_uart_mac[20];                          // ���� MAC ��ַ    �ַ�����ʽ
    uint8                           wifi_uart_local_ip[17];                     // ���� IP ��ַ     �ַ�����ʽ
    uint8                           wifi_uart_local_port[10];                   // �����˿ں�       �ַ�����ʽ
    uint8                           wifi_uart_remote_ip[5][15];                 // Զ�� IP ��ַ     �ַ�����ʽ
    wifi_uart_mode_enum             wifi_uart_mode;                             // WIFI ģʽ
    wifi_uart_transfer_mode_enum    wifi_uart_transfer_mode;                    // ��ǰ����ģʽ
    wifi_uart_connect_mode_enum     wifi_uart_connect_mode;                     // ��������ģʽ
    wifi_uart_connect_state_enum    wifi_uart_connect_state;                    // �������������
}wifi_uart_information_struct;

extern wifi_uart_information_struct wifi_uart_information;


uint8   wifi_uart_disconnected_wifi         (void);                                                                         // �Ͽ� WIFI ����
uint8   wifi_uart_entry_serianet            (void);                                                                         // ��͸��ģʽ
uint8   wifi_uart_exit_serianet             (void);                                                                         // �ر�͸��ģʽ

uint8   wifi_uart_connect_tcp_servers       (char *ip, char *port, wifi_uart_transfer_mode_enum mode);                      // ���� TCP ����
uint8   wifi_uart_connect_udp_client        (char *ip, char *port, char *local_port, wifi_uart_transfer_mode_enum mode);    // ���� UDP ����
uint8   wifi_uart_disconnect_link           (void);                                                                         // �Ͽ����� TCP Server ʹ�ñ��ӿڽ���Ͽ���������
uint8   wifi_uart_disconnect_link_with_id   (wifi_uart_link_id_enum link_id);                                               // TCP Server �Ͽ�ָ������ TCP/UDP Client �������з�Ӧ

uint8   wifi_uart_entry_tcp_servers         (char *port);                                                                   // ���� TCP ������
uint8   wifi_uart_exit_tcp_servers          (void);                                                                         // �ر� TCP ������
uint8   wifi_uart_tcp_servers_check_link    (void);                                                                         // TCP Server ģʽ�¼�鵱ǰ�������� ����ȡ IP

uint32  wifi_uart_send_buffer               (uint8 *buff, uint32 len);                                                      // WIFI ģ�����ݷ��ͺ���
uint32  wifi_uart_tcp_servers_send_buffer   (uint8 *buff, uint32 len, wifi_uart_link_id_enum id);                           // WIFI ģ����Ϊ TCP Server ָ��Ŀ���豸���ͺ���
uint16  wifi_uart_read_buffer               (uint8 *buff, uint16 len);                                                      // WIFI ģ�����ݽ��պ���

void    wifi_uart_callback                  (void);                                                                         // WIFI ģ�鴮�ڻص�����
uint8   wifi_uart_init                      (char *wifi_ssid, char *pass_word, wifi_uart_mode_enum wifi_mode);              // WIFI ģ���ʼ������


#endif
