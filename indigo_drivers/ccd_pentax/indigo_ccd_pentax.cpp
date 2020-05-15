// Copyright (c) 2020 CloudMakers, s. r. o.
// All rights reserved.
//
// You can use this software under the terms of 'INDIGO Astronomy
// open-source license' (see LICENSE.md).
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS 'AS IS' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// version history
// 2.0 by Peter Polakovic <peter.polakovic@cloudmakers.eu>
// 2.0 by Tsunoda Koji <tsunoda@astroarts.co.jp>

/** INDIGO RICOH PENTAX DSLR driver
 \file indigo_ccd_pentax.cpp
 */

#include <memory>
#include <set>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include <pthread.h>

#include <libusb-1.0/libusb.h>

#include <indigo/indigo_usb_utils.h>

#include <ricoh_camera_sdk.hpp>

#include "indigo_ccd_pentax.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace Ricoh::CameraController;

#define MAX_DEVICES    	4
#define VENDOR_RICOH    0x25fb

#define PRIVATE_DATA               ((pentax_private_data *)device->private_data)
#define DRIVER_VERSION              0x0009
#define DRIVER_NAME                 "indigo_ccd_pentax"

#define DSLR_DELETE_IMAGE_PROPERTY      (PRIVATE_DATA->dslr_delete_image_property)
#define DSLR_DELETE_IMAGE_ON_ITEM       (DSLR_DELETE_IMAGE_PROPERTY->items + 0)
#define DSLR_DELETE_IMAGE_OFF_ITEM      (DSLR_DELETE_IMAGE_PROPERTY->items + 1)
#define DSLR_MIRROR_LOCKUP_PROPERTY     (PRIVATE_DATA->dslr_mirror_lockup_property)
#define DSLR_MIRROR_LOCKUP_LOCK_ITEM    (DSLR_MIRROR_LOCKUP_PROPERTY->items + 0)
#define DSLR_MIRROR_LOCKUP_UNLOCK_ITEM  (DSLR_MIRROR_LOCKUP_PROPERTY->items + 1)
#define DSLR_ZOOM_PREVIEW_PROPERTY      (PRIVATE_DATA->dslr_zoom_preview_property)
#define DSLR_ZOOM_PREVIEW_ON_ITEM       (DSLR_ZOOM_PREVIEW_PROPERTY->items + 0)
#define DSLR_ZOOM_PREVIEW_OFF_ITEM      (DSLR_ZOOM_PREVIEW_PROPERTY->items + 1)

#define DSLR_LOCK_PROPERTY              (PRIVATE_DATA->dslr_lock_property)
#define DSLR_LOCK_ITEM                  (DSLR_LOCK_PROPERTY->items + 0)
#define DSLR_UNLOCK_ITEM                (DSLR_LOCK_PROPERTY->items + 1)
#define DSLR_AF_PROPERTY                (PRIVATE_DATA->dslr_af_property)
#define DSLR_AF_ITEM                    (DSLR_AF_PROPERTY->items + 0)
#define DSLR_SET_HOST_TIME_PROPERTY     (PRIVATE_DATA->dslr_set_host_time_property)
#define DSLR_SET_HOST_TIME_ITEM         (DSLR_SET_HOST_TIME_PROPERTY->items + 0)

typedef struct {
	std::shared_ptr<CameraDevice> device;
	indigo_property *dslr_delete_image_property;
	indigo_property *dslr_mirror_lockup_property;
	indigo_property *dslr_zoom_preview_property;
	indigo_property *dslr_lock_property;
	indigo_property *dslr_af_property;
	indigo_property *dslr_set_host_time_property;
	bool abort_capture;
	void *image_buffer;
} pentax_private_data;


static indigo_device *devices[MAX_DEVICES];


static indigo_result ccd_enumerate_properties(indigo_device *device, indigo_client *client, indigo_property *property);

static indigo_result ccd_attach(indigo_device *device) {
	assert(device != NULL);
	assert(PRIVATE_DATA != NULL);
	if (indigo_ccd_attach(device, DRIVER_VERSION) == INDIGO_OK) {
		// --------------------------------------------------------------------------------
		CCD_MODE_PROPERTY->hidden = true;
		CCD_STREAMING_PROPERTY->hidden = true;
		CCD_IMAGE_FORMAT_PROPERTY->hidden = true;
		CCD_FRAME_PROPERTY->hidden = true;
		CCD_INFO_WIDTH_ITEM->number.value = 0;
		CCD_INFO_HEIGHT_ITEM->number.value = 0;
		CCD_INFO_PIXEL_WIDTH_ITEM->number.value = 0;
		CCD_INFO_BITS_PER_PIXEL_ITEM->number.value = 16;
		// -------------------------------------------------------------------------------- DSLR_DELETE_IMAGE
		DSLR_DELETE_IMAGE_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_DELETE_IMAGE_PROPERTY_NAME, "DSLR", "Delete downloaded image", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_ONE_OF_MANY_RULE, 2);
		if (DSLR_DELETE_IMAGE_PROPERTY == NULL)
			return INDIGO_FAILED;
		indigo_init_switch_item(DSLR_DELETE_IMAGE_ON_ITEM, DSLR_ZOOM_PREVIEW_ON_ITEM_NAME, "On", true);
		indigo_init_switch_item(DSLR_DELETE_IMAGE_OFF_ITEM, DSLR_ZOOM_PREVIEW_OFF_ITEM_NAME, "Off", false);
		// -------------------------------------------------------------------------------- DSLR_MIRROR_LOCKUP
		DSLR_MIRROR_LOCKUP_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_MIRROR_LOCKUP_PROPERTY_NAME, "DSLR", "Use mirror lockup", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_ONE_OF_MANY_RULE, 2);
		if (DSLR_MIRROR_LOCKUP_PROPERTY == NULL)
			return INDIGO_FAILED;
		indigo_init_switch_item(DSLR_MIRROR_LOCKUP_LOCK_ITEM, DSLR_MIRROR_LOCKUP_LOCK_ITEM_NAME, "Lock", false);
		indigo_init_switch_item(DSLR_MIRROR_LOCKUP_UNLOCK_ITEM, DSLR_MIRROR_LOCKUP_UNLOCK_ITEM_NAME, "Unlock", true);
		// -------------------------------------------------------------------------------- DSLR_ZOOM_PREVIEW
		DSLR_ZOOM_PREVIEW_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_ZOOM_PREVIEW_PROPERTY_NAME, "DSLR", "Zoom preview", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_ONE_OF_MANY_RULE, 2);
		if (DSLR_ZOOM_PREVIEW_PROPERTY == NULL)
			return INDIGO_FAILED;
// TODO:
//		DSLR_ZOOM_PREVIEW_PROPERTY->hidden = PRIVATE_DATA->zoom == NULL;
		indigo_init_switch_item(DSLR_ZOOM_PREVIEW_ON_ITEM, DSLR_ZOOM_PREVIEW_ON_ITEM_NAME, "On", false);
		indigo_init_switch_item(DSLR_ZOOM_PREVIEW_OFF_ITEM, DSLR_ZOOM_PREVIEW_OFF_ITEM_NAME, "Off", true);
		// -------------------------------------------------------------------------------- DSLR_LOCK
		DSLR_LOCK_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_LOCK_PROPERTY_NAME, "DSLR", "Lock camera GUI", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_ONE_OF_MANY_RULE, 2);
		if (DSLR_LOCK_PROPERTY == NULL)
			return INDIGO_FAILED;
// TODO:
//		DSLR_LOCK_PROPERTY->hidden = PRIVATE_DATA->lock == NULL;
		indigo_init_switch_item(DSLR_LOCK_ITEM, DSLR_LOCK_ITEM_NAME, "Lock", false);
		indigo_init_switch_item(DSLR_UNLOCK_ITEM, DSLR_UNLOCK_ITEM_NAME, "Unlock", true);
		// -------------------------------------------------------------------------------- DSLR_AF
		DSLR_AF_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_AF_PROPERTY_NAME, "DSLR", "Autofocus", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_AT_MOST_ONE_RULE, 1);
		if (DSLR_AF_PROPERTY == NULL)
			return INDIGO_FAILED;
// TODO:
//		DSLR_AF_PROPERTY->hidden = PRIVATE_DATA->af == NULL;
		indigo_init_switch_item(DSLR_AF_ITEM, DSLR_AF_ITEM_NAME, "Start autofocus", false);
		// -------------------------------------------------------------------------------- DSLR_SET_HOST_TIME
		DSLR_SET_HOST_TIME_PROPERTY = indigo_init_switch_property(NULL, device->name, DSLR_SET_HOST_TIME_PROPERTY_NAME, "DSLR", "Set host time", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_AT_MOST_ONE_RULE, 1);
		if (DSLR_SET_HOST_TIME_PROPERTY == NULL)
			return INDIGO_FAILED;
// TODO:
//		DSLR_SET_HOST_TIME_PROPERTY->hidden = PRIVATE_DATA->set_host_time == NULL;
		indigo_init_switch_item(DSLR_SET_HOST_TIME_ITEM, DSLR_SET_HOST_TIME_ITEM_NAME, "Set host time", false);
		// --------------------------------------------------------------------------------
		INDIGO_DEVICE_ATTACH_LOG(DRIVER_NAME, device->name);
		return ccd_enumerate_properties(device, NULL, NULL);
	}
	return INDIGO_FAILED;
}

static indigo_result ccd_enumerate_properties(indigo_device *device, indigo_client *client, indigo_property *property) {
	assert(device != NULL);
	assert(DEVICE_CONTEXT != NULL);
	if (IS_CONNECTED) {
		if (indigo_property_match(DSLR_DELETE_IMAGE_PROPERTY, property))
			indigo_define_property(device, DSLR_DELETE_IMAGE_PROPERTY, NULL);
		if (indigo_property_match(DSLR_MIRROR_LOCKUP_PROPERTY, property))
			indigo_define_property(device, DSLR_MIRROR_LOCKUP_PROPERTY, NULL);
		if (indigo_property_match(DSLR_ZOOM_PREVIEW_PROPERTY, property))
			indigo_define_property(device, DSLR_ZOOM_PREVIEW_PROPERTY, NULL);
		if (indigo_property_match(DSLR_LOCK_PROPERTY, property))
			indigo_define_property(device, DSLR_LOCK_PROPERTY, NULL);
		if (indigo_property_match(DSLR_AF_PROPERTY, property))
			indigo_define_property(device, DSLR_AF_PROPERTY, NULL);
		if (indigo_property_match(DSLR_SET_HOST_TIME_PROPERTY, property))
			indigo_define_property(device, DSLR_SET_HOST_TIME_PROPERTY, NULL);
		// TODO: from SDK Config
	}
	return indigo_ccd_enumerate_properties(device, client, property);
}

static void handle_set_property(indigo_device *device) {
	// TODO:
}

static void handle_streaming(indigo_device *device) {
	// TODO:
}

static void handle_exposure(indigo_device *device) {
	// TODO:
}

static indigo_result ccd_change_property(indigo_device *device, indigo_client *client, indigo_property *property) {
	assert(device != NULL);
	assert(DEVICE_CONTEXT != NULL);
	assert(property != NULL);
	if (indigo_property_match(CONNECTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- CONNECTION
		indigo_property_copy_values(CONNECTION_PROPERTY, property, false);
		if (CONNECTION_CONNECTED_ITEM->sw.value) {
			CONNECTION_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_property(device, CONNECTION_PROPERTY, NULL);
			auto resp = PRIVATE_DATA->device->connect(DeviceInterface::USB);
			auto state = resp.getResult() == Result::Ok ? INDIGO_OK_STATE : INDIGO_ALERT_STATE;
			CONNECTION_PROPERTY->state = state;
			indigo_update_property(device, CONNECTION_PROPERTY, NULL);
			return state == INDIGO_OK_STATE ? INDIGO_OK : INDIGO_FAILED;
		} else {
			CONNECTION_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_property(device, CONNECTION_PROPERTY, NULL);
			auto resp = PRIVATE_DATA->device->disconnect(DeviceInterface::USB);
			auto state = resp.getResult() == Result::Ok ? INDIGO_OK_STATE : INDIGO_ALERT_STATE;
			CONNECTION_PROPERTY->state = state;
			indigo_delete_property(device, DSLR_DELETE_IMAGE_PROPERTY, NULL);
			indigo_delete_property(device, DSLR_MIRROR_LOCKUP_PROPERTY, NULL);
			indigo_delete_property(device, DSLR_ZOOM_PREVIEW_PROPERTY, NULL);
			indigo_delete_property(device, DSLR_LOCK_PROPERTY, NULL);
			indigo_delete_property(device, DSLR_AF_PROPERTY, NULL);
			indigo_delete_property(device, DSLR_SET_HOST_TIME_PROPERTY, NULL);
			if (PRIVATE_DATA->image_buffer) {
				free(PRIVATE_DATA->image_buffer);
				PRIVATE_DATA->image_buffer = NULL;
			}
			indigo_global_unlock(device);
			CONNECTION_PROPERTY->state = INDIGO_OK_STATE;
			return state == INDIGO_OK_STATE ? INDIGO_OK : INDIGO_FAILED;
		}
	} else if (indigo_property_match(DSLR_DELETE_IMAGE_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_DELETE_IMAGE_PROPERTY
		indigo_property_copy_values(DSLR_DELETE_IMAGE_PROPERTY, property, false);
		DSLR_DELETE_IMAGE_PROPERTY->state = INDIGO_OK_STATE;
		indigo_update_property(device, DSLR_DELETE_IMAGE_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(DSLR_MIRROR_LOCKUP_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_MIRROR_LOCKUP
		indigo_property_copy_values(DSLR_MIRROR_LOCKUP_PROPERTY, property, false);
		DSLR_MIRROR_LOCKUP_PROPERTY->state = INDIGO_OK_STATE;
		indigo_update_property(device, DSLR_MIRROR_LOCKUP_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(DSLR_ZOOM_PREVIEW_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_ZOOM_PREVIEW
		indigo_property_copy_values(DSLR_ZOOM_PREVIEW_PROPERTY, property, false);
//		indigo_set_timer(device, 0, handle_zoom);
		return INDIGO_OK;
	} else if (indigo_property_match(DSLR_LOCK_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_LOCK
		indigo_property_copy_values(DSLR_LOCK_PROPERTY, property, false);
//		indigo_set_timer(device, 0, handle_lock);
		return INDIGO_OK;
	} else if (indigo_property_match(DSLR_AF_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_AF
		indigo_property_copy_values(DSLR_AF_PROPERTY, property, false);
//		indigo_set_timer(device, 0, handle_af);
		return INDIGO_OK;
	} else if (indigo_property_match(DSLR_SET_HOST_TIME_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DSLR_AF
		indigo_property_copy_values(DSLR_SET_HOST_TIME_PROPERTY, property, false);
//		indigo_set_timer(device, 0, handle_set_host_time);
		return INDIGO_OK;
		// -------------------------------------------------------------------------------- CCD_EXPOSURE
	} else if (indigo_property_match(CCD_EXPOSURE_PROPERTY, property)) {
		indigo_property_copy_values(CCD_EXPOSURE_PROPERTY, property, false);
//		indigo_set_timer(device, 0, handle_exposure);
		// -------------------------------------------------------------------------------- CCD_STREAMING
	} else if (indigo_property_match(CCD_STREAMING_PROPERTY, property)) {
		indigo_property_copy_values(CCD_STREAMING_PROPERTY, property, false);
		PRIVATE_DATA->abort_capture = false;
//		indigo_set_timer(device, 0, handle_streaming);
		// -------------------------------------------------------------------------------- CCD_ABORT_EXPOSURE
	} else if (indigo_property_match(CCD_ABORT_EXPOSURE_PROPERTY, property)) {
		indigo_property_copy_values(CCD_ABORT_EXPOSURE_PROPERTY, property, false);
		if (CCD_ABORT_EXPOSURE_ITEM->sw.value) {
			CCD_ABORT_EXPOSURE_ITEM->sw.value = false;
			PRIVATE_DATA->abort_capture = true;
		}
		indigo_update_property(device, CCD_ABORT_EXPOSURE_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(CCD_PREVIEW_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- CCD_PREVIEW
		indigo_property_copy_values(CCD_PREVIEW_PROPERTY, property, false);
		if (CCD_PREVIEW_ENABLED_ITEM->sw.value) {
			if (CCD_PREVIEW_IMAGE_PROPERTY->hidden) {
				CCD_PREVIEW_IMAGE_PROPERTY->hidden = false;
				if (IS_CONNECTED)
					indigo_define_property(device, CCD_PREVIEW_IMAGE_PROPERTY, NULL);
			}
		} else {
			if (!CCD_PREVIEW_IMAGE_PROPERTY->hidden) {
				if (IS_CONNECTED)
					indigo_delete_property(device, CCD_PREVIEW_IMAGE_PROPERTY, NULL);
				CCD_PREVIEW_IMAGE_PROPERTY->hidden = true;
			}
		}
		CCD_PREVIEW_PROPERTY->state = INDIGO_OK_STATE;
		if (IS_CONNECTED)
			indigo_update_property(device, CCD_PREVIEW_PROPERTY, NULL);
		return INDIGO_OK;
	} else {
		// TODO:
	}
	return indigo_ccd_change_property(device, client, property);
}

static indigo_result ccd_detach(indigo_device *device) {
	assert(device != NULL);
	if (CONNECTION_CONNECTED_ITEM->sw.value)
		indigo_device_disconnect(NULL, device->name);
	indigo_release_property(DSLR_DELETE_IMAGE_PROPERTY);
	indigo_release_property(DSLR_MIRROR_LOCKUP_PROPERTY);
	indigo_release_property(DSLR_ZOOM_PREVIEW_PROPERTY);
	indigo_release_property(DSLR_LOCK_PROPERTY);
	indigo_release_property(DSLR_AF_PROPERTY);
	indigo_release_property(DSLR_SET_HOST_TIME_PROPERTY);
	INDIGO_DEVICE_DETACH_LOG(DRIVER_NAME, device->name);
	return indigo_ccd_detach(device);
}

static indigo_result focuser_attach(indigo_device *device) {
	assert(device != NULL);
	assert(PRIVATE_DATA != NULL);
	if (indigo_focuser_attach(device, DRIVER_VERSION) == INDIGO_OK) {
		// --------------------------------------------------------------------------------
		FOCUSER_POSITION_PROPERTY->hidden = true;
		FOCUSER_SPEED_PROPERTY->hidden = true;
		// --------------------------------------------------------------------------------
		INDIGO_DEVICE_ATTACH_LOG(DRIVER_NAME, device->name);
		return ccd_enumerate_properties(device, NULL, NULL);
	}
	return INDIGO_FAILED;
}


// -------------------------------------------------------------------------------- hot-plug support

static pthread_mutex_t device_mutex = PTHREAD_MUTEX_INITIALIZER;

static std::map<std::string, std::shared_ptr<CameraDevice>> detectedCameras;

static int find_available_device_slot() {
	for(int slot = 0; slot < MAX_DEVICES; slot++) {
		if (devices[slot] == NULL) return slot;
	}
	return -1;
}


static int find_device_slot(const char *str) {
	for(int slot = 0; slot < MAX_DEVICES; slot++) {
		indigo_device *device = devices[slot];
		if (device == NULL) continue;
		if (PRIVATE_DATA->device->getSerialNumber() == str) return slot;
	}
	return -1;
}

static int hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
	static indigo_device ccd_template = INDIGO_DEVICE_INITIALIZER(
		"",
		ccd_attach,
		ccd_enumerate_properties,
		ccd_change_property,
		NULL,
		ccd_detach
	);
/*
	static indigo_device focuser_template = INDIGO_DEVICE_INITIALIZER(
		"",
		focuser_attach,
		indigo_focuser_enumerate_properties,
		focuser_change_property,
		NULL,
		focuser_detach
	);
*/
	struct libusb_device_descriptor descriptor;

	pthread_mutex_lock(&device_mutex);
	switch (event) {
		case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED: {
			int rc = libusb_get_device_descriptor(dev, &descriptor);
			INDIGO_DRIVER_DEBUG(DRIVER_NAME, "libusb_get_device_descriptor ->  %s", rc < 0 ? libusb_error_name(rc) : "OK");
			if (descriptor.idVendor == VENDOR_RICOH) {
				// RICOH camera detected
			  auto detectedCameraDevices = CameraDeviceDetector::detect(DeviceInterface::USB);
				INDIGO_DRIVER_DEBUG(DRIVER_NAME, "On arrived: %d cameras detected.", detectedCameraDevices.size());
				for (auto camera : detectedCameraDevices) {
					const auto serialNumber = camera->getSerialNumber();
					if ( detectedCameras.find(serialNumber) != detectedCameras.end() ) {
						// already detected
						continue;
					}
					// new device
					detectedCameras[serialNumber] = camera;

					indigo_device *device = (indigo_device*)malloc(sizeof(indigo_device));
					assert(device != NULL);
					memcpy(device, &ccd_template, sizeof(indigo_device));
					device->master_device = device;
					sprintf(device->name, "%s #%s", camera->getModel().c_str(), serialNumber.c_str());
					INDIGO_DEVICE_ATTACH_LOG(DRIVER_NAME, device->name);
					pentax_private_data *private_data = (pentax_private_data *)malloc(sizeof(pentax_private_data));
					assert(private_data);
					memset(private_data, 0, sizeof(pentax_private_data));
					private_data->device = camera;
					device->private_data = private_data;
					indigo_async(reinterpret_cast<void*(*)(void*)>(indigo_attach_device), device);
					// TODO: focus
					for (int j = 0; j < MAX_DEVICES; j++) {
						if (devices[j] == NULL) {
							indigo_async(reinterpret_cast<void*(*)(void*)>(indigo_attach_device), devices[j] = device);
							break;
						}
					}
				}
			}
			break;
		}
		case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT: {
			if (descriptor.idVendor == VENDOR_RICOH) {
				// RICOH camera removed
			  std::vector<std::shared_ptr<CameraDevice>> detectedCameraDevices =
						CameraDeviceDetector::detect(DeviceInterface::USB);
				INDIGO_DRIVER_DEBUG(DRIVER_NAME, "On left: %d cameras detected.", detectedCameraDevices.size());
				std::set<std::string> detectedSerials;
				for (auto camera : detectedCameraDevices) {
					detectedSerials.insert(camera->getSerialNumber());
				}
				for (int j = 0; j < MAX_DEVICES; j++) {
					if (devices[j] != NULL) {
						indigo_device *device = devices[j];
						if (PRIVATE_DATA->device && detectedSerials.find(PRIVATE_DATA->device->getSerialNumber()) == detectedSerials.end()) {
							PRIVATE_DATA->device = nullptr;
							free(PRIVATE_DATA);
							indigo_detach_device(device);
							free(device);
							devices[j] = NULL;
						}
					}
				}
			}
		}
	}
	pthread_mutex_unlock(&device_mutex);
	return 0;
};

static libusb_hotplug_callback_handle callback_handle;

indigo_result indigo_ccd_pentax(indigo_driver_action action, indigo_driver_info *info) {

	static indigo_driver_action last_action = INDIGO_DRIVER_SHUTDOWN;

	SET_DRIVER_INFO(info, "RICOH PENTAX Camera", __FUNCTION__, DRIVER_VERSION, true, last_action);

	if (action == last_action)
		return INDIGO_OK;

	switch (action) {
		case INDIGO_DRIVER_INIT: {
			last_action = action;
			for (int i = 0; i < MAX_DEVICES; i++) {
				devices[i] = 0;
			}
			indigo_start_usb_event_handler();
			int rc = libusb_hotplug_register_callback(NULL, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &callback_handle);
			INDIGO_DRIVER_DEBUG(DRIVER_NAME, "libusb_hotplug_register_callback ->  %s", rc < 0 ? libusb_error_name(rc) : "OK");
			return rc >= 0 ? INDIGO_OK : INDIGO_FAILED;
			}
			break;
		case INDIGO_DRIVER_SHUTDOWN: {
			last_action = action;
			libusb_hotplug_deregister_callback(NULL, callback_handle);
			INDIGO_DRIVER_DEBUG(DRIVER_NAME, "libusb_hotplug_deregister_callback");
/*
			for (int j = 0; j < MAX_DEVICES; j++) {
				if (devices[j] != NULL) {
					indigo_device *device = devices[j];
					hotplug_callback(NULL, PRIVATE_DATA->dev, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, NULL);
				}
			}
*/
			}
			break;

		case INDIGO_DRIVER_INFO: {
			}
			break;
	}

	return INDIGO_OK;
}

#ifdef __cplusplus
}
#endif
