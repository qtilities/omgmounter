- replace desktop files with generated. Do we need this app as "service"?
- StartupType=Unique
```cpp
// Allow only one application instance
const KDBusService service(KDBusService::Unique);
```
- command line help system strings are not translated
