/* mbed Microcontroller Library
 * Copyright (c) 2017 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CLOUD_CLIENT_DM_
#define _CLOUD_CLIENT_DM_

#include "mbed.h"
#include "MbedCloudClient.h"

/* This class provides a friendly API to the Mbed Cloud Client and its
 * Device Management functionality.
 *
 * Please refer to http://www.openmobilealliance.org/wp/OMNA/LwM2M/LwM2MRegistry.html
 * and particularly objects urn:oma:lwm2m:oma:3 and urn:oma:lwm2m:oma:5
 * for a description of the objects and resources involved.
 *
 * The pattern of usage is as follows:
 *
 * 1.  Instantiate CloudClientDm, with debug either on or off.
 * 2.  Configure CloudClientDm by calling the "set" or "add" methods
 *     for the Device object resources you would like to include.  Note
 *     that some resources may be added by Mbed Cloud Client itself but
 *     there is no harm in you adding them, they will not be
 *     duplicated and your value will overwrite the previous value.
 * 3.  Start CloudClientDm; this creates the Device object and its
 *     resources according to your configuration.
 * 4.  Register CloudClientDm with a LWM2M server and use it.
 * 5.  When finished, stop CloudClientDm.
 * 6.  Delete CloudClientDm.
 *
 *  Note: the following DM object resources:
 *
 *        Manufacturer,
 *        Model Number,
 *        Supported Binding Mode,
 *        Time,
 *
 * ...are managed entirely by the mbed cloud client and cannot be
 * modified through this interface.
 */
class CloudClientDm {
public:

    /** The possible battery status values (according to
     * the OMA LWM2M Device object standard)
     */
    typedef enum {
        BATTERY_STATUS_NORMAL = 0,
        BATTERY_STATUS_CHARGING = 1,
        BATTERY_STATUS_CHARGING_COMPLETE = 2,
        BATTERY_STATUS_FAULT = 3,
        BATTERY_STATUS_LOW_BATTERY = 4,
        BATTERY_STATUS_NOT_PRESENT = 5,
        BATTERY_STATUS_UNKNOWN = 6,
        MAX_NUM_BATTERY_STATUS
    } BatteryStatus;

    /** The possible power sources (according to
     * the OMA LWM2M Device object standard)
     */
    typedef enum {
        POWER_SOURCE_UNUSED = 0xFF, // This one not from
                                    // the standard
        POWER_SOURCE_DC = 0,
        POWER_SOURCE_INTERNAL_BATTERY = 1,
        POWER_SOURCE_EXTERNAL_BATTERY = 2,
        POWER_SOURCE_POE = 4,
        POWER_SOURCE_USB = 5,
        POWER_SOURCE_MAINS = 6,
        POWER_SOURCE_SOLAR = 7,
        MAX_NUM_POWER_SOURCES
    } PowerSource;

    /** The possible errors (according to the
     * OMA LWM2M Device object standard)
     */
    typedef enum {
        ERROR_NONE = 0,
        ERROR_LOW_BATTERY = 1,
        ERROR_NO_EXTERNAL_POWER = 2,
        ERROR_GPS_FAILURE = 3,
        ERROR_LOW_RECEIVED_SIGNAL = 4,
        ERROR_OUT_OF_MEMORY = 5,
        ERROR_SMS_FAILURE = 6,
        ERROR_IP_CONNECTIVITY_FAILURE = 7,
        ERROR_PERIPHERAL_FAILURE = 8,
        MAX_NUM_ERRORS
    } Error;

    /** Constructor.
     *
     * @param debugOn                  true if you want debug prints, otherwise false.
     * @param registeredUserCallback   function to be called when registration with a
     *                                 LWM2M server has completed.
     * @param deregisteredUserCallback function to be called when the client has
     *                                 deregisteted from the LWM2M server.
     * @param errorUserCallback        function to be called when an error has occurred.
     */
    CloudClientDm(bool debugOn,
                  Callback<void()> registeredUserCallback = NULL,
                  Callback<void()> deregisteredUserCallback = NULL,
                  Callback<void(int)> errorUserCallback = NULL);

    /** Destructor.
     */
    ~CloudClientDm();

    /** Start the mbed cloud client with Device object
     * plus any additional objects.
     *
     * @param objectList list of additional objects to add (may
     *                   be NULL).
     * @return           true if successful, otherwise false.
     */
    bool start(M2MObjectList *objectList);

    /** Stop the mbed cloud client and its objects, deregistering from the
     * server if required.
     */
    void stop();

    /** Connect the mbed cloud client with the server.
     *
     * @param interface  a pointer to the NetworkInterface to use.
     * @return           true if successful, otherwise false.
     */
    bool connect(void *interface);

    /** Keep a UDP link cooking.
     */
    void keepAlive();

    /** Set the value of the Device object Device Type resource.
     * The value of this static resource is stored in Cloud Client
     * storage.
     *
     * @param deviceType the device type as a string.
     * @return           true if successful, otherwise false.
     */
    bool setDeviceObjectStaticDeviceType(const char *deviceType);

    /** Set the value of the Device object Serial Number resource.
     * The value of this static resource is stored in Cloud Client
     * storage.
     *
     * @param serialNumber the serial number as a string.
     * @return             true if successful, otherwise false.
     */
    bool setDeviceObjectStaticSerialNumber(const char *serialNumber);

    /** Set the value of the Device object Hardware Version resource.
     * The value of this static resource is stored in Cloud Client
     * storage.
     *
     * @param hardwareVersion the hardware version as a string.
     * @return                true if successful, otherwise false.
     */
    bool setDeviceObjectStaticHardwareVersion(const char *hardwareVersion);

    /** Set the value of the Device object Software Version resource.
     * The value of this static resource is stored in Cloud Client
     * storage.
     *
     * @param softwareVersion the software version as a string.
     * @return                true if successful, otherwise false.
     */
    bool setDeviceObjectStaticSoftwareVersion(const char *softwareVersion);

    /** Set the value of the Device object Firmware Version resource.
     * You'da though that the value of this resource would be stored
     * statically in Cloud Client storage but for some reason the
     * mbed Cloud Client doesn't do so.
     *
     * @param firmwareVersion the firmware version as a string.
     * @return                true if successful, otherwise false.
     */
    bool setDeviceObjectFirmwareVersion(const char *firmwareVersion);

    /** Add an Available Power Source to the Device object resource.
     *
     * @param powerSource the power source.
     * @return            true if successful, otherwise false.
     */
    bool addDeviceObjectPowerSource(PowerSource powerSource);

    /** Delete an Available Power Source from the Device object resource.
     *
     * @param powerSource the power source.
     * @return            true if successful, otherwise false.
     */
    bool deleteDeviceObjectPowerSource(PowerSource powerSource);

    /** Set the value of the Device object Power Source Voltage
     * resource for a given power source.
     *
     * @param powerSource   the power source to which this applies.
     * @param voltageMV     the Voltage.
     * @return              true if successful, otherwise false.
     */
    bool setDeviceObjectVoltage(PowerSource powerSource, int voltageMV);

    /** Set the value of the Device object Power Source Current
     * resource for a given power source.
     *
     * @param powerSource   the power source to which this applies.
     * @param currentMA     the current.
     * @return              true if successful, otherwise false.
     */
    bool setDeviceObjectCurrent(PowerSource powerSource, int currentMA);

    /** Set the value of the Device object Battery Level resource. For
     * this to work a power source of type POWER_SOURCE_INTERNAL_BATTERY
     * must exist.
     *
     * @param batteryLevelPercent the battery level.
     * @return                    true if successful, otherwise false.
     */
    bool setDeviceObjectBatteryLevel(int batteryLevelPercent);

    /** Set the value of the Device object Battery Status resource. For
     * this to work a power source of type POWER_SOURCE_INTERNAL_BATTERY
     * must exist.
     *
     * @param batteryStatus the battery status.
     * @return              true if successful, otherwise false.
     */
    bool setDeviceObjectBatteryStatus(BatteryStatus batteryStatus);

    /** Set the value of the Device object Memory Total resource.
     * Note: the Cloud Client appears to read this value
     * from the Cloud Client storage, however attempts to write
     * there fail hence this simply overwrites the value that
     * is read from storage.
     *
     * @param memoryTotalKBytes the total memory.
     * @return                  true if successful, otherwise false.
     */
    bool setDeviceObjectMemoryTotal(int64_t memoryTotalKBytes);

    /** Set the value of the Device object Memory Free resource.
     *
     * @param memoryFreeKBytes the free memory.
     * @return                 true if successful, otherwise false.
     */
    bool setDeviceObjectMemoryFree(int64_t memoryFreeKBytes);

    /** Set the value of the Device object Error Code resource.
     * Note: only a single error code is supported.
     *
     * @param errorCode the error code.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectErrorCode(Error errorCode);

    /** Set the value of the Device object UTC Offset resource.
     * Note: the Cloud Client appears to read this value
     * from the Cloud Client storage, however attempts to write
     * there seem to invalidate the configuration, hence this
     * simply overwrites the value that is read from storage.
     *
     * @param utcOffsetISO8601 the UTC offset, in the form "UTC+X"
     *                         [ISO 8601].
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectUtcOffset(const char *utcOffsetISO8601);

    /** Set the value of the Device object Timezone resource.
     * Note: the Cloud Client appears to read this value
     * from the Cloud Client storage, however attempts to write
     * there seem to invalidate the configuration, hence this
     * simply overwrites the value that is read from storage.
     *
     * @param timezoneIANA the timezone, in IANA Timezone (TZ)
     *                     database format.
     * @return             true if successful, otherwise false.
     */
    bool setDeviceObjectTimezone(const char *timezoneIANA);

protected:

    /** Callback for registration event.
     */
    void clientRegisteredCallback();

    /** Callback for deregistration event.
     */
    void clientDeregisteredCallback();

    /** Callback for error event.
     *
     * @param errorCode the Mbed Client error code.
     */
    void errorCallback(int errorCode);

    /** Create the given string resource on the Device object.
     * The resources on which this can be used are:
     *
     * M2MDevice::Manufacturer
     * M2MDevice::DeviceType
     * M2MDevice::ModelNumber
     * M2MDevice::SerialNumber
     * M2MDevice::HardwareVersion
     * M2MDevice::FirmwareVersion
     * M2MDevice::SoftwareVersion
     * M2MDevice::UTCOffset
     * M2MDevice::Timezone
     * M2MDevice::SupportedBindingMode
     *
     * @param resource  the type of the resource.
     * @param value     the string value to set.
     * @return          true if successful, otherwise false.
     */
    bool createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                    const char *value);

    /** Create the given single-instance integer resource on
     * the Device object.  The resources on which this can be
     * used are:
     *
     * M2MDevice::BatteryLevel
     * M2MDevice::BatteryStatus
     * M2MDevice::MemoryFree
     * M2MDevice::MemoryTotal
     * M2MDevice::CurrentTime
     *
     * @param resource  the type of the resource.
     * @param value     the integer to set.
     * @return          true if successful, otherwise false.
     */
    bool createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                    int64_t value);

    /** Create the given multi-instance integer resource on
     * the Device object.  The resources on which this can be
     * used are:
     *
     * M2MDevice::AvailablePowerSources
     * M2MDevice::PowerSourceVoltage
     * M2MDevice::PowerSourceCurrent
     * M2MDevice::ErrorCode
     *
     * @param resource  the type of the resource.
     * @param value     the integer to set.
     * @param instance  the instance ID.
     * @return          true if successful, otherwise false.
     */
    bool createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                    int64_t value, uint16_t instance);

    /** Create the given executable resource on the Device object.
     * The resources on which this can be used are:
     *
     * M2MDevice::ResetErrorCode
     * M2MDevice::FactoryReset
     *
     * @param resource  the type of the resource.
     * @return          true if successful, otherwise false.
     */
    bool createDeviceObjectResource(M2MDevice::DeviceResource resource);

    /** Delete the given single-instance resource on the Device
     * object.
     *
     * @param resource  the type of the resource.
     * @return          true if successful, otherwise false.
     */
    bool deleteDeviceObjectResource(M2MDevice::DeviceResource resource);

    /** Delete the given multi-instance resource on
     * the Device object.
     *
     * @param resource  the type of the resource.
     * @param instance  the instance ID.
     * @return          true if successful, otherwise false.
     */
    bool deleteDeviceObjectResource(M2MDevice::DeviceResource resource,
                                    uint16_t instance);

    /** Set a Device object resource.
     *
     * @param resource  the type of the resource.
     * @param str       the value to set.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                 std::string str);

    /** Set a Device object resource.
     *
     * @param resource  the type of the resource.
     * @param value     the value to set.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                 const char *value);

    /** Set a Device object resource.
     *
     * @param resource  the type of the resource.
     * @param value     the value to set.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                 int64_t value);

    /** Set the given multi-instance integer resource on
     * the Device object to value.
     *
     * @param resource  the type of the resource.
     * @param value     the integer to set.
     * @param instance  the instance ID.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                 int64_t value, uint16_t instance);

    /** Set a Device object resource in the config store.
     *
     * @param resource  the type of the resource.
     * @param str       the value to set.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectConfigResource(M2MDevice::DeviceResource resource,
                                       std::string str);

    /** Set a Device object resource in the config store.
     *
     * @param resource  the type of the resource.
     * @param value     the value to set.
     * @return          true if successful, otherwise false.
     */
    bool setDeviceObjectConfigResource(M2MDevice::DeviceResource resource,
                                       const char *value);

    /** Get the error string for an MbedClient error code
     *
     * @param errorCode  the Mbed Client error code.
     * @return           a pointer to an error string.
     */
    const char *getMbedClientErrorString(MbedCloudClient::Error errorCode);

    /** Get the error string for a Cloud Client Storage error code
     *
     * @param errorCode  the Cloud Client Storage error code.
     * @return           a pointer to an error string.
     */
    const char *getCCSErrorString(ccs_status_e errorCode);

    /** Callback to be called when registration has occurred.
     */
    Callback<void()> _registeredUserCallback;

    /** Callback to be called when deregistration has occurred.
     */
    Callback<void()> _deregisteredUserCallback;

    /** Callback to be called when an error has occurred.
     */
    Callback<void(int)> _errorUserCallback;

    /** Set to true to have debug prints.
     */
    bool               _debugOn;

    /** Will be set to true when start() is called,
     * never set back to false again.
     */
    bool               _started;

    /** An instance of the mbed Cloud Client.
     */
    MbedCloudClient    _cloudClient;

    /** Array to track instances of Available Power Source.
     */
    uint16_t           _powerSourceInstance[MAX_NUM_POWER_SOURCES];
};

#endif // _CLOUD_CLIENT_DM_

// End of file
