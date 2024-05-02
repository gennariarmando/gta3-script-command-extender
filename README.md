# Script Command Extender for GTA3
Plugin for developers, helps you define new script commands/opcodes.

## Compatibility
Compatible only with GTA3 1.0

## Usage
```C++
#include "ScmExtender.h"
#include "CRunningScript.h"
...
      // Example of a custom script command.
      // Command id should be an unused one, see script commands list for current game,
      // eventually it's also possible to replace an already existing one.
      const int32_t NEW_COMMAND_ID = 1475;
      ScmExtender::AddOneCommand(NEW_COMMAND_ID, [](int32_t* params) -> int8_t {
          // ...
          
          return 0;
      });
...

```

# How to install
Grab [latest](https://github.com/gennariarmando/gta3-script-command-extender/releases) archive. \
Move ScriptCommandExtenderIII.asi in the root or "scripts" folder of your GTA3 directory. \
Enjoy

