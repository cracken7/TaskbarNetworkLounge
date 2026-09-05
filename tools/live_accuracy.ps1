$ErrorActionPreference = 'Stop'
# Live accuracy comparison: sample Windows' own "Network Interface" performance
# counters (the same source Task Manager's Performance tab uses) while capturing
# what the widget renders, so the two can be compared directly.
#
# Usage: powershell -File live_accuracy.ps1 [samples]

$samples = if ($args.Count -ge 1) { [int]$args[0] } else { 12 }
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$build = Join-Path $root 'build'

$adapter = Get-NetAdapter | Where-Object Status -eq 'Up' | Select-Object -First 1
if (-not $adapter) { Write-Output 'no adapter is up'; exit 1 }
$instance = $adapter.InterfaceDescription -replace '[#\\\/\(\)]', '_'
Write-Output "adapter: $($adapter.Name) / $instance"

$paths = @(
    "\Network Interface($instance)\Bytes Received/sec",
    "\Network Interface($instance)\Bytes Sent/sec"
)

# Kick off download traffic in the background so there is something to measure.
$job = Start-Job -ScriptBlock {
    for ($i = 0; $i -lt 6; $i++) {
        try {
            (New-Object Net.WebClient).DownloadFile(
                'https://codeload.github.com/ramensoftware/windhawk-mods/zip/refs/heads/main',
                "$env:TEMP\live_acc_$i.zip")
        } catch {}
    }
}

Start-Sleep -Seconds 2

$rows = @()
for ($i = 1; $i -le $samples; $i++) {
    $counter = Get-Counter -Counter $paths -SampleInterval 1 -MaxSamples 1
    $down = [math]::Round($counter.CounterSamples[0].CookedValue, 0)
    $up = [math]::Round($counter.CounterSamples[1].CookedValue, 0)
    $shot = Join-Path $build ("acc_{0:d2}.png" -f $i)
    & (Join-Path $build 'shot.exe') $shot 12 1022 200 48 | Out-Null
    $rows += [pscustomobject]@{ n = $i; perfDown = $down; perfUp = $up; shot = (Split-Path -Leaf $shot) }
    Write-Output ("sample {0,2}: perfcounter down={1,10:N0} B/s  up={2,9:N0} B/s  -> {3}" -f $i, $down, $up, (Split-Path -Leaf $shot))
}

Remove-Job -Job $job -Force -ErrorAction SilentlyContinue | Out-Null
Get-ChildItem "$env:TEMP\live_acc_*.zip" -ErrorAction SilentlyContinue | Remove-Item -Force
$rows | Export-Csv -NoTypeInformation -Path (Join-Path $build 'live_accuracy.csv')
Write-Output "csv: $(Join-Path $build 'live_accuracy.csv')"
