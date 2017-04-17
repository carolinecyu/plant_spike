/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
void InitializeSystem(void);
void HandleCapSenseProximity(void);
void CustomEventHandler(uint32 event, void * eventParam);


#define TRUE								1
#define FALSE								0
#define ZERO								0

/* CapSense Proximity value ranges from 0-255*/
#define MAX_PROX_VALUE						0xFF
#define MAX_PROX_VALUE1						0xFF

/* [] END OF FILE */
