/*! 
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright(c) 2009 Apogee Instruments, Inc. 
* \class FindDeviceUsb 
* \brief searches through a number of usb devices looking for apogee devices 
* 
*/ 

#include "FindDeviceUsb.h" 

#include <sstream>

#include "AltaIo.h" 
#include "AscentBasedIo.h" 
#include "AspenIo.h" 
#include "apgHelper.h" 
#include "helpers.h"
#include "CameraInfo.h" 
#include "CamHelpers.h"  // for vids and pids
#include "linux/linuxHelpers.h"


//////////////////////////// 
// DTOR 
FindDeviceUsb::~FindDeviceUsb() 
{ 

} 

//////////////////////////// 
// FIND 
std::string FindDeviceUsb::Find()
{
    std::string result;
    try
    {
        std::vector< std::vector<uint16_t> > devs = GetApgDevices();

        std::vector< std::vector<uint16_t> >::iterator iter;
        
        for(iter = devs.begin(); iter != devs.end(); ++iter)
	    {      
            const uint16_t vid = (*iter).at(1);
            const uint16_t pid = (*iter).at(2);

            // deal with blank apogee devices
            if (UsbFrmwr::CYPRESS_VID == vid)
            {
                  std::string content = "<d>address="+help::uShort2Str((*iter).at(0)) +
                    ",interface=usb,deviceType=camera,id=0xFFFF,firmwareRev=0xFFEE,model=AltaU-Blank,interfaceStatus=NA</d>";

                 result.append(content);
            }

             // deal with apogee devices
             if (UsbFrmwr::APOGEE_VID == vid)
            {
                    // ticket 53, repeat of something chwy found
                    // i think i only need to do this check on windoze
                    // check if the usb device is already opened by another
                    // process
                    if (IsDeviceAlreadyOpen((*iter).at(0) ))
                    {
                        continue;
                    }

                    if (UsbFrmwr::FILTER_WHEEL_PID == pid)
                    {
                        //right now no id's or firmware verions for the filterwheel
                        std::string content = "<d>address="+help::uShort2Str((*iter).at(0))+
                                ",interface=usb,model=Filter Wheel,deviceType=filterWheel,id=0xFFFF,firmwareRev=0xFFEE</d>";
                            result.append(content);
                    }

                    if (UsbFrmwr::ALTA_USB_PID == pid)
                    {
                            std::string content = "<d>address="+help::uShort2Str((*iter).at(0))+
                                ",interface=usb,deviceType=camera," +
                                AltaInfo(help::uShort2Str((*iter).at(0) )) +"</d>";
                            result.append(content);
                    }

                    if (UsbFrmwr::ASCENT_USB_PID == pid)
                    {
                            std::string content = "<d>address="+help::uShort2Str((*iter).at(0))+
                                ",interface=usb,deviceType=camera," + 
                                AscentInfo(help::uShort2Str((*iter).at(0) )) +"</d>";
                            result.append(content);
                    }

                    if (UsbFrmwr::ASPEN_USB_PID == pid)
                    {
                        std::string content = "<d>address="+help::uShort2Str((*iter).at(0))+
                            ",interface=usb,deviceType=camera," + 
                            AspenInfo(help::uShort2Str((*iter).at(0) )) + "</d>";
                        result.append(content);
                    }
                }
            }
    }
    catch(std::exception & err)
    {
        // catch, log, and rethrow errors from parsecfgfile
        apgHelper::LogErrorMsg(__FILE__, err.what(), __LINE__);

        throw;
    }
    
    if (result.empty())
    {
    	//no devices, return the no op string
    	result.append("<d></d>");
    }

    return result;
}


//////////////////////////// 
//  ALTA   INFO 
std::string FindDeviceUsb::AltaInfo(const std::string & deviceAddr)
{
    AltaIo usbIo(CamModel::USB, deviceAddr);  
    return(MkCamInfoStr(usbIo.GetId(), usbIo.GetFirmwareRev() ));

}

//////////////////////////// 
//  ASCENT    INFO 
std::string FindDeviceUsb::AscentInfo(const std::string & deviceAddr)
{
    AscentBasedIo usbIo(CamModel::USB, deviceAddr);  
    return(MkCamInfoStr(usbIo.GetId(), usbIo.GetFirmwareRev() ));
}

   
//////////////////////////// 
//  ASPEN   INFO 
std::string FindDeviceUsb::AspenInfo(const std::string & deviceAddr)
{
    AspenIo usbIo(CamModel::USB, deviceAddr);  
    return(MkCamInfoStr(usbIo.GetId(), usbIo.GetFirmwareRev() ));
}

//////////////////////////// 
//          MK       CAM       INFO       STR
std::string FindDeviceUsb::MkCamInfoStr(const uint16_t Id, const uint16_t FrmwrRev)
{
    std::stringstream infoStr;
    infoStr << std::hex << std::showbase;
    infoStr << "id=" << Id;
    infoStr << ",firmwareRev=" << FrmwrRev;
    infoStr << ",model=" << CamModel::GetPlatformStr(Id, false).c_str();
    infoStr << "-" <<  CamModel::GetModelStr(Id);
    infoStr << ",interfaceStatus=NA";

    return infoStr.str();
}

//////////////////////////// 
// GET		APG		DEVICES
std::vector< std::vector<uint16_t> > FindDeviceUsb::GetApgDevices()
{
	return linuxHelpers::GetDevicesLinux();
}

bool FindDeviceUsb::IsDeviceAlreadyOpen(const uint16_t deviceNum)
{
    return false;
}
