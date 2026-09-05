$ErrorActionPreference = 'Stop'
# Rigorous accuracy check: integrate Windows' own "Bytes Received/sec" perf
# counter over a window and compare the total with what the mod's sampler
# counted over the same window. Both measure the same NIC octets, so the two
# totals should agree within a few percent.
#
# Usage: powershell -File accuracy_integral.ps1 [seconds]

$seconds = if ($args.Count -ge 1) { [int]$args[0] } else { 20 }
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$build = Join-Path $root 'build'

$adapter = Get-NetAdapter | Where-Object Status -eq 'Up' | Select-Object -First 1
$instance = $adapter.InterfaceDescription -replace '[#\\\/\(\)]', '_'
$paths = @(
    "\Network Interface($instance)\Bytes Received/sec",
    "\Network Interface($instance)\Bytes Sent/sec"
)
Write-Output "adapter : $($adapter.Name)"
Write-Output "window  : $seconds s"

# Start the mod's own sampler (same code path as the mod, stubbed Windhawk API).
$acc = Start-Process -FilePath (Join-Path $build 'test_accuracy.exe') `
    -ArgumentList @($seconds, 0) -PassThru -NoNewWindow `
    -RedirectStandardOutput (Join-Path $build 'integral_mod.txt')

Start-Sleep -Milliseconds 900

# Traffic generator.
$job = Start-Job -ScriptBlock {
    for ($i = 0; $i -lt 8; $i++) {
        try {
            (New-Object Net.WebClient).DownloadFile(
                'https://codeload.github.com/ramensoftware/windhawk-mods/zip/refs/heads/main',
                "$env:TEMP\int_acc_$i.zip")
        } catch {}
    }
}

# Integrate the perf counter for the same window.
$counters = Get-Counter -Counter $paths -SampleInterval 1 -MaxSamples ($seconds - 2)
$sumDown = 0.0
$sumUp = 0.0
foreach ($c in $counters) {
    $sumDown += $c.CounterSamples[0].CookedValue
    $sumUp += $c.CounterSamples[1].CookedValue
}

$acc.WaitForExit()
Remove-Job -Job $job -Force -ErrorAction SilentlyContinue | Out-Null
Get-ChildItem "$env:TEMP\int_acc_*.zip" -ErrorAction SilentlyContinue | Remove-Item -Force

$modOut = Get-Content (Join-Path $build 'integral_mod.txt')
$modDown = [double](($modOut | Select-String 'counted_down_bytes=(\d+)').Matches.Groups[1].Value)
$modUp = [double](($modOut | Select-String 'counted_up_bytes=(\d+)').Matches.Groups[1].Value)
$peak = ($modOut | Select-String 'peak_down_formatted=(.+)').Matches.Groups[1].Value

Write-Output ''
Write-Output ("perf counter integral : down={0,12:N0} B   up={1,12:N0} B" -f $sumDown, $sumUp)
Write-Output ("mod sampler total     : down={0,12:N0} B   up={1,12:N0} B" -f $modDown, $modUp)
if ($sumDown -gt 0) {
    Write-Output ("ratio mod/perf        : down={0:N4}   up={1:N4}" -f ($modDown / $sumDown), ($modUp / $sumUp))
}
Write-Output "mod peak download     : $peak"
