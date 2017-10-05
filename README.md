Introduction
============
This class provides a friendly API to the Mbed Cloud Client and its Device Management functionality.

Please  refer to http://www.openmobilealliance.org/wp/OMNA/LwM2M/LwM2MRegistry.html and particularly objects `urn:oma:lwm2m:oma:3` and `urn:oma:lwm2m:oma:5` for a description of the objects and resources involved.

Usage
=====
The pattern of usage is as follows:

1.  Instantiate CloudClientDm, with debug either on or off.
2.  Configure CloudClientDm by calling the "set" or "add" methods
    for the Device object resourcs you would like to include.  Note
    that some resources may be added by Mbed Cloud Client itself but
    there is no harm in you adding them, they will not be
    duplicated and your value will overwrite the previous value.
3.  Start CloudClientDm; this creates the Device object and its
    resources according to your configuration.
4.  Register CloudClientDm with a LWM2M server and use it.
5.  When finished, stop CloudClientDm.
6.  Delete CloudClientDm.

Note: the following DM object resources:

* Manufacturer,
* Model Number,
* Supported Binding Mode,
* Time,

..are managed entirely by the mbed cloud client and cannot be modified through this interface.