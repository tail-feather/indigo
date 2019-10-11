# indigo_astronomy

## How to build

```
pip install -r requirements.txt
python setup.py build
```

## Usage

### connect server

```python
>>> import indigo_astronomy
>>> res, entry = indigo_astronomy.indigo_connect_server(None, "127.0.0.1", 7624)
>>> res, entry
(indigo_result.INDIGO_OK, <indigo_astronomy.__indigo.indigo_server_entry object at 0x7fe7063d31f0>)
```

### attach client

```python
>>> import indigo_astronomy
>>> class client(indigo_astronomy.indigo_client):
...     def __init__(self):
...         super().__init__()
...         self.name = "Test"
...         self.is_remote = False
...         self.last_result = indigo_astronomy.INDIGO_OK
...         self.version = indigo_astronomy.INDIGO_VERSION_CURRENT
...     def on_attach(self, client):
...         print("on_attach: client:{!r}".format(client.name))
...     def on_define_property(self, client, device, prop, value):
...         print("on_define_property: client:{!r}, device:{!r}, property:{!r}, value:{!r}".format(client.name, device.name, prop.name, value))
...     def on_update_property(self, client, device, prop, value):
...         print("on_update_property: client:{!r}, device:{!r}, property:{!r}, value:{!r}".format(client.name, device.name, prop.name, value))
...     def on_delete_property(self, client, device, prop, value):
...         print("on_delete_property: client:{!r}, device:{!r}, property:{!r}, value:{!r}".format(client.name, device.name, prop.name, value))
...     def on_send_message(self, client, device, message):
...         print("on_send_message: client:{!r}, device:{!r}, message:{!r}".format(client.name, device.name, message))
...     def on_detach(self, client):
...         print("on_detach: client:{!r}".format(client.name))
... 
>>> c = client()
>>> indigo_astronomy.indigo_attach_client(c)
on_attach: client:'Test'
indigo_result.INDIGO_OK
>>> ret, entry = indigo_astronomy.indigo_connect_server(None, "127.0.0.1", 7624)
>>> ret, entry
(indigo_result.INDIGO_OK, <indigo_astronomy.__indigo.indigo_server_entry object at 0x7fd637caca78>)
>>> import time; time.sleep(1)
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'INFO', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'DRIVERS', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'LOAD', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'UNLOAD', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'RESTART', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'LOG_LEVEL', value:None
on_define_property: client:'Test', device:'@ 127.0.0.1', property:'FEATURES', value:None
>>> indigo_astronomy.indigo_detach_client(c)
on_detach: client:'Test'
indigo_result.INDIGO_OK
>>> 
```

