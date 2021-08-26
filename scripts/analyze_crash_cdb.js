"use strict";

const execute = cmd => host.namespace.Debugger.Utility.Control.ExecuteCommand(cmd);
const log  = msg => host.diagnostics.debugLog(`${msg}\n`);

function runCommand(command)
{

    log(command);

    var ret = "";
    var ctl = host.namespace.Debugger.Utility.Control;
    try
    {
    var output = ctl.ExecuteCommand(command);
    for (var line of output)
    {
        ret += line;
        ret += '\n';
    }
    }
    catch (e)
    {
        ret += 'error';
    }
    return ret;
}

function search_code(start, size)
{
    var ptrs = host.memory.readMemoryValues(start, size, 8);
    for (var i = 0; i < size; i++)
    {
        var address = start + i * 8;
        var ptr = ptrs[i];
        var nearSymbol = execute(`ln ${ptr}`);
        if (nearSymbol[0].toString().length > 0)
        {
            return address.toString(16)
        }		   
    }
    return false
}

function analyzeCrash(exceptionPointer)
{
    var data = host.memory.readMemoryValues(exceptionPointer, 2, 8);
    log('--------------------------------')
    log('Exception Data\n')
    log(runCommand(`.exr ${data[1]}`))

    // Search stack
    var stackStart = host.memory.readMemoryValues(data[0]+0x20, 1, 8)[0];
   
    var newStack = search_code(stackStart, 60);
    if (!newStack){
        stackStart = host.memory.readMemoryValues(data[0]+0x28, 1, 8)[0];
        newStack = search_code(stackStart, 60);
    }

    log('--------------------------------')
    log('Stack\n')

    if (newStack){
        log(runCommand(`k = ${newStack} 20`))
        log('--------------------------------')
        return true
    }
    else{
        log("[!] Failed to find stack")
        log('--------------------------------')
        return false
    }
}

function initializeScript()
{
    log(`Usage: 
            !analyzeCrash <exception_address>`)
    return [
        new host.apiVersionSupport(1, 3),
        new host.functionAlias(analyzeCrash , "analyzeCrash")
        ];
}

