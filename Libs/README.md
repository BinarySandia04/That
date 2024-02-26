# Zag libraries

Every library contains a `package.toml` file describing all bindings to it

All bindings must be inside the _<_name> namespace
and all files must be placed inside the 'src' directory

```
[_info] # Information about the library
_name = <string> # Name of the library. Must be unique
_display_name = <string> # Display name of the library
_version = <string> # (OPTIONAL) Version of the library
_namespace = <string> # (OPTIONAL) redifines cpp namespace 
_root = <string> # Root object for the lib bindings. Can leave empty if the library doesn't need to create an object
_file_deps = <Arr<string>> # (OPTIONAL) additional .cpp file deps for the library
_requires = <Arr<string>> # (OPTIONAL) _name of libs that this lib requires

[a.b.c.d]
_function = {
    # Function object
    _cname = <string> # Name of the cpp function to be binded
    _args = <Arr<string>> # Type of args ordered
}

[_conversions.<name>]
_cname = <string> # Name of the cpp function to be binded 
_from = <string> # From type (1rst arg)
_to = <string> # To type (2nd arg)

[_types.<internal_name>]
_name = <string> # Name inside the language 
_cname = <string> # Name of the class binded

[_types.<internal_name>.a]
_function = { ... }

```
