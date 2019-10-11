#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <indigo/indigo_client.h>

#include "common.hpp"

struct indigo_server_entry_deleter {
    void operator ()(indigo_server_entry *entry) {
        indigo_disconnect_server(entry);
    }
};

void init_indigo_client(pybind11::module &scope) {
#if defined(INDIGO_LINUX) || defined(INDIGO_MACOS)
    pybind11::class_<indigo_driver_entry>(scope, "indigo_driver_entry")
        .def_property("description",
            char_array_getter(&indigo_driver_entry::description),
            char_array_setter(&indigo_driver_entry::description))
        .def_property("name",
            char_array_getter(&indigo_driver_entry::name),
            char_array_setter(&indigo_driver_entry::name))
        // TODO: void *
        //.def_readonly("driver", &indigo_driver_entry::driver)
        //.def_readonly("dl_handle", &indigo_driver_entry::dl_handle)
        .def_readwrite("initialized", &indigo_driver_entry::initialized)
        ;
    pybind11::class_<indigo_subprocess_entry>(scope, "indigo_subprocess_entry")
        .def_property("executable",
            char_array_getter(&indigo_subprocess_entry::executable),
            char_array_setter(&indigo_subprocess_entry::executable))
        .def_readwrite("thread", &indigo_subprocess_entry::thread)
        .def_readwrite("thread_started", &indigo_subprocess_entry::thread_started)
        .def_readwrite("pid", &indigo_subprocess_entry::pid)
        .def_readwrite("protocol_adapter", &indigo_subprocess_entry::protocol_adapter)
        .def_property("last_error",
            char_array_getter(&indigo_subprocess_entry::last_error),
            char_array_setter(&indigo_subprocess_entry::last_error))
        ;
    // TODO: indigo_driver_entry **
    //scope.def("indigo_add_driver", &indigo_add_driver);
    scope.def("indigo_remove_driver", &indigo_remove_driver);
    // TODO: indigo_driver_entry **
    //scope.def("indigo_load_driver", &indigo_load_driver);
    //scope.def("indigo_start_subprocess", &indigo_start_subprocess);
    scope.def("indigo_kill_subprocess", &indigo_kill_subprocess);
#endif // INDIGO_LINUX || INDIGO_MACOS

    pybind11::class_<indigo_server_entry,
            std::unique_ptr<indigo_server_entry, indigo_server_entry_deleter>
        >(scope, "indigo_server_entry")
        .def_property("name",
            char_array_getter(&indigo_server_entry::name),
            char_array_setter(&indigo_server_entry::name))
        .def_property("host",
            char_array_getter(&indigo_server_entry::host),
            char_array_setter(&indigo_server_entry::host))
        .def_readwrite("port", &indigo_server_entry::port)
        .def_readwrite("thread", &indigo_server_entry::thread)
        .def_readwrite("thread_started", &indigo_server_entry::thread_started)
        .def_readwrite("socket", &indigo_server_entry::socket)
        .def_readwrite("protocol_adapter", &indigo_server_entry::protocol_adapter)
        .def_property("last_error",
            char_array_getter(&indigo_server_entry::last_error),
            char_array_setter(&indigo_server_entry::last_error))
        ;

    scope.def("indigo_service_name", &indigo_service_name);
    scope.def("indigo_connect_server", [](const char *name, const char *host, int port) {
        indigo_server_entry *entry = nullptr;
        auto ret = indigo_connect_server(name, host, port, &entry);
        return std::make_tuple(ret, std::unique_ptr<indigo_server_entry, indigo_server_entry_deleter>(entry, indigo_server_entry_deleter()));
    });
    scope.def("indigo_disconnect_server", &indigo_disconnect_server);
}
