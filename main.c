#include <project.h>
#include <main.h>
#include "BLEApplications.c"

void StackEventHandler( uint32 eventCode, void *eventParam );
extern uint8 deviceConnected;

/*'startNotification' flag is set when the central device writes to CCC (Client  
* Characteristic Configuration) of the CapSense proximity characteristic to 
* enable notifications */
extern uint8 startNotification;	

/* 'restartAdvertisement' flag is used to restart advertisement */
extern uint8 restartAdvertisement;

/* 'initializeCapSenseBaseline' flag is used to call the function once that initializes 
* all CapSense baseline. The baseline is initialized when the first advertisement 
* is started. This is done so that any external disturbance while powering the kit does 
* not infliuence the baseline value, that may cause wrong readings. */
uint8 initializeCapSenseBaseline = TRUE;

int main()
{
    InitializeSystem();
    
    for(;;)
    {
        /* Place your application code here */
        CyBle_ProcessEvents();
        
        if(TRUE == deviceConnected) 
		{
			/* After the connection, send new connection parameter to the Client device 
			* to run the BLE communication on desired interval. This affects the data rate 
			* and power consumption. High connection interval will have lower data rate but 
			* lower power consumption. Low connection interval will have higher data rate at
			* expense of higher power. This function is called only once per connection. */
			UpdateConnectionParam();
			
			/* If CapSense Initialize Baseline API has not been called yet, call the
			* API and reset the flag. */
			if(initializeCapSenseBaseline)
			{
				/* Reset 'initializeCapSenseBaseline' flag*/
				initializeCapSenseBaseline = FALSE;
				
				/* Initialize all CapSense Baseline */
				CapSense_InitializeAllBaselines();
			}
            
            if(restartAdvertisement)
		    {
			/* Reset 'restartAdvertisement' flag*/
			restartAdvertisement = FALSE;
			
			/* Start Advertisement and enter Discoverable mode*/
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);	
		    }

    }
}


/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
* Summary:
*        Starts the components and initializes CapSense baselines
*
* Parameters:
*  void
*
* Return:
*  void
*

*******************************************************************************/
void InitializeSystem(void)
{
	/* Enable global interrupt mask */
	CyGlobalIntEnable;

	/* Start BLE component and register the CustomEventHandler function. This 
	* function exposes the events from BLE component for application use */
	CyBle_Start(CustomEventHandler);					
	
	/* Set drive mode of the status LED pin to Strong*/
	/*Status_LED_SetDriveMode(Status_LED_DM_STRONG);	*/
	
	#ifdef CAPSENSE_ENABLED
	/* Enable the proximity widget explicitly and start CapSense component. 
	* Initialization of the baseline is done when the first connection 
	* happens  */
	CapSense_EnableWidget(CapSense_BUTTON0__BTN);
    CapSense_EnableWidget(CapSense_BUTTON1__BTN);
	CapSense_Start();
	#endif

		
	/* Start the Button ISR to allow wakeup from sleep */
	isr_button_StartEx(MyISR);
}

/*******************************************************************************
* Function Name: HandleCapSenseProximity
********************************************************************************
* Summary:
*       Read the proximity data from the sensor and update the BLE
*		custom notification value.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void HandleCapSenseProximity(void)
{
	#ifdef CAPSENSE_ENABLED
	/* Static variable to store last proximity value */
	static uint8 lastProximityValue = MAX_PROX_VALUE;
	static uint8 lastProximityValue1 = MAX_PROX_VALUE1;
        
	/* 'proximityValue' stores the proximity value read from CapSense component */
	uint8 proximityValue;	
    uint8 proximityValue1;	

	/* Scan the Proximity Widget */
	/*CapSense_ScanWidget(CapSense_PROXIMITYSENSOR0__PROX);	*/
    CapSense_ScanEnabledWidgets();
	
	/* Wait for CapSense scanning to be complete */
	while(CapSense_IsBusy())
	{
		#ifdef ENABLE_LOW_POWER_MODE
		/* While CapSense is scanning, put CPU to sleep to conserve power */
		CySysPmSleep();
		#endif
	}
	
	/* Update CapSense Baseline */
	CapSense_UpdateEnabledBaselines();	

	/* Get the Diffcount between proximity raw data and baseline */
    /*proximityValue = CapSense_GetDiffCountData(CapSense_PROXIMITYSENSOR0__PROX);*/
    proximityValue = CapSense_GetDiffCountData(CapSense_BUTTON0__BTN);
	proximityValue1 = CapSense_GetDiffCountData(CapSense_BUTTON1__BTN);
    
	/* Check if the proximity data is different from previous */
    
	if(lastProximityValue != proximityValue)				
	{
		/* Send the proximity data to BLE central device by notifications*/
		SendDataOverCapSenseNotification(proximityValue);
        
		/* Update present proximity value */
		lastProximityValue = proximityValue;		
	}
    
    if(lastProximityValue1 != proximityValue1)				
	{
		/* Send the proximity data to BLE central device by notifications*/
		SendDataOverCapSenseNotification1(proximityValue1);
        
		/* Update present proximity value */
		lastProximityValue1 = proximityValue1;		
	}
    
	#endif
    
    
   
}
