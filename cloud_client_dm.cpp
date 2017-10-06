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

#include "mbed.h"
#include "MbedCloudClient.h"
#include "CloudClientStorage.h"
#include "cloud_client_dm.h"

#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
#include "update_ui_example.h"
#endif

#define printfLog(format, ...) debug_if(_debugOn, format, ## __VA_ARGS__)

/**********************************************************************
 * STATIC VARIABLES
 **********************************************************************/

// Strings to match the Device object resources enum.
// Note: MUST have the same number of elements as M2MDevice::DeviceResource
static const char * deviceObjectResourceString[] = {"mbed.Manufacturer",
                                                    "mbed.DeviceType",
                                                    "mbed.ModelNumber",
                                                    "mbed.SerialNumber",
                                                    "mbed.HardwareVersion",
                                                    "mbed.FirmwareVersion",
                                                    "mbed.SoftwareVersion",
                                                    "mbed.Reboot",
                                                    "mbed.FactoryReset",
                                                    "mbed.AvailablePowerSources",
                                                    "mbed.PowerSourceVoltage",
                                                    "mbed.PowerSourceCurrent",
                                                    "mbed.BatteryLevel",
                                                    "mbed.BatteryStatus",
                                                    "mbed.MemoryFree",
                                                    "mbed.MemoryTotalKB",
                                                    "mbed.ErrorCode",
                                                    "mbed.ResetErrorCode",
                                                    "mbed.CurrentTime",
                                                    "mbed.UTCOffset",
                                                    "mbed.Timezone",
                                                    "mbed.SupportedBindingMode"};

/**********************************************************************
 * PROTECTED METHODS: CALLBACKS
 **********************************************************************/

// Callback for registration event
void CloudClientDm::clientRegisteredCallback()
{
    static const ConnectorClientEndpointInfo* endpoint;

    _registered = true;
    printfLog("Client registered.\n");

    endpoint = _cloudClient.endpoint_info();
    if (endpoint != NULL) {
#ifdef MBED_CONF_APP_DEVELOPER_MODE
        printfLog("Endpoint Name: %s.\n", endpoint->internal_endpoint_name.c_str());
#else
        printfLog("Endpoint Name: %s.\n", endpoint->endpoint_name.c_str());
#endif
        printfLog("Device ID: %s.\n", endpoint->internal_endpoint_name.c_str());
    }

    if (_registeredUserCallback) {
        _registeredUserCallback();
    }
}

// Callback for deregistration event
void CloudClientDm::clientDeregisteredCallback()
{
    _registered = false;
    printfLog("Client deregistered.\n");

    if (_deregisteredUserCallback) {
        _deregisteredUserCallback();
    }
}

// Callback for error event
void CloudClientDm::errorCallback(int errorCode)
{
    printfLog("Error occurred: %s.\n", getMbedClientErrorString((MbedCloudClient::Error) errorCode));
    printfLog("Error code: %d.\n", errorCode);
    printfLog("Error details: %s.\n",_cloudClient.error_description());

    if (_errorUserCallback) {
        _errorUserCallback(errorCode);
    }
}

/**********************************************************************
 * PROTECTED METHODS: GENERAL
 **********************************************************************/

// Create the given resource on the Device object, for string types
bool CloudClientDm::createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                               const char *value)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) &&
        (deviceObject->create_resource(resource, String(value, strlen(value))) != NULL)) {
        success = true;
    } else {
        printfLog("Error creating string resource \"%s\" on the Device object.\n",
                  deviceObjectResourceString[resource]);
    }

    return success;
}

// Create the given resource on the Device object, for integer types that can
// only have a single instance
bool CloudClientDm::createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                               int64_t value)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) && (deviceObject->create_resource(resource, value) != NULL)) {
        success = true;
    } else {
        printfLog("Error creating single-instance integer resource \"%s\" on the Device object.\n",
                  deviceObjectResourceString[resource]);
    }

    return success;
}

// Create the given resource on the Device object, for integer types where
// multiple instances are possible
bool CloudClientDm::createDeviceObjectResource(M2MDevice::DeviceResource resource,
                                               int64_t value, uint16_t instance)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) &&
        (deviceObject->create_resource_instance(resource, value, instance) != NULL)) {
        success = true;
    } else {
        printfLog("Error creating integer multi-instance resource \"%s\", instance %d, on the Device object.\n",
                  deviceObjectResourceString[resource], instance);
    }

    return success;
}

// Create the given resource on the Device object, for executable types
bool CloudClientDm::createDeviceObjectResource(M2MDevice::DeviceResource resource)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) && (deviceObject->create_resource(resource) != NULL)) {
        success = true;
    } else {
        printfLog("Error creating control resource \"%s\" on the Device object.\n",
                  deviceObjectResourceString[resource]);
    }

    return success;
}

// Delete the given resource on the Device object (for single instance resources).
bool CloudClientDm::deleteDeviceObjectResource(M2MDevice::DeviceResource resource)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) && deviceObject->delete_resource(resource)) {
        success = true;
    } else {
        printfLog("Error deleting single-instance resource \"%s\", on the Device object.\n",
                  deviceObjectResourceString[resource]);
    }

    return success;
}

// Delete the given instance of the given resource on the Device object, for types
// where multiple instances are possible

bool CloudClientDm::deleteDeviceObjectResource(M2MDevice::DeviceResource resource,
                                               uint16_t instance)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if ((deviceObject != NULL) && deviceObject->delete_resource_instance(resource, instance)) {
        success = true;
    } else {
        printfLog("Error deleting multi-instance resource \"%s\", instance %d, on the Device object.\n",
                  deviceObjectResourceString[resource], instance);
    }

    return success;
}

// Set a Device object resource.
bool CloudClientDm::setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                            std::string str)
{
    bool success;

    // If we've not started, make sure the resource has been created
    if (!_started) {
        M2MDevice *deviceObject = M2MInterfaceFactory::create_device();
        if ((deviceObject != NULL) && !deviceObject->is_resource_present(resource)) {
            createDeviceObjectResource(resource, str.c_str());
        }
    }

    // Now set the value
    success = _cloudClient.set_device_resource_value(resource, str);

    if (!success) {
        printfLog("Error setting %s.\n", deviceObjectResourceString[resource]);
    }

    return success;
}

// Set a Device object resource.
bool CloudClientDm::setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                            const char *value)
{
    std::string str(value);
    return setDeviceObjectResource(resource, str);
}

// Set a Device object resource.
bool CloudClientDm::setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                            int64_t value)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if (deviceObject != NULL) {
        // If we've not started, make sure the resource instance has been created
        if ((deviceObject != NULL) && !deviceObject->is_resource_present(resource)) {
            createDeviceObjectResource(resource, value);
        }

        // Now set the value
        success = deviceObject->set_resource_value(resource, value);
    }

    if (!success) {
        printfLog("Error setting integer resource \"%s\" on the Device object.\n",
                  deviceObjectResourceString[resource]);
    }

    return success;
}

// Set a Device object multi-instance integer resource.
bool CloudClientDm::setDeviceObjectResource(M2MDevice::DeviceResource resource,
                                            int64_t value, uint16_t instance)
{
    bool success = false;
    M2MDevice *deviceObject = M2MInterfaceFactory::create_device();

    if (deviceObject != NULL) {
        // If we've not started, make sure the resource instance has been created
        // Note: returning -1 seems like a valid thing to do if the resource's
        // value happens to be -1.  Thankfully that should not be the case for any
        // of the resources we are concerned with here.
        if (!_started) {
            if (deviceObject->resource_value_int(resource, instance) == -1) {
                createDeviceObjectResource(resource, value, instance);
            }
        }

        // Now set the value
        success = deviceObject->set_resource_value(resource, value, instance);
    }

    if (!success) {
        printfLog("Error setting integer multi-instance resource \"%s\", instance %d, on the Device object.\n",
                  deviceObjectResourceString[resource], instance);
    }

    return success;
}

// Set a Device object resource in the config store.
bool CloudClientDm::setDeviceObjectConfigResource(M2MDevice::DeviceResource resource,
                                                  std::string str)
{
    return setDeviceObjectConfigResource(resource, str.c_str());
}

// Set a Device object resource in the config store.
bool CloudClientDm::setDeviceObjectConfigResource(M2MDevice::DeviceResource resource,
                                                  const char *value)
{
    ccs_status_e ccsStatus;

    delete_config_parameter(deviceObjectResourceString[resource]);
    ccsStatus = set_config_parameter(deviceObjectResourceString[resource],
                                     (const uint8_t *) value, strlen(value));

    if (ccsStatus != CCS_STATUS_SUCCESS) {
        printfLog("Error setting %s (%s).\n",
                  deviceObjectResourceString[resource],
                  getCCSErrorString(ccsStatus));
    }

    return (ccsStatus == CCS_STATUS_SUCCESS);
}

// Get the error string for an Mbed Client error code.
const char *CloudClientDm::getMbedClientErrorString(MbedCloudClient::Error errorCode)
{
    const char *errorString = NULL;

    switch (errorCode) {
        case MbedCloudClient::ConnectErrorNone:
            errorString = "MbedCloudClient::ConnectErrorNone";
            break;
        case MbedCloudClient::ConnectAlreadyExists:
            errorString = "MbedCloudClient::ConnectAlreadyExists";
            break;
        case MbedCloudClient::ConnectBootstrapFailed:
            errorString = "MbedCloudClient::ConnectBootstrapFailed";
            break;
        case MbedCloudClient::ConnectInvalidParameters:
            errorString = "MbedCloudClient::ConnectInvalidParameters";
            break;
        case MbedCloudClient::ConnectNotRegistered:
            errorString = "MbedCloudClient::ConnectNotRegistered";
            break;
        case MbedCloudClient::ConnectTimeout:
            errorString = "MbedCloudClient::ConnectTimeout";
            break;
        case MbedCloudClient::ConnectNetworkError:
            errorString = "MbedCloudClient::ConnectNetworkError";
            break;
        case MbedCloudClient::ConnectResponseParseFailed:
            errorString = "MbedCloudClient::ConnectResponseParseFailed";
            break;
        case MbedCloudClient::ConnectUnknownError:
            errorString = "MbedCloudClient::ConnectUnknownError";
            break;
        case MbedCloudClient::ConnectMemoryConnectFail:
            errorString = "MbedCloudClient::ConnectMemoryConnectFail";
            break;
        case MbedCloudClient::ConnectNotAllowed:
            errorString = "MbedCloudClient::ConnectNotAllowed";
            break;
        case MbedCloudClient::ConnectSecureConnectionFailed:
            errorString = "MbedCloudClient::ConnectSecureConnectionFailed";
            break;
        case MbedCloudClient::ConnectDnsResolvingFailed:
            errorString = "MbedCloudClient::ConnectDnsResolvingFailed";
            break;
#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
        case MbedCloudClient::UpdateWarningCertificateNotFound:
            errorString = "MbedCloudClient::UpdateWarningCertificateNotFound";
            break;
        case MbedCloudClient::UpdateWarningIdentityNotFound:
            errorString = "MbedCloudClient::UpdateWarningIdentityNotFound";
            break;
        case MbedCloudClient::UpdateWarningCertificateInvalid:
            errorString = "MbedCloudClient::UpdateWarningCertificateInvalid";
            break;
        case MbedCloudClient::UpdateWarningSignatureInvalid:
            errorString = "MbedCloudClient::UpdateWarningSignatureInvalid";
            break;
        case MbedCloudClient::UpdateWarningVendorMismatch:
            errorString = "MbedCloudClient::UpdateWarningVendorMismatch";
            break;
        case MbedCloudClient::UpdateWarningClassMismatch:
            errorString = "MbedCloudClient::UpdateWarningClassMismatch";
            break;
        case MbedCloudClient::UpdateWarningDeviceMismatch:
            errorString = "MbedCloudClient::UpdateWarningDeviceMismatch";
            break;
        case MbedCloudClient::UpdateWarningURINotFound:
            errorString = "MbedCloudClient::UpdateWarningURINotFound";
            break;
        case MbedCloudClient::UpdateWarningRollbackProtection:
            errorString = "MbedCloudClient::UpdateWarningRollbackProtection";
            break;
        case MbedCloudClient::UpdateWarningUnknown:
            errorString = "MbedCloudClient::UpdateWarningUnknown";
            break;
        case MbedCloudClient::UpdateErrorWriteToStorage:
            errorString = "MbedCloudClient::UpdateErrorWriteToStorage";
            break;
#endif
        default:
            errorString = "UNKNOWN";
            break;
    }

    return errorString;
}

// Get the error string for a Cloud Client Storage error code.
const char *CloudClientDm::getCCSErrorString(ccs_status_e errorCode)
{
    const char *errorString = NULL;

    switch (errorCode) {
        case CCS_STATUS_SUCCESS:
            errorString = "CloudClientStorage::Success";
            break;
        case CCS_STATUS_ERROR:
            errorString = "CloudClientStorage::Error";
            break;
        case CCS_STATUS_KEY_DOESNT_EXIST:
            errorString = "CloudClientStorage::KeyDoesntExist";
            break;
        case CCS_STATUS_VALIDATION_FAIL:
            errorString = "CloudClientStorage::ValidationFail";
            break;
        case CCS_STATUS_MEMORY_ERROR:
            errorString = "CloudClientStorage::MemoryError";
            break;
        default:
            errorString = "UNKNOWN";
            break;
    }

    return errorString;
}

/**********************************************************************
 * PUBLIC METHODS: GENERAL
 **********************************************************************/

// Constructor.
CloudClientDm::CloudClientDm(bool debugOn,
                             Callback<void()> registeredUserCallback,
                             Callback<void()> deregisteredUserCallback,
                             Callback<void(int)> errorUserCallback)
{
    _debugOn = debugOn;
    _started = false;
    _registered = false;
    _registeredUserCallback = registeredUserCallback;
    _deregisteredUserCallback = deregisteredUserCallback;
    _errorUserCallback = errorUserCallback;

    for (unsigned int x = 0; x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0]); x++) {
        _powerSourceInstance[x] = POWER_SOURCE_UNUSED;
    }
}

// Destructor.
CloudClientDm::~CloudClientDm()
{
}

// Add an M2M object that you have created to the client.
void CloudClientDm::addObject(M2MObject *object)
{
    printfLog("Adding object: \"%s\" to mbed cloud client's list...\n", object->name());
    _objectList.push_back(object);
}

// Initialise LWM2M and its objects.
bool CloudClientDm::start( MbedCloudClientCallback *updateCallback)
{
    _started = true;
    _cloudClient.add_objects(_objectList);
    _cloudClient.on_registered(this, &CloudClientDm::clientRegisteredCallback);
    _cloudClient.on_unregistered(this, &CloudClientDm::clientDeregisteredCallback);
    _cloudClient.on_error(this, &CloudClientDm::errorCallback);
    if (updateCallback != NULL) {
        _cloudClient.set_update_callback(updateCallback);
    }

    return true;
}

// Close the cloud client and release its objects,
// deregistering from the server.
void CloudClientDm::stop()
{
    Timer timer;

    // This is an asynchronous operation,
    // the connection is not closed until
    // clientDeregisteredCallback() is called
    timer.start();
    _cloudClient.close();
    while (_registered && (timer.read_ms() < CLOUD_CLIENT_STOP_TIMEOUT_SECONDS * 1000))  {
        wait_ms(100);
    }

    // Delete the available power source and associated Device object
    // resources that have been created
    for (uint16_t x = 0; (x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0])); x++) {
        if (_powerSourceInstance[x] != POWER_SOURCE_UNUSED) {
            deleteDeviceObjectResource(M2MDevice::AvailablePowerSources, x);
            deleteDeviceObjectResource(M2MDevice::PowerSourceVoltage, x);
            deleteDeviceObjectResource(M2MDevice::PowerSourceCurrent, x);
            _powerSourceInstance[x] = POWER_SOURCE_UNUSED;
        }
    }

    // Everything else will be freed when we are deleted
}

// Connect the mbed cloud client with the server.
bool CloudClientDm::connect(void *interface)
{
    bool success = false;

    if (_started) {
        success = _cloudClient.setup(interface);

#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
        /* Set callback functions for authorizing updates and monitoring progress.
           Code is implemented in update_ui_example.cpp
           Both callbacks are completely optional. If no authorization callback
           is set, the update process will procede immediately in each step.
        */
        update_ui_set_cloud_client(&_cloudClient);
        // TODO
        //_cloudClient.set_update_authorize_handler(update_authorize);
        //_cloudClient.set_update_progress_handler(update_progress);
#endif
    }

    return success;
}

// Return true if the cloud client is connected to the server.
bool CloudClientDm::isConnected()
{
    return _registered;
}

// Keep a UDP link up
void CloudClientDm::keepAlive()
{
    _cloudClient.keep_alive();
}

/**********************************************************************
 * PUBLIC METHODS: DEVICE OBJECT
 **********************************************************************/

// Set the Device object Device Type resource.
bool CloudClientDm::setDeviceObjectStaticDeviceType(const char *deviceType)
{
    return setDeviceObjectConfigResource(M2MDevice::DeviceType, deviceType);
}

// Set the Device object Serial Number resource.
bool CloudClientDm::setDeviceObjectStaticSerialNumber(const char *serialNumber)
{
    return setDeviceObjectConfigResource(M2MDevice::SerialNumber, serialNumber);
}

// Set the Device object Hardware Version resource.
bool CloudClientDm::setDeviceObjectStaticHardwareVersion(const char *hardwareVersion)
{
    return setDeviceObjectConfigResource(M2MDevice::HardwareVersion, hardwareVersion);
}

// Set the Device object Software Version resource.
bool CloudClientDm::setDeviceObjectStaticSoftwareVersion(const char *softwareVersion)
{
    return setDeviceObjectConfigResource(M2MDevice::SoftwareVersion, softwareVersion);
}

// Set the Device object Firmware Version resource.
bool CloudClientDm::setDeviceObjectFirmwareVersion(const char *firmwareVersion)
{
    return setDeviceObjectResource(M2MDevice::FirmwareVersion, firmwareVersion);
}

// Add a Device object Available Power Source resource.
bool CloudClientDm::addDeviceObjectPowerSource(PowerSource powerSource)
{
    bool success = false;
    bool foundFree = false;
    uint16_t x;

    // Find a spare instance ID
    for (x = 0; (x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0])) && !foundFree; x++) {
        if (_powerSourceInstance[x] == POWER_SOURCE_UNUSED) {
            foundFree = true;
        }
    }

    // Create the Available Power Source, Voltage and Current
    // for that instance
    if (foundFree) {
        x--;
        _powerSourceInstance[x] = powerSource;
        success = createDeviceObjectResource(M2MDevice::AvailablePowerSources, (int64_t) powerSource, x) &&
                  createDeviceObjectResource(M2MDevice::PowerSourceVoltage, (int64_t) 0, x) &&
                  createDeviceObjectResource(M2MDevice::PowerSourceCurrent, (int64_t) 0, x);
    }

    return success;
}

// Delete a Device object Available Power Source resource.
bool CloudClientDm::deleteDeviceObjectPowerSource(PowerSource powerSource)
{
    bool success = false;
    bool foundIt = false;
    uint16_t x;

    // Find the instance ID
    for (x = 0; (x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0])) && !foundIt; x++) {
        if (_powerSourceInstance[x] == powerSource) {
            foundIt = true;
        }
    }

    // Delete those Available Power Source, Voltage and Current instances
    if (foundIt) {
        x--;
        if (deleteDeviceObjectResource(M2MDevice::AvailablePowerSources, x) &&
            deleteDeviceObjectResource(M2MDevice::PowerSourceVoltage, x) &&
            deleteDeviceObjectResource(M2MDevice::PowerSourceCurrent, x)) {
            _powerSourceInstance[x] = POWER_SOURCE_UNUSED;
            success = true;
        }
    }

    return success;
}

// Set the Device object Power Source Voltage resource.
bool CloudClientDm::setDeviceObjectVoltage(PowerSource powerSource, int voltageMV)
{
    bool success = false;
    bool foundIt = false;
    uint16_t x;

     // Find the instance ID
     for (x = 0; (x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0])) && !foundIt; x++) {
         if (_powerSourceInstance[x] == powerSource) {
             foundIt = true;
         }
     }

     if (foundIt) {
         x--;
         success = setDeviceObjectResource(M2MDevice::PowerSourceVoltage, (int64_t) voltageMV, x);
     }

    return success;
}

// Set the Device object Power Source Current resource.
bool CloudClientDm::setDeviceObjectCurrent(PowerSource powerSource, int currentMA)
{
    bool success = false;
    bool foundIt = false;
    uint16_t x;

     // Find the instance ID
     for (x = 0; (x < sizeof(_powerSourceInstance) / sizeof (_powerSourceInstance[0])) && !foundIt; x++) {
         if (_powerSourceInstance[x] == powerSource) {
             foundIt = true;
         }
     }

     if (foundIt) {
         x--;
         success = setDeviceObjectResource(M2MDevice::PowerSourceCurrent, (int64_t) currentMA, x);
     }

    return success;
}

// Set the Device object Battery Level resource.
bool CloudClientDm::setDeviceObjectBatteryLevel(int batteryLevelPercent)
{
    return setDeviceObjectResource(M2MDevice::BatteryLevel, (int64_t) batteryLevelPercent);
}

// Set the Device object Battery Status resource.
bool CloudClientDm::setDeviceObjectBatteryStatus(CloudClientDm::BatteryStatus batteryStatus)
{
    return setDeviceObjectResource(M2MDevice::BatteryStatus, (int64_t) batteryStatus);
}

// Set the Device object Memory Total resource.
bool CloudClientDm::setDeviceObjectMemoryTotal(int64_t memoryTotalKBytes)
{
    return setDeviceObjectResource(M2MDevice::MemoryTotal, memoryTotalKBytes);
}

// Set the Device object Memory Free resource.
bool CloudClientDm::setDeviceObjectMemoryFree(int64_t memoryFreeKBytes)
{
    return setDeviceObjectResource(M2MDevice::MemoryFree, memoryFreeKBytes);
}

// Set the Device object Error Code resource.
bool CloudClientDm::setDeviceObjectErrorCode(CloudClientDm::Error errorCode)
{
    return setDeviceObjectResource(M2MDevice::ErrorCode, (int64_t) errorCode, 0);
}

// Set the Device object UTC Offset resource.
bool CloudClientDm::setDeviceObjectUtcOffset(const char *utcOffsetISO8601)
{
    return setDeviceObjectResource(M2MDevice::UTCOffset, utcOffsetISO8601);
}

// Set the Device object Timezone resource.
bool CloudClientDm::setDeviceObjectTimezone(const char *timezoneIANA)
{
    return setDeviceObjectResource(M2MDevice::Timezone, timezoneIANA);
}

// End of file
