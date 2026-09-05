$ErrorActionPreference = 'Stop'
$seconds = if ($args.Count -ge 1) { [int]$args[0] } else { 30 }

$target = Get-CimInstance Win32_Process -Filter "Name='explorer.exe'" |
    Where-Object { $_.CommandLine -like '*network-lounge*' } |
    Select-Object -First 1
if (-not $target) { Write-Output 'tool-mod process not found'; exit 1 }

$p = Get-Process -Id $target.ProcessId
$cpu0 = $p.TotalProcessorTime.TotalSeconds
$ws0 = $p.WorkingSet64
$handles0 = $p.HandleCount
$gdi0 = [int](Get-CimInstance Win32_Process -Filter "ProcessId=$($p.Id)").HandleCount

Start-Sleep -Seconds $seconds

$p.Refresh()
$cpu1 = $p.TotalProcessorTime.TotalSeconds
$ws1 = $p.WorkingSet64
$handles1 = $p.HandleCount

$cores = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
$cpuPct = (($cpu1 - $cpu0) / $seconds) * 100
$cpuPctNorm = $cpuPct / $cores

"pid            : $($p.Id)"
"window         : $seconds s, $cores logical cores"
"cpu delta      : $([math]::Round($cpu1-$cpu0,3)) s  =>  $([math]::Round($cpuPct,2))% of one core  /  $([math]::Round($cpuPctNorm,3))% of total CPU"
"working set    : $([math]::Round($ws0/1MB,2)) MB -> $([math]::Round($ws1/1MB,2)) MB (delta $([math]::Round(($ws1-$ws0)/1KB,1)) KB)"
"handles        : $handles0 -> $handles1 (delta $($handles1-$handles0))"
"threads        : $($p.Threads.Count)"

# GDI + USER object counts: the real leak detector for a GDI+ drawing widget.
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public class Gui {
    [DllImport("user32.dll")] public static extern uint GetGuiResources(IntPtr h, uint flags);
    [DllImport("kernel32.dll")] public static extern IntPtr OpenProcess(uint a, bool inherit, int pid);
    [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr h);
    public static string Counts(int pid) {
        IntPtr h = OpenProcess(0x1000, false, pid); // PROCESS_QUERY_LIMITED_INFORMATION
        if (h == IntPtr.Zero) return "open failed " + Marshal.GetLastWin32Error();
        uint gdi = GetGuiResources(h, 0);   // GR_GDIOBJECTS
        uint user = GetGuiResources(h, 1);  // GR_USEROBJECTS
        CloseHandle(h);
        return "gdi=" + gdi + " user=" + user;
    }
}
'@
"gui objects    : $([Gui]::Counts($p.Id))"
