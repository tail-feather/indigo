#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <indigo/indigo_bus.h>

#include "common.hpp"



struct py_indigo_device {
    indigo_device that;

    py_indigo_device() : that {} {
        that.device_context = this;
        that.attach = &emit_attach;
        that.enumerate_properties = &emit_enumerate_properties;
        that.change_property = &emit_change_property;
        that.enable_blob = &emit_enable_blob;
        that.detach = &emit_detach;
    }
    virtual void on_attach(indigo_device *device) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_device, on_attach, device);
    }
    virtual void on_enumerate_properties(indigo_device *device, indigo_client *client, indigo_property *property) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_device, on_enumerate_properties, device, client, property);
    }
    virtual void on_change_property(indigo_device *device, indigo_client *client, indigo_property *property) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_device, on_change_property, device, client, property);
    }
    virtual void on_enable_blob(indigo_device *device, indigo_client *client, indigo_property *property, indigo_enable_blob_mode mode) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_device, on_enable_blob, device, client, property, mode);
    }
    virtual void on_detach(indigo_device *device) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_device, on_detach, device);
    }

    operator indigo_device *() {
        return &that;
    }

    static py_indigo_device *resolve(indigo_device *device) {
        return reinterpret_cast<py_indigo_device *>(device->device_context);
    }

    static indigo_result emit_attach(indigo_device *device) {
        auto self = resolve(device);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_attach(device);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_enumerate_properties(indigo_device *device, indigo_client *client, indigo_property *property) {
        auto self = resolve(device);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_enumerate_properties(device, client, property);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_change_property(indigo_device *device, indigo_client *client, indigo_property *property) {
        auto self = resolve(device);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_change_property(device, client, property);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_enable_blob(indigo_device *device, indigo_client *client, indigo_property *property, indigo_enable_blob_mode mode) {
        auto self = resolve(device);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_enable_blob(device, client, property, mode);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_detach(indigo_device *device) {
        auto self = resolve(device);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_detach(device);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
};


struct py_indigo_client : public indigo_client {
    indigo_client that;

    py_indigo_client() : that {} {
        // indigo_device defined by extern "C"
        that.client_context = this;
        that.attach = &emit_attach;
        that.define_property = &emit_define_property;
        that.update_property = &emit_update_property;
        that.delete_property = &emit_delete_property;
        that.send_message = &emit_send_message;
        that.detach = &emit_detach;
    }
    virtual void on_attach(indigo_client *client) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_attach, client);
    }
    virtual void on_define_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_define_property, client, device, property, value);
    }
    virtual void on_update_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_update_property, client, device, property, value);
    }
    virtual void on_delete_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_delete_property, client, device, property, value);
    }
    virtual void on_send_message(indigo_client *client, indigo_device *device, const char *message) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_send_message, client, device, message);
    }
    virtual void on_detach(indigo_client *client) {
        PYBIND11_OVERLOAD_PURE(void, py_indigo_client, on_detach, client);
    }

    operator indigo_client *() {
        return &that;
    }

    static py_indigo_client *resolve(indigo_client *client) {
        return reinterpret_cast<py_indigo_client *>(client->client_context);
    }

    static indigo_result emit_attach(indigo_client *client) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_attach(client);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_define_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_define_property(client, device, property, value);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_update_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_update_property(client, device, property, value);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_delete_property(indigo_client *client, indigo_device *device, indigo_property *property, const char *value) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_delete_property(client, device, property, value);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_send_message(indigo_client *client, indigo_device *device, const char *message) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_send_message(client, device, message);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
    static indigo_result emit_detach(indigo_client *client) {
        auto self = resolve(client);
        if ( ! self ) {
            return INDIGO_FAILED;
        }
        try {
            self->on_detach(client);
            return INDIGO_OK;
        } catch (pybind11::error_already_set &e) {
            return INDIGO_FAILED;
        }
    }
};


void init_indigo_bus(pybind11::module &scope) {
    pybind11::enum_<indigo_device_interface>(scope, "indigo_device_interface")
        .value("INDIGO_INTERFACE_MOUNT", INDIGO_INTERFACE_MOUNT)
        .value("INDIGO_INTERFACE_CCD", INDIGO_INTERFACE_CCD)
        .value("INDIGO_INTERFACE_GUIDER", INDIGO_INTERFACE_GUIDER)
        .value("INDIGO_INTERFACE_FOCUSER", INDIGO_INTERFACE_FOCUSER)
        .value("INDIGO_INTERFACE_WHEEL", INDIGO_INTERFACE_WHEEL)
        .value("INDIGO_INTERFACE_DOME", INDIGO_INTERFACE_DOME)
        .value("INDIGO_INTERFACE_GPS", INDIGO_INTERFACE_GPS)
        .value("INDIGO_INTERFACE_WEATHER", INDIGO_INTERFACE_WEATHER)
        .value("INDIGO_INTERFACE_AO", INDIGO_INTERFACE_AO)
        .value("INDIGO_INTERFACE_AUX", INDIGO_INTERFACE_AUX)
        .value("INDIGO_INTERFACE_AUX_JOYSTICK", INDIGO_INTERFACE_AUX_JOYSTICK)
        .value("INDIGO_INTERFACE_AUX_SHUTTER", INDIGO_INTERFACE_AUX_SHUTTER)
        .value("INDIGO_INTERFACE_AUX_POWERBOX", INDIGO_INTERFACE_AUX_POWERBOX)
        .value("INDIGO_INTERFACE_AUX_SQM", INDIGO_INTERFACE_AUX_SQM)
        .value("INDIGO_INTERFACE_AUX_DUSTCAP", INDIGO_INTERFACE_AUX_DUSTCAP)
        .value("INDIGO_INTERFACE_AUX_LIGHTBOX", INDIGO_INTERFACE_AUX_LIGHTBOX)
        .export_values()
        ;

    pybind11::enum_<indigo_version>(scope, "indigo_version")
        .value("INDIGO_VERSION_NONE", INDIGO_VERSION_NONE)
        .value("INDIGO_VERSION_LEGACY", INDIGO_VERSION_LEGACY)
        .value("INDIGO_VERSION_2_0", INDIGO_VERSION_2_0)
        .value("INDIGO_VERSION_CURRENT", INDIGO_VERSION_CURRENT)
        .export_values()
        ;

    pybind11::enum_<indigo_result>(scope, "indigo_result")
        .value("INDIGO_OK", INDIGO_OK)
        .value("INDIGO_FAILED", INDIGO_FAILED)
        .value("INDIGO_TOO_MANY_ELEMENTS", INDIGO_TOO_MANY_ELEMENTS)
        .value("INDIGO_LOCK_ERROR", INDIGO_LOCK_ERROR)
        .value("INDIGO_NOT_FOUND", INDIGO_NOT_FOUND)
        .value("INDIGO_CANT_START_SERVER", INDIGO_CANT_START_SERVER)
        .value("INDIGO_DUPLICATED", INDIGO_DUPLICATED)
        .export_values()
        ;

    pybind11::enum_<indigo_property_type>(scope, "indigo_property_type")
        .value("INDIGO_TEXT_VECTOR", INDIGO_TEXT_VECTOR)
        .value("INDIGO_NUMBER_VECTOR", INDIGO_NUMBER_VECTOR)
        .value("INDIGO_SWITCH_VECTOR", INDIGO_SWITCH_VECTOR)
        .value("INDIGO_LIGHT_VECTOR", INDIGO_LIGHT_VECTOR)
        .value("INDIGO_BLOB_VECTOR", INDIGO_BLOB_VECTOR)
        .export_values()
        ;

    pybind11::enum_<indigo_property_state>(scope, "indigo_property_state")
        .value("INDIGO_IDLE_STATE", INDIGO_IDLE_STATE)
        .value("INDIGO_OK_STATE", INDIGO_OK_STATE)
        .value("INDIGO_BUSY_STATE", INDIGO_BUSY_STATE)
        .value("INDIGO_ALERT_STATE", INDIGO_ALERT_STATE)
        .export_values()
        ;

    pybind11::enum_<indigo_property_perm>(scope, "indigo_property_perm")
        .value("INDIGO_RO_PERM", INDIGO_RO_PERM)
        .value("INDIGO_RW_PERM", INDIGO_RW_PERM)
        .value("INDIGO_WO_PERM", INDIGO_WO_PERM)
        .export_values()
        ;

    pybind11::enum_<indigo_rule>(scope, "indigo_rule")
        .value("INDIGO_ONE_OF_MANY_RULE", INDIGO_ONE_OF_MANY_RULE)
        .value("INDIGO_AT_MOST_ONE_RULE", INDIGO_AT_MOST_ONE_RULE)
        .value("INDIGO_ANY_OF_MANY_RULE", INDIGO_ANY_OF_MANY_RULE)
        .export_values()
        ;

    pybind11::enum_<indigo_enable_blob_mode>(scope, "indigo_enable_blob_mode")
        .value("INDIGO_ENABLE_BLOB_ALSO", INDIGO_ENABLE_BLOB_ALSO)
        .value("INDIGO_ENABLE_BLOB_NEVER", INDIGO_ENABLE_BLOB_NEVER)
        .value("INDIGO_ENABLE_BLOB_URL", INDIGO_ENABLE_BLOB_URL)
        .export_values()
        ;

    pybind11::enum_<indigo_raw_type>(scope, "indigo_raw_type")
        .value("INDIGO_RAW_MONO8", INDIGO_RAW_MONO8)
        .value("INDIGO_RAW_MONO16", INDIGO_RAW_MONO16)
        .value("INDIGO_RAW_RGB24", INDIGO_RAW_RGB24)
        .value("INDIGO_RAW_RGB48", INDIGO_RAW_RGB48)
        .export_values()
        ;

    pybind11::enum_<indigo_log_levels>(scope, "indigo_log_levels")
        .value("INDIGO_LOG_ERROR", INDIGO_LOG_ERROR)
        .value("INDIGO_LOG_INFO", INDIGO_LOG_INFO)
        .value("INDIGO_LOG_DEBUG", INDIGO_LOG_DEBUG)
        .value("INDIGO_LOG_TRACE", INDIGO_LOG_TRACE)
        .export_values()
        ;

    pybind11::class_<indigo_enable_blob_mode_record>(scope, "indigo_enable_blob_mode_record")
        .def_property("device",
            char_array_getter(&indigo_enable_blob_mode_record::device),
            char_array_setter(&indigo_enable_blob_mode_record::device))
        .def_property("name",
            char_array_getter(&indigo_enable_blob_mode_record::name),
            char_array_setter(&indigo_enable_blob_mode_record::name))
        .def_readwrite("mode", &indigo_enable_blob_mode_record::mode)
        .def_readwrite("next", &indigo_enable_blob_mode_record::next)
        ;

    pybind11::class_<indigo_raw_header>(scope, "indigo_raw_header")
        .def_readwrite("signature", &indigo_raw_header::signature)
        .def_readwrite("width", &indigo_raw_header::width)
        .def_readwrite("height", &indigo_raw_header::height)
        ;

    pybind11::class_<indigo_item>(scope, "indigo_item")
        .def_property("name",
            char_array_getter(&indigo_item::name),
            char_array_setter(&indigo_item::name))
        .def_property("label",
            char_array_getter(&indigo_item::label),
            char_array_setter(&indigo_item::label))
        .def_property("hints",
            char_array_getter(&indigo_item::hints),
            char_array_setter(&indigo_item::hints))
        // TODO: union attributes
        //.def_property("text")
        //.def_property("number")
        //.def_property("text")
        //.def_property("sw")
        //.def_property("light")
        //.def_property("blob")
        .def("validate_blob", [](indigo_item &self) {
            return indigo_validate_blob(&self);
        })
        .def("init_text_item", [](indigo_item &self, const char *name, const char *label, const char *value) {
            indigo_init_text_item(&self, name, label, value);
        })
        .def("init_number_item", [](indigo_item &self, const char *name, const char *label, double min, double max, double step, double value) {
            indigo_init_number_item(&self, name, label, min, max, step, value);
        })
        .def("init_sexagesimal_number_item", [](indigo_item *self, const char *name, const char *label, double min, double max, double step, double value) {
            indigo_init_sexagesimal_number_item(self, name, label, min, max, step, value);
        })
        .def("init_switch_item", [](indigo_item &self, const char *name, const char *label, bool value) {
            indigo_init_switch_item(&self, name, label, value);
        })
        .def("init_light_item", [](indigo_item &self, const char *name, const char *label, indigo_property_state value) {
            indigo_init_light_item(&self, name, label, value);
        })
        .def("init_blob_item", [](indigo_item &self, const char *name, const char *label) {
            indigo_init_blob_item(&self, name, label);
        })
        .def("populate_http_blob_item", [](indigo_item &self) {
            return indigo_populate_http_blob_item(&self);
        })
        .def("switch_match", [](indigo_item &self, indigo_property *other) {
            return indigo_switch_match(&self, other);
        })
        ;

    pybind11::class_<indigo_property>(scope, "indigo_property")
        .def_property("device",
            char_array_getter(&indigo_property::device),
            char_array_setter(&indigo_property::device))
        .def_property("name",
            char_array_getter(&indigo_property::name),
            char_array_setter(&indigo_property::name))
        .def_property("group",
            char_array_getter(&indigo_property::group),
            char_array_setter(&indigo_property::group))
        .def_property("label",
            char_array_getter(&indigo_property::label),
            char_array_setter(&indigo_property::label))
        .def_property("hints",
            char_array_getter(&indigo_property::hints),
            char_array_setter(&indigo_property::hints))
        .def_readwrite("state", &indigo_property::state)
        .def_readwrite("type", &indigo_property::type)
        .def_readwrite("perm", &indigo_property::perm)
        .def_readwrite("rule", &indigo_property::rule)
        .def_readwrite("version", &indigo_property::version)
        .def_readwrite("hidden", &indigo_property::hidden)
        .def_readwrite("count", &indigo_property::count)
        // TODO: make array (use `count`)
        //.def_readwrite("items", &indigo_property::items)
        // alias
        .def("init_text_property", [](indigo_property &self, const char *device, const char *name, const char *group, const char *label, indigo_property_state state, indigo_property_perm perm, int count) {
            return indigo_init_text_property(&self, device, name, group, label, state, perm, count);
        })
        .def("init_number_property", [](indigo_property &self, const char *device, const char *name, const char *group, const char *label, indigo_property_state state, indigo_property_perm perm, int count) {
            return indigo_init_number_property(&self, device, name, group, label, state, perm, count);
        })
        .def("init_switch_property", [](indigo_property &self, const char *device, const char *name, const char *group, const char *label, indigo_property_state state, indigo_property_perm perm, indigo_rule rule, int count) {
            return indigo_init_switch_property(&self, device, name, group, label, state, perm, rule, count);
        })
        .def("init_light_property", [](indigo_property &self, const char *device, const char *name, const char *group, const char *label, indigo_property_state state, indigo_property_perm perm, int count) {
            return indigo_init_light_property(&self, device, name, group, label, state, count);
        })
        .def("init_blob_property", [](indigo_property &self, const char *device, const char *name, const char *group, const char *label, indigo_property_state state, indigo_property_perm perm, int count) {
            return indigo_init_blob_property(&self, device, name, group, label, state, count);
        })
        .def("resize_property", [](indigo_property &self, int count) {
            return indigo_resize_property(&self, count);
        })
        .def("release_property", [](indigo_property &self) {
            return indigo_release_property(&self);
        })
        .def("property_match", [](indigo_property &self, indigo_property *other) {
            return indigo_property_match(&self, other);
        })
        .def("set_switch", [](indigo_property &self, indigo_item *item, bool value) {
            indigo_set_switch(&self, item, value);
        })
        .def("get_item", [](indigo_property &self, const char *item_name) {
            return indigo_get_item(&self, const_cast<char *>(item_name));
        })
        .def("get_switch", [](indigo_property &self, const char *item_name) {
            return indigo_get_switch(&self, const_cast<char *>(item_name));
        })
        .def("property_copy_values", [](indigo_property &self, indigo_property *other, bool with_state) {
            indigo_property_copy_values(&self, other, with_state);
        })
        .def("property_copy_targets", [](indigo_property &self, indigo_property *other, bool with_state) {
            indigo_property_copy_targets(&self, other, with_state);
        })
        .def("property_sort_items", [](indigo_property &self) {
            indigo_property_sort_items(&self);
        })
        ;

    pybind11::class_<indigo_device>(scope, "_indigo_device")
        .def(pybind11::init([](py_indigo_device &device) {
            return &device.that;
        }))
        .def_property("name",
            char_array_getter(&indigo_device::name),
            char_array_setter(&indigo_device::name))
        .def_readwrite("lock", &indigo_device::lock)
        .def_readwrite("is_remote", &indigo_device::is_remote)
        .def_readwrite("gp_bits", &indigo_device::gp_bits)
        .def_readwrite("device_context", &indigo_device::device_context)
        .def_readwrite("private_data", &indigo_device::private_data)
        .def_readwrite("master_device", &indigo_device::master_device)
        .def_readwrite("last_result", &indigo_device::last_result)
        .def_readwrite("version", &indigo_device::version)
        .def("attach", [](indigo_device &self, indigo_device *device) {
            return self.attach(device);
        })
        .def("enumerate_properties", [](indigo_device &self, indigo_device *device, indigo_client *client, indigo_property *property) {
            return self.enumerate_properties(device, client, property);
        })
        .def("change_property", [](indigo_device &self, indigo_device *device, indigo_client *client, indigo_property *property) {
            return self.change_property(device, client, property);
        })
        .def("enable_blob", [](indigo_device &self, indigo_device *device, indigo_client *client, indigo_property *property, indigo_enable_blob_mode mode) {
            return self.enable_blob(device, client, property, mode);
        })
        .def("detach", [](indigo_device &self, indigo_device *device) {
            return self.detach(device);
        })
        // alias
        .def("attach", [](indigo_device &self) {
            return indigo_attach_device(&self);
        })
        .def("detach", [](indigo_device &self) {
            return indigo_detach_device(&self);
        })
        .def("define_property", [](indigo_device &self, indigo_property *property, const char *value) {
            return indigo_define_property(&self, property, value);
        })
        .def("update_property", [](indigo_device &self, indigo_property *property, const char *value) {
            return indigo_update_property(&self, property, value);
        })
        .def("delete_property", [](indigo_device &self, indigo_property *property, const char *value) {
            return indigo_delete_property(&self, property, value);
        })
        .def("send_message", [](indigo_device &self, const char *message) {
            return indigo_send_message(&self, message);
        })
        ;
    pybind11::class_<py_indigo_device>(scope, "indigo_device")
        .def(pybind11::init<>())
        .def_property("lock",
            [](py_indigo_device &self) {
                return self.that.lock;
            },
            [](py_indigo_device &self, indigo_glock value) {
                self.that.lock = value;
            })
        .def_property("is_remote",
            [](py_indigo_device &self) {
                return self.that.is_remote;
            },
            [](py_indigo_device &self, bool value) {
                self.that.is_remote = value;
            })
        .def_property("gp_bits",
            [](py_indigo_device &self) {
                return self.that.gp_bits;
            },
            [](py_indigo_device &self, uint16_t value) {
                self.that.gp_bits = value;
            })
        .def_property_readonly("device_context",
            [](py_indigo_device &self) {
                return self.that.device_context;
            })
        .def_property_readonly("private_data",
            [](py_indigo_device &self) {
                return self.that.private_data;
            })
        .def_property_readonly("master_device",
            [](py_indigo_device &self) {
                return self.that.master_device;
            })
        .def_property("last_result",
            [](py_indigo_device &self) {
                return self.that.last_result;
            },
            [](py_indigo_device &self, indigo_result value) {
                self.that.last_result = value;
            })
        .def_property("version",
            [](py_indigo_device &self) {
                return self.that.version;
            },
            [](py_indigo_device &self, indigo_version value) {
                self.that.version = value;
            })
        .def("on_attach", &py_indigo_device::on_attach)
        .def("on_enumerate_properties", &py_indigo_device::on_enumerate_properties)
        .def("on_change_property", &py_indigo_device::on_change_property)
        .def("on_enable_blob", &py_indigo_device::on_enable_blob)
        .def("on_detach", &py_indigo_device::on_detach)
        ;
    pybind11::implicitly_convertible<py_indigo_device, indigo_device>();

    pybind11::class_<indigo_client>(scope, "_indigo_client")
        .def(pybind11::init([](py_indigo_client &client) {
            return &client.that;
        }))
        .def_property("name",
            char_array_getter(&indigo_client::name),
            char_array_setter(&indigo_client::name))
        .def_readwrite("is_remote", &indigo_client::is_remote)
        //.def_readwrite("client_context", &indigo_client::client_context)
        .def_property_readonly("client_context", [](indigo_client &self) {
            return self.client_context;
        })
        .def_readwrite("last_result", &indigo_client::last_result)
        .def_readwrite("version", &indigo_client::version)
        .def_readwrite("enable_blob_mode_records", &indigo_client::enable_blob_mode_records)
        .def("attach", [](indigo_device &self, indigo_device *device) {
            return self.attach(device);
        })
        .def("define_property", [](indigo_client &self, indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
            return self.define_property(client, device, property, message);
        })
        .def("update_property", [](indigo_client &self, indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
            return self.update_property(client, device, property, message);
        })
        .def("delete_property", [](indigo_client &self, indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
            return self.delete_property(client, device, property, message);
        })
        .def("send_message", [](indigo_client &self, indigo_client *client, indigo_device *device, const char *message) {
            return self.send_message(client, device, message);
        })
        .def("detach", [](indigo_client &self, indigo_client *client) {
            return self.detach(client);
        })
        // alias
        .def("attach", [](indigo_client &self) {
            return indigo_attach_client(&self);
        })
        .def("detach", [](indigo_client &self) {
            return indigo_detach_client(&self);
        })
        .def("enumerate_properties", [](indigo_client &self, indigo_property *property) {
            return indigo_enumerate_properties(&self, property);
        })
        .def("change_property", [](indigo_client &self, indigo_property *property) {
            return indigo_change_property(&self, property);
        })
        .def("enable_blob", [](indigo_client &self, indigo_property *property, indigo_enable_blob_mode mode) {
            return indigo_enable_blob(&self, property, mode);
        })
        // TODO: array of (item, value)
        //.def("change_text_property")
        .def("change_text_property_1", [](indigo_client &self, const char *device, const char *name, const char *item, const char *value) {
            return indigo_change_text_property_1(&self, device, name, item, value);
        })
        // TODO: array of (item, value)
        //.def("change_number_property")
        .def("change_number_property_1", [](indigo_client &self, const char *device, const char *name, const char *item, double value) {
            return indigo_change_number_property_1(&self, device, name, item, value);
        })
        // TODO: array of (item, value)
        //.def("change_switch_property")
        .def("change_switch_property_1", [](indigo_client &self, const char *device, const char *name, const char *item, bool value) {
            return indigo_change_switch_property_1(&self, device, name, item, value);
        })
        .def("device_connect", [](indigo_client &self, const char *device) {
            return indigo_device_connect(&self, const_cast<char *>(device));
        })
        .def("device_disconnect", [](indigo_client &self, const char *device) {
            return indigo_device_disconnect(&self, const_cast<char *>(device));
        })
        ;

    pybind11::class_<py_indigo_client>(scope, "indigo_client")
        .def(pybind11::init<>())
        .def_property("name",
            [](py_indigo_client &self) {
                return std::string(self.that.name);
            },
            [](py_indigo_client &self, const std::string &value) {
                set_char_array(self.that.name, value);
            })
        .def_property("is_remote",
            [](py_indigo_client &self) {
                return self.that.is_remote;
            },
            [](py_indigo_client &self, bool value) {
                self.that.is_remote = value;
            })
        .def_property_readonly("client_context",
            [](py_indigo_client &self) {
                return self.that.client_context;
            })
        .def_property("last_result",
            [](py_indigo_client &self) {
                return self.that.last_result;
            },
            [](py_indigo_client &self, indigo_result value) {
                self.that.last_result = value;
            })
        .def_property("version",
            [](py_indigo_client &self) {
                return self.that.version;
            },
            [](py_indigo_client &self, indigo_version value) {
                self.that.version = value;
            })
        .def_property_readonly("enable_blob_mode_records",
            [](py_indigo_client &self) {
                return self.that.enable_blob_mode_records;
            })
        .def("on_attach", &py_indigo_client::on_attach)
        .def("on_define_property", &py_indigo_client::on_define_property)
        .def("on_update_property", &py_indigo_client::on_update_property)
        .def("on_delete_property", &py_indigo_client::on_delete_property)
        .def("on_send_message", &py_indigo_client::on_send_message)
        .def("on_detach", &py_indigo_client::on_detach)
        ;
    pybind11::implicitly_convertible<py_indigo_client, indigo_client>();

    pybind11::class_<indigo_adapter_context>(scope, "indigo_adapter_context")
        .def_readwrite("input", &indigo_adapter_context::input)
        .def_readwrite("output", &indigo_adapter_context::output)
        .def_readwrite("web_socket", &indigo_adapter_context::web_socket)
        .def_property("url_prefix",
            char_array_getter(&indigo_adapter_context::url_prefix),
            char_array_setter(&indigo_adapter_context::url_prefix))
        ;

    pybind11::class_<indigo_blob_entry>(scope, "indigo_blob_entry")
        .def_readwrite("item", &indigo_blob_entry::item)
        .def_readwrite("content", &indigo_blob_entry::content)
        .def_readwrite("size", &indigo_blob_entry::size)
        .def_property("format",
            char_array_getter(&indigo_blob_entry::format),
            char_array_setter(&indigo_blob_entry::format))
        .def_readwrite("mutext", &indigo_blob_entry::mutext)
        ;

    scope.def("indigo_trace", [](const char *msg) {
        indigo_trace("%s", msg);
    });
    scope.def("indigo_debug", [](const char *msg) {
        indigo_debug("%s", msg);
    });
    scope.def("indigo_error", [](const char *msg) {
        indigo_error("%s", msg);
    });
    scope.def("indigo_log", [](const char *msg) {
        indigo_log("%s", msg);
    });
    scope.def("indigo_trace_property", &indigo_trace_property);
    scope.def("indigo_start", &indigo_start);
    scope.def("indigo_set_log_level", &indigo_set_log_level);
    scope.def("indigo_get_log_level", &indigo_get_log_level);
    scope.def("indigo_attach_device", &indigo_attach_device);
    scope.def("indigo_detach_device", &indigo_detach_device);
    scope.def("indigo_attach_client", &indigo_attach_client);
    scope.def("indigo_detach_client", &indigo_detach_client);
    // implicit cast
    scope.def("indigo_attach_device", [](py_indigo_device &device) {
        return indigo_attach_device(device);
    });
    scope.def("indigo_detach_device", [](py_indigo_device &device) {
        return indigo_detach_device(device);
    });
    scope.def("indigo_attach_client", [](py_indigo_client &client) {
        return indigo_attach_client(client);
    });
    scope.def("indigo_detach_client", [](py_indigo_client &client) {
        return indigo_detach_client(client);
    });
    scope.def("indigo_define_property", [](indigo_device *device, indigo_property *property, const char *value) {
        return indigo_define_property(device, property, "%s", value);
    });
    scope.def("indigo_update_property", [](indigo_device *device, indigo_property *property, const char *value) {
        return indigo_update_property(device, property, "%s", value);
    });
    scope.def("indigo_delete_property", [](indigo_device *device, indigo_property *property, const char *value) {
        return indigo_delete_property(device, property, "%s", value);
    });
    scope.def("indigo_send_message", [](indigo_device *device, const char *msg) {
        return indigo_send_message(device, "%s", msg);
    });
    scope.def("indigo_enumerate_properties", &indigo_enumerate_properties);
    scope.def("indigo_change_property", &indigo_change_property);
    scope.def("indigo_enable_blob", &indigo_enable_blob);
    scope.def("indigo_stop", &indigo_stop);
    scope.def("indigo_init_text_property", &indigo_init_text_property);
    scope.def("indigo_init_number_property", &indigo_init_number_property);
    scope.def("indigo_init_switch_property", &indigo_init_switch_property);
    scope.def("indigo_init_light_property", &indigo_init_light_property);
    scope.def("indigo_init_blob_property", &indigo_init_blob_property);
    scope.def("indigo_resize_property", &indigo_resize_property);
    scope.def("indigo_alloc_blob_buffer", &indigo_alloc_blob_buffer);
    scope.def("indigo_release_property", &indigo_release_property);
    scope.def("indigo_validate_blob", &indigo_validate_blob);
    scope.def("indigo_init_text_item", [](indigo_item *item, const char *name, const char *label, const char *value) {
        return indigo_init_text_item(item, name, label, value);
    });
    scope.def("indigo_init_number_item", &indigo_init_number_item);
    // defined by macro
    scope.def("indigo_init_sexagesimal_number_item", [](indigo_item *item, const char *name, const char *label, double min, double max, double step, double value) {
        indigo_init_sexagesimal_number_item(item, name, label, min, max, step, value);
    });
    scope.def("indigo_init_switch_item", &indigo_init_switch_item);
    scope.def("indigo_init_light_item", &indigo_init_light_item);
    scope.def("indigo_init_blob_item", &indigo_init_blob_item);
    scope.def("indigo_populate_http_blob_item", &indigo_populate_http_blob_item);
    scope.def("indigo_property_match", &indigo_property_match);
    scope.def("indigo_switch_match", &indigo_switch_match);
    scope.def("indigo_set_switch", &indigo_set_switch);
    scope.def("indigo_get_item", &indigo_get_item);
    scope.def("indigo_get_switch", &indigo_get_switch);
    scope.def("indigo_property_copy_values", &indigo_property_copy_values);
    scope.def("indigo_property_copy_targets", &indigo_property_copy_targets);
    // TODO: array of (item, value)
    //scope.def("indigo_change_text_property", &indigo_change_text_property);
    scope.def("indigo_change_text_property_1", [](indigo_client *client, const char *device, const char *name, const char *item, const char *value) {
        return indigo_change_text_property_1(client, device, name, item, value);
    });
    // TODO: array of (item, value)
    //scope.def("indigo_change_number_property", &indigo_change_number_property);
    scope.def("indigo_change_number_property_1", &indigo_change_number_property_1);
    // TODO: array of (item, value)
    //scope.def("indigo_change_switch_property", &indigo_change_switch_property);
    scope.def("indigo_change_switch_property_1", &indigo_change_switch_property_1);
    scope.def("indigo_device_connect", &indigo_device_connect);
    scope.def("indigo_device_disconnect", &indigo_device_disconnect);
    scope.def("indigo_trim_local_service", &indigo_trim_local_service);
    scope.def("indigo_async", &indigo_async);
    scope.def("indigo_stod", &indigo_stod);
    scope.def("indigo_dtos", &indigo_dtos);
    scope.def("indigo_usleep", &indigo_usleep);
    scope.def("indigo_atod", &indigo_atod);
    scope.def("indigo_dtoa", &indigo_dtoa);

    scope.def("get_indigo_use_host_suffix", []() {
        return indigo_use_host_suffix;
    });
    scope.def("set_indigo_use_host_suffix", [](bool use) {
        indigo_use_host_suffix = use;
    });
}
