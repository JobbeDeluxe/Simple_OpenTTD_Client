param(
    [string]$Directory = $env:VCPKG_ROOT,
    [string[]]$Triplet,
    [switch]$NoInstall
)

$repoRoot = Split-Path -Parent $PSScriptRoot
if (-not $Directory) {
    $Directory = Join-Path $repoRoot '.vcpkg'
}

$parent = Split-Path -Parent $Directory
if ($parent) {
    $null = New-Item -ItemType Directory -Path $parent -Force
}

$gitDir = Join-Path $Directory '.git'
if (-not (Test-Path $gitDir)) {
    Write-Host "Cloning vcpkg into $Directory"
    git clone https://github.com/microsoft/vcpkg.git $Directory
} else {
    Write-Host "Updating existing vcpkg checkout in $Directory"
    git -C $Directory pull --ff-only
}

$vcpkgExe = Join-Path $Directory 'vcpkg.exe'
if (-not (Test-Path $vcpkgExe)) {
    Write-Host 'Bootstrapping vcpkg'
    $bootstrapBat = Join-Path $Directory 'bootstrap-vcpkg.bat'
    $bootstrapSh = Join-Path $Directory 'bootstrap-vcpkg.sh'
    if (Test-Path $bootstrapBat) {
        & $bootstrapBat -disableMetrics
    } elseif (Test-Path $bootstrapSh) {
        & $bootstrapSh -disableMetrics
    } else {
        throw "Unable to locate vcpkg bootstrap script in $Directory"
    }
} else {
    Write-Host 'vcpkg already bootstrapped'
}

if (-not (Test-Path $vcpkgExe)) {
    $vcpkgExe = Join-Path $Directory 'vcpkg'
}
if (-not (Test-Path $vcpkgExe)) {
    throw "Unable to find vcpkg executable in $Directory"
}

if (-not $NoInstall) {
    if (-not $Triplet -or $Triplet.Count -eq 0) {
        $Triplet = @('x64-windows')
    }
    foreach ($t in $Triplet) {
        Write-Host "Installing dependencies for triplet: $t"
        & $vcpkgExe install --triplet $t
    }
}

Write-Host "All done. Set VCPKG_ROOT to $Directory or add it to your profile."
