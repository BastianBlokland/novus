# Vscode Novus extension

Vscode extension containing language support for the Novus language.
At the moment only basic syntax highlighting is supported.

## Installation
* Copy the `novus` directory into your `<user home>/.vscode/extensions` directory.
* Reload vscode.

## Problem-matcher
The extension includes a problem-matcher named `novus` to report problems based on diagnostic output
from the nov compiler.

Create a task in your `.vscode/tasks.json` file for compiling a novus file:
Replace `<<Path to novus compiler binary>>` to where the `novc` executable is located.
```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "compile",
      "command": "<<Path to novus novc executable>>",
      "type": "shell",
      "args": [
        "${file}"
      ],
      "presentation": {
        "echo": true,
        "panel": "shared",
        "clear": true
      },
      "problemMatcher": "$novus"
    }
  ]
}
```

Alternatively you can create a task that automatically compiles files in the background:
```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "watch",
      "type": "shell",
      "command": "${workspaceFolder}/scripts/watch_ns.sh",
      "args": [
        "${workspaceFolder}",
        "${workspaceFolder}/bin/novc"
      ],
      "isBackground": true,
      "problemMatcher": "$novus",
    }
  ]
}
```
The `scripts/watch_ns.sh` bash script uses `inotifywait` to detect file changes.
