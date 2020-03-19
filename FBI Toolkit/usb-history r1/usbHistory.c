/*
 *	usbHistory.c - 	USB History Dumb
 *					a tool to extract USB Trace Evidence
 *					(c) nabiy . http://nabiy.sdf1.org
 *	This program uses non-standard generic string functions
 *	compile with Pelles C http://www.smorgasbordet.com/pellesc/
 *	or switch them with standard strings functions.
 */
#define WIN32_LEAN_AND_MEAN
#define WIN32_DEFAULT_LIBS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE 16383 //16,383 characters for a name (260 ANSI)
#define USBSTOR "SYSTEM\\CurrentControlSet\\Enum\\USBSTOR"
#define DEVINTERFACE_DISK "SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
#define DEVINTERFACE_VOLUME "SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53F5630D-B6BF-11D0-94F2-00A0C91EFB8B}"
#define MAX_VALUE_NAME 1638
/* a Value can have 1MB standard format win Me/98/95 only 16,300 bytes */

struct usbInfo { /* usb structure info */
	TCHAR		cKeyName[MAX_KEY_LENGTH] ; 
	TCHAR		cDeviceID[MAX_KEY_LENGTH] ; 
	SYSTEMTIME	stStamp ; // should be creation time of first use
	struct instanceInfo * instance;
} ;

struct instanceInfo { /* each instance record from registry */
	TCHAR		cInstanceID[MAX_KEY_LENGTH] ; 
	TCHAR		cFriendlyName[MAX_VALUE] ; 
	TCHAR		cDriver[MAX_VALUE] ; // Diver Key - this maches
	TCHAR		cParentIdPrefix[MAX_VALUE] ; 
	TCHAR		cHardwareID[MAX_VALUE] ; 
	TCHAR		cLastDriveLetter[MAX_VALUE_NAME] ;
	SYSTEMTIME	stDiskStamp ; // Last write time of instance entry 
	SYSTEMTIME  stVolumeStamp ; // in Device Class Key {53f56307-b6bf-11d0-94f2-00a0c91efb8b}
	struct instanceInfo * next ; // pointer to next instanceInfo if there is more than one
} ;

int countSubKeys( const TCHAR hMainKey[] ) ;
void getDeviceList( struct usbInfo * ) ;
void cleanDeviceList( struct usbInfo * ) ;

int _tmain (int argc, LPTSTR argv [])
{

	int iSubKeyCount = countSubKeys( USBSTOR ) ;
	struct usbInfo deviceList[ iSubKeyCount ] ;
	struct instanceInfo * current ;

	_tprintf(TEXT("\nUSB History Dump\nby nabiy (c)2008\n")) ;

	getDeviceList(deviceList) ;

	for (int i = 0; i < iSubKeyCount; i++) {
		current = deviceList[i].instance ;

		_tprintf(TEXT("\n(%d) --- %s\n\n"), i+1, current->cFriendlyName) ;
		_tprintf(TEXT("\t\tinstanceID: %s\n"), current->cInstanceID) ;
		_tprintf(TEXT("\t\tParentIdPrefix: %s\n"), current->cParentIdPrefix) ;
		if (_tcsstr(current->cLastDriveLetter, "Dos") != NULL )
			_tprintf(TEXT("\t\tLast Mounted As: %s\n"), current->cLastDriveLetter) ;
		_tprintf(TEXT("\t\tDriver:%s\n"), current->cDriver) ;
		_tprintf(TEXT("\t\tDisk Stamp: %02i/%02i/%04i %02i:%02i\n"), current->stDiskStamp.wMonth, current->stDiskStamp.wDay, 
				current->stDiskStamp.wYear, current->stDiskStamp.wHour, current->stDiskStamp.wMinute) ;
		_tprintf(TEXT("\t\tVolume Stamp: %02i/%02i/%04i %02i:%02i\n"), current->stVolumeStamp.wMonth, current->stVolumeStamp.wDay, 
				current->stVolumeStamp.wYear, current->stVolumeStamp.wHour, current->stVolumeStamp.wMinute) ;		
		while (current->next != NULL) {
			current = current->next ;
			_tprintf(TEXT("\n\t\tinstanceID: %s\n"), current->cInstanceID) ;
			_tprintf(TEXT("\t\tParentIdPrefix: %s\n"), current->cParentIdPrefix) ;
			if (_tcsstr(current->cLastDriveLetter, "Dos") != NULL )
				_tprintf(TEXT("\t\tLast Mounted As: %s\n"), current->cLastDriveLetter) ;
			_tprintf(TEXT("\t\tDriver: %s\n"), current->cDriver) ;
			_tprintf(TEXT("\t\tDisk Stamp: %02i/%02i/%04i %02i:%02i\n"), current->stDiskStamp.wMonth, current->stDiskStamp.wDay, 
					current->stDiskStamp.wYear, current->stDiskStamp.wHour, current->stDiskStamp.wMinute) ;
			_tprintf(TEXT("\t\tVolume Stamp: %02i/%02i/%04i %02i:%02i\n"), current->stVolumeStamp.wMonth, current->stVolumeStamp.wDay, 
					current->stVolumeStamp.wYear, current->stVolumeStamp.wHour, current->stVolumeStamp.wMinute) ;
		}
	}

	cleanDeviceList(deviceList) ;

	return 0 ;
}

int countSubKeys( const TCHAR hMainKey[] ) 
{
	/* 
	   This function counts the total subkeys of a given registry key in HKLM
	   we open a Registry Key, Query the Key for the number of subkeys and
	   then we close the key.
	*/

	HKEY 		hKey ;
	DWORD		dwSubKeys = 0 ;		

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, hMainKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL ) ;

		return (dwSubKeys) ;
	}

	RegCloseKey (hKey) ;

	return 0 ;
}

void getDeviceList( struct usbInfo * usbList ) 
{

	HKEY 		hKey, hInstanceKey, hControlKey ;
	DWORD		dwName = MAX_KEY_LENGTH ;
	TCHAR		cSubKeyName[MAX_PATH +1] ;			 	// buffer for subkey name
	DWORD		dwSubKeys = 0 ;							// number of subkeys
	DWORD		dwValues = 0 ;							// number of Values
	DWORD		dwMaxValueNameLen = MAX_VALUE_NAME;		// Max Value Name Length
	DWORD		dwMaxValueLen = MAX_VALUE;				// Max Value Length
	FILETIME	ft ;									// last write time (file time structure)
	SYSTEMTIME	stGMT, stLocal ; // system time for readability
	DWORD 		ValType, ValNameLen, ValLen;
	LPTSTR 		ValName;
	LPBYTE 		Val;
	int iSubKeyCount = countSubKeys( USBSTOR ) ;

	struct instanceInfo * prev, * current ;
	/*
	   Open USBSTOR, get info and SubKeys for each device
    */
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT(USBSTOR), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

		// If there are subkeys, Enumerate untill RegEnumKeyEx fails
		if (dwSubKeys) {

			for (int i = 0; i < dwSubKeys; i++){

				dwName = MAX_KEY_LENGTH ;

				if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {
					TCHAR deviceKeyName[MAX_KEY_LENGTH] = USBSTOR ;
					_tcscat(deviceKeyName, TEXT("\\") ) ;
					_tcscat(deviceKeyName, cSubKeyName) ;
					_tcscpy( usbList[i].cDeviceID, cSubKeyName) ;
					_tcsncpy( usbList[i].cKeyName, deviceKeyName, MAX_KEY_LENGTH - 1 ) ;
					// collect the creation time of this key - should be when they first plugged it in
					FileTimeToSystemTime( &ft, &stGMT) ;
					SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
					usbList[i].stStamp.wMonth = stLocal.wMonth ;
					usbList[i].stStamp.wDay = stLocal.wDay ;
					usbList[i].stStamp.wYear = stLocal.wYear ;
					usbList[i].stStamp.wHour = stLocal.wHour ;
					usbList[i].stStamp.wMinute = stLocal.wMinute ; 
					usbList[i].instance = NULL ;
				}
			}
		}
		RegCloseKey( hKey ) ;
	}

	// now we move on to getting the instance info from registry 
	for (int i = 0; i < iSubKeyCount; i++) {

		TCHAR	instanceIdKeyName[MAX_KEY_LENGTH] ;


		if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, usbList[i].cKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

			RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

			if (dwSubKeys) {

				// for now only update the first instance
				for (int n = 0; n < dwSubKeys; n++){
					// open the subkey
					// create the instance structure
					dwName = MAX_KEY_LENGTH ; // reset every cycle!
					current = (struct instanceInfo *) malloc(sizeof(struct instanceInfo)) ;
					if (RegEnumKeyEx(hKey, n, cSubKeyName, &dwName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {

						// fill out each instanceInfo
						_tcscpy( current->cInstanceID, cSubKeyName) ;
						_tcscpy(instanceIdKeyName, usbList[i].cKeyName) ;
						_tcscat(instanceIdKeyName, TEXT("\\") ) ;
						_tcscat(instanceIdKeyName, current->cInstanceID) ;
						// open each instanceID Registry Key to get the value data

						if ( RegOpenKeyEx( hKey, cSubKeyName, 0, KEY_READ, &hInstanceKey) == ERROR_SUCCESS) {

							RegQueryInfoKey(hInstanceKey, NULL, NULL, NULL, NULL, NULL, NULL, 
									&dwValues, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL ) ;
							ValName = malloc (dwMaxValueNameLen+1); // Allow for null. 
							Val = malloc (dwMaxValueLen); // Size in bytes. 

							if (dwValues) {
								for (int i=0; i<dwValues; i++){
									ValNameLen = dwMaxValueNameLen + 1;  
									ValLen = dwMaxValueLen + 1;
									RegEnumValue (hInstanceKey, i, ValName, &ValNameLen, 
											NULL, &ValType, Val, &ValLen);

									if (ValType == REG_SZ ||  ValType == REG_MULTI_SZ){// A null-terminated string. 
										if (_tcscmp(TEXT("ParentIdPrefix"), ValName) == 0) // treat hardware ID
											_tcscpy(current->cParentIdPrefix, (LPTSTR) Val) ;// as REG_SZ
										if (_tcscmp(TEXT("FriendlyName"), ValName) == 0) 
											_tcscpy(current->cFriendlyName, (LPTSTR) Val) ;
										if (_tcscmp(TEXT("HardwareID"), ValName) == 0) 
											_tcscpy(current->cHardwareID, (LPTSTR) Val) ;
										if (_tcscmp(TEXT("Driver"), ValName) == 0)
											_tcscpy(current->cDriver, (LPTSTR) Val) ;
									}
								}
							}	
							free(Val) ; free(ValName) ;
						} 
						RegCloseKey( hInstanceKey ) ;	
					}
					//End of instance cycle - assign current to instance.
					if (usbList[i].instance == NULL)
						usbList[i].instance = current ;
					else
						prev->next = current ;
					current->next = NULL ;
					prev = current ;

				} 
			}

		}
		RegCloseKey( hKey ) ;
	}

	/*

	   Get time stamps for each instance. as contained in the following DEVINTERFACE_DISK registry key:
	   SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53f56307-b6bf-11d0-94f2-00a0c91efb8b}
	   these time stamps should represent the last time the usb device is plugged in for each instance

	*/

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( DEVINTERFACE_DISK ), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

		for (int i = 0; i < iSubKeyCount; i++) {
			current = usbList[i].instance ;
			
			if (dwSubKeys) {
				for (int i = 0; i < dwSubKeys; i++){
					dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

					if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

						if ( _tcsstr( cSubKeyName, current->cInstanceID) != NULL ) {

							/* 
							   we have openned a key here that uses the ParentId Prefix. we should see if there is 
						  	   a Control subkey. and if there is we should try to get the times from that. As they
							   update more often than the main key.
							*/

							TCHAR cControlKeyName[MAX_PATH +1] ;
							_tcscpy(cControlKeyName, cSubKeyName) ;
							_tcscat(cControlKeyName, TEXT("\\Control") ) ;

							if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
									( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL, 
											NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {
								
								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stDiskStamp.wMonth = stLocal.wMonth ;
								current->stDiskStamp.wDay = stLocal.wDay ;
								current->stDiskStamp.wYear = stLocal.wYear ;
								current->stDiskStamp.wHour = stLocal.wHour ;
								current->stDiskStamp.wMinute = stLocal.wMinute ; 

								RegCloseKey( hControlKey ) ;
								
							} else {

								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stDiskStamp.wMonth = stLocal.wMonth ;
								current->stDiskStamp.wDay = stLocal.wDay ;
								current->stDiskStamp.wYear = stLocal.wYear ;
								current->stDiskStamp.wHour = stLocal.wHour ;
								current->stDiskStamp.wMinute = stLocal.wMinute ; 
							}
						}
					}
				}
			}

			while (current->next != NULL) {
				current = current->next ;

				if (dwSubKeys) {
					for (int i = 0; i < dwSubKeys; i++){
						dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

						if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

							if ( _tcsstr( cSubKeyName, current->cInstanceID) != NULL ) {
								/* 
								   we have openned a key here that uses the ParentId Prefix. we should see if there is 
								   a Control subkey. and if there is we should try to get the times from that. As they
								   update more often than the main key.
								*/

								TCHAR cControlKeyName[MAX_PATH +1] ;
								_tcscpy(cControlKeyName, cSubKeyName) ;
								_tcscat(cControlKeyName, TEXT("\\Control") ) ;

								if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
										( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL, 
														  NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stDiskStamp.wMonth = stLocal.wMonth ;
									current->stDiskStamp.wDay = stLocal.wDay ;
									current->stDiskStamp.wYear = stLocal.wYear ;
									current->stDiskStamp.wHour = stLocal.wHour ;
									current->stDiskStamp.wMinute = stLocal.wMinute ; 

									RegCloseKey( hControlKey ) ;

								} else {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
							current->stDiskStamp.wMonth = stLocal.wMonth ;
							current->stDiskStamp.wDay = stLocal.wDay ;
						    current->stDiskStamp.wYear = stLocal.wYear ;
							current->stDiskStamp.wHour = stLocal.wHour ;
							current->stDiskStamp.wMinute = stLocal.wMinute ; 
								}
							}
						}
					}
				}
			}
		}
		RegCloseKey( hKey ) ;
	}

	/*

	   Get time stamps for each instance. as contained in the following DEVINTERFACE_VOLUME registry key:
	   SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53F5630D-B6BF-11D0-94F2-00A0C91EFB8B}
	   these time stamps should represent the last time the usb device is plugged in for each instance

	*/

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( DEVINTERFACE_VOLUME ), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

		for (int i = 0; i < iSubKeyCount; i++) {
			current = usbList[i].instance ;
			
			if (dwSubKeys) {
				for (int i = 0; i < dwSubKeys; i++){
					dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

					if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

						if ( _tcsstr( cSubKeyName, current->cParentIdPrefix) != NULL ) {
							/* 
							   we have openned a key here that uses the ParentId Prefix. we should see if there is 
						  	   a Control subkey. and if there is we should try to get the times from that. As they
							   update more often than the main key.
							*/

							TCHAR cControlKeyName[MAX_PATH +1] ;
							_tcscpy(cControlKeyName, cSubKeyName) ;
							_tcscat(cControlKeyName, TEXT("\\Control") ) ;

							if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
									( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL, 
											NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {

								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stVolumeStamp.wMonth = stLocal.wMonth ;
								current->stVolumeStamp.wDay = stLocal.wDay ;
								current->stVolumeStamp.wYear = stLocal.wYear ;
								current->stVolumeStamp.wHour = stLocal.wHour ;
								current->stVolumeStamp.wMinute = stLocal.wMinute ;

								RegCloseKey( hControlKey ) ;
								
							} else {

							FileTimeToSystemTime( &ft, &stGMT) ;
							SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
							current->stVolumeStamp.wMonth = stLocal.wMonth ;
							current->stVolumeStamp.wDay = stLocal.wDay ;
						    current->stVolumeStamp.wYear = stLocal.wYear ;
							current->stVolumeStamp.wHour = stLocal.wHour ;
							current->stVolumeStamp.wMinute = stLocal.wMinute ;

							}
						}
					}
				}
			}

			while (current->next != NULL) {

				current = current->next ;

				if (dwSubKeys) {
					for (int i = 0; i < dwSubKeys; i++){

						dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

						if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {
							
							if ( _tcsstr( cSubKeyName, current->cParentIdPrefix) != NULL ) {
								/* 
								   we have openned a key here that uses the ParentId Prefix. we should see if there is 
								   a Control subkey. and if there is we should try to get the times from that. As they
								   update more often than the main key.
								*/

								TCHAR cControlKeyName[MAX_PATH +1] ;
								_tcscpy(cControlKeyName, cSubKeyName) ;
								_tcscat(cControlKeyName, TEXT("\\Control") ) ;

								if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
										( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL, 
														  NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stVolumeStamp.wMonth = stLocal.wMonth ;
									current->stVolumeStamp.wDay = stLocal.wDay ;
									current->stVolumeStamp.wYear = stLocal.wYear ;
									current->stVolumeStamp.wHour = stLocal.wHour ;
									current->stVolumeStamp.wMinute = stLocal.wMinute ;

									RegCloseKey( hControlKey ) ;

								} else {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stVolumeStamp.wMonth = stLocal.wMonth ;
									current->stVolumeStamp.wDay = stLocal.wDay ;
									current->stVolumeStamp.wYear = stLocal.wYear ;
									current->stVolumeStamp.wHour = stLocal.wHour ;
									current->stVolumeStamp.wMinute = stLocal.wMinute ;
								}
							}
						}
					}
				}
			}
		}
		RegCloseKey( hKey ) ;
	}

	/*
	   Open USBSTOR, get info and SubKeys for each device
    */
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\MountedDevices"), 
				0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, 
				&dwValues, &dwMaxValueNameLen, &dwMaxValueLen, NULL, &ft ) ;
		ValName = malloc (dwMaxValueNameLen+1); // Allow for null. 
		Val = malloc (dwMaxValueLen); // Size in bytes.

		FileTimeToSystemTime( &ft, &stGMT) ;
		SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);

		if (dwValues) {
			for (int i=0; i<dwValues; i++){

				ValNameLen = dwMaxValueNameLen ;  
				ValLen = dwMaxValueLen ;
				TCHAR valueString[ValLen+1] ;
				RegEnumValue (hKey, i, ValName, &ValNameLen, NULL, &ValType, Val, &ValLen);

				// eleminate older volume's and only check for recent DosDevices
				if (strncmp(  (LPSTR)ValName, "\\DosDev", 7) == 0) {	

					// build a usable string out of the REG_BINARY data in the registry by
					// skipping the white space in between each character.
					if (ValType == REG_BINARY)
						for (int i = 0; i < ValLen; i++, Val++) 
							valueString[i] = Val[i] ;

					valueString[ValLen+1] = '\0' ; 	//append a null by to the end of our string
													//as a null character isn't guaranteed

					// We only want to look at removable media and ignore fixed devices
					if (strncmp( valueString, "\\??\\STORAGE#RemovableMedia#", 27) == 0) {

						// Now cycle through each and see if hte ParentID is in valueString, if it is
						// then copy ValName to lastDriveLetter
						for (int i = 0; i < iSubKeyCount; i++) {

							current = usbList[i].instance ;

							if (  ( _tcsstr( valueString, current->cParentIdPrefix) != NULL)  
									&& (strlen(current->cParentIdPrefix) > 1 ) )
								_tcscpy(current->cLastDriveLetter, (LPTSTR) ValName) ;
							while (current->next != NULL) {
								current = current->next ;
								if ( ( _tcsstr( valueString, current->cParentIdPrefix) != NULL )  
										&& (strlen(current->cParentIdPrefix) > 1 ) )
									_tcscpy(current->cLastDriveLetter, (LPTSTR)ValName) ;
							}
						}
						// Done Cycling Through the list.
					}
				}
			}
		}
		
	free(Val) ; free(ValName) ;
	}
	
	RegCloseKey( hKey ) ;	
}


void cleanDeviceList( struct usbInfo * usbList) 
{

	int iSubKeyCount = countSubKeys( USBSTOR ) ;
	struct instanceInfo * next, * current ;

	for (int i = 0; i < iSubKeyCount; i++) {
		current = usbList[i].instance ;
		while (current->next != NULL) {
			next = current->next ;
			free(current) ;
			current = next ;
		}
	}

}
